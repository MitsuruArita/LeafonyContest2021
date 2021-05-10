#include "promiscuous.h"
#include "CocoaCounter.h"
//#include "freertos/FreeRTOS.h"
#include <Wire.h>
#include <WireSlave.h>

//わからん
#define TO_LTE_LEAF     25  //leaf pin 24
#define FROM_LTE_LEAF   26   //leaf pin 22
//#define LED             26

Promiscuous prom = Promiscuous();
CocoaCounter cocoa;
bool _once = true;
byte send_data[2] = {0};

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}
int get_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;
  return esp_sleep_get_wakeup_cause();
}

void setup()
{
  Serial.begin(115200);
  Serial.println("hai");
  //while (!Serial);
  //pinMode(LED, OUTPUT);
  pinMode(FROM_LTE_LEAF, INPUT_PULLUP);
  pinMode(TO_LTE_LEAF, OUTPUT);
  digitalWrite(TO_LTE_LEAF, HIGH);
  //digitalWrite(LED, HIGH);
  print_wakeup_reason();
  delay(2000);
  //初回起動の時は即座にスリープ
  if ((get_wakeup_reason() != ESP_SLEEP_WAKEUP_EXT0) || digitalRead(FROM_LTE_LEAF) == HIGH) {
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_26, 0);
    Serial.println("sleep");
    //digitalWrite(LED, LOW);
    //スリープ
    esp_deep_sleep_start();
  }
  Serial.println("wake up");
  //LTEから起こされたときはスキャンする
  prom.init();
  cocoa.init();
}
void requestEvent()
{
  //人数送信
  WireSlave1.write(send_data, 2);
}
void loop()
{
  if (_once) {
    Serial.println("Sniffer Start!!");
    while (prom.getChannel() < WIFI_CHANNEL_MAX) {
      Serial.print("channel:");
      Serial.println(prom.getChannel());
      vTaskDelay(WIFI_CHANNEL_SWITCH_INTERVAL / portTICK_PERIOD_MS);
      prom.switchChannel();
      delay(4000);
    }
    Serial.print("Probe Num: ");
    Serial.println(prom.getProbeNum());
    send_data[0] = prom.getProbeNum() & 0xFF;
    prom.clearAddress();
    Serial.println("Scan Start!!");
    cocoa.Scan(5);
    Serial.print("Cocoa Num: ");
    Serial.println(cocoa.getCocoaNum());
    send_data[1] = cocoa.getCocoaNum() & 0xFF;
    cocoa.Clear();
    //スキャン終了
    Serial.println(send_data[0]);
    Serial.println(send_data[1]);
    if(!WireSlave1.begin(21, 22, 0x5f)){
      Serial.println("failed");
    }
    WireSlave1.onRequest(requestEvent);
    digitalWrite(TO_LTE_LEAF, LOW);
    _once = false;
  }
  else if (digitalRead(FROM_LTE_LEAF) == HIGH) {
    Serial.println("sleep again");
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_26, 0);
    esp_deep_sleep_start();
  }
  else{
    WireSlave1.update();
  }
}
