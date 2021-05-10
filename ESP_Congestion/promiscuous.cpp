#include "promiscuous.h"
#include <string.h>

esp_err_t event_handler(void *ctx, system_event_t *event)
{
  return ESP_OK;
}

void Promiscuous::init(void)
{
  nvs_flash_init();
  tcpip_adapter_init();
  ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
  ESP_ERROR_CHECK( esp_wifi_set_country(&wifi_country) ); /* set country for channel range [1, 13] */
  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL) );
  ESP_ERROR_CHECK( esp_wifi_start() );
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
}

void Promiscuous::setChannel(uint8_t channel)
{
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

const char *wifi_sniffer_packet_type2str(wifi_promiscuous_pkt_type_t type)
{
  switch (type) {
    case WIFI_PKT_MGMT: return "MGMT";
    case WIFI_PKT_DATA: return "DATA";
    default:
    case WIFI_PKT_MISC: return "MISC";
  }
}

void wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type)
{
  if (type != WIFI_PKT_MGMT)
    return;

  const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
  const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
  const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

  bool isDuplicate;
  char receiverAddress[13];

  sprintf(receiverAddress,
          "%02x%02x%02x%02x%02x%02x",
          hdr->addr1[0], hdr->addr1[1], hdr->addr1[2],
          hdr->addr1[3], hdr->addr1[4], hdr->addr1[5]
         );
  //プローブリクエスト信号の場合
  if (strcmp(receiverAddress, "ffffffffffff") == 0 && (ppkt->rx_ctrl.rssi) > -70) {
    char senderAddress[13];
    sprintf(senderAddress,
            "%02x%02x%02x%02x%02x%02x",
            hdr->addr2[0], hdr->addr2[1], hdr->addr2[2],
            hdr->addr2[3], hdr->addr2[4], hdr->addr2[5]
           );
    isDuplicate = false;
    for (int i = 0; i < addressIndex; i++) {
      //被っていたら登録しない
      if (strcmp(senderAddress, clientAddress[i]) == 0) {
        isDuplicate = true;
        break;
      }
    }
    //被ってなかったら登録
    if (!isDuplicate) {
      //printf("address:%s rssi:%02d\n", senderAddress, ppkt->rx_ctrl.rssi);
      strcpy(clientAddress[addressIndex++], senderAddress);
    }
  }
}

void Promiscuous::clearAddress(void)
{
  for (int i = 0; i < 64; i++) {
    strcpy(clientAddress[i], "");
  }
  addressIndex = 0;
}
uint8_t Promiscuous::getProbeNum(void)
{
  return addressIndex;
}

bool Promiscuous::switchChannel(void)
{
  channel = (channel % WIFI_CHANNEL_MAX) + 1;
}

uint8_t Promiscuous::getChannel(void)
{
  return channel;
}
