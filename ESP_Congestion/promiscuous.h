#ifndef _PROMISCUOUS_H_
#define _PROMISCUOUS_H_

#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#define WIFI_CHANNEL_SWITCH_INTERVAL (1000)
#define WIFI_CHANNEL_MAX (13)

//uint8_t level = 0;
static wifi_country_t wifi_country = {.cc = "CN", .schan = 1, .nchan = 13}; //Most recent esp32 library struct

static char clientAddress[40][13];
static uint8_t addressIndex = 0;

static esp_err_t event_handler(void *ctx, system_event_t *event);
static const char *wifi_sniffer_packet_type2str(wifi_promiscuous_pkt_type_t type);
static void wifi_sniffer_packet_handler(void *buff, wifi_promiscuous_pkt_type_t type);

typedef struct {
  unsigned frame_ctrl: 16;
  unsigned duration_id: 16;
  uint8_t addr1[6]; /* receiver address */
  uint8_t addr2[6]; /* sender address */
  uint8_t addr3[6]; /* filtering address */
  unsigned sequence_ctrl: 16;
  uint8_t addr4[6]; /* optional */
} wifi_ieee80211_mac_hdr_t;

typedef struct {
  wifi_ieee80211_mac_hdr_t hdr;
  uint8_t payload[0]; /* network data ended with 4 bytes csum (CRC32) */
} wifi_ieee80211_packet_t;

class Promiscuous {
  public:
    Promiscuous(): channel(1), scanTime(5) {}
    void init();
    void setChannel(uint8_t channel);
    void clearAddress();
    uint8_t getProbeNum();
    bool switchChannel();
    uint8_t getChannel();
  private:
    uint8_t channel;
    int scanTime;
};

#endif
