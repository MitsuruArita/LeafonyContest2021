#include "AlarmSleep.h"
#include "IFTTT.h"
#include "ArduinoJson.h"
#include "Seeed_BME280.h"
#include <Wire.h>
#include <WireSlaveRequest.h>
#include <Lpwa.h>

LpwaCtrl pmctrl;
IFTTT ifttt;
AlarmSleep _alarm(30);

BME280 bme280;

bool waitingESP = false;

void setup()
{
  //while(1)
  //pmctrl.enableLeaf(true);
  Serial.begin(115200);
  //while (!Serial);
  pmctrl.setLed(false, false, false);
  pmctrl.enableLeaf(true);
  pinMode(TO_ESP, OUTPUT);
  pinMode(FROM_ESP, INPUT_PULLUP);
  digitalWrite(TO_ESP, HIGH);
}

void loop()
{
  /*
    pmctrl.setLed(false, false, false);
    delay(10000);
    function(NULL);
  */
  byte espData[2] = {0};
  if (waitingESP == false) {
    pmctrl.enableLeaf(true);
    pinMode(TO_ESP, OUTPUT);
    pinMode(FROM_ESP, INPUT_PULLUP);
    digitalWrite(TO_ESP, HIGH);
    pmctrl.setLed(false, false, false);
    Serial.println("virtual sleep");
    //アラームつけてスリープ
    /*
    delay(10000);
    Serial.println("wake up");
    function(NULL);
    */
    _alarm.alarmSleep(function);
    pmctrl.setLed(true, false, false);
    waitingESP = true;
    Wire.setSDA(I2C2_SDA);
    Wire.setSCL(I2C2_SCL);
    Wire.begin();
  }
  else {
    //ESPがスキャン終了したら
    if (digitalRead(FROM_ESP) == LOW) {
      Serial.println("from esp: ");
      pmctrl.setLed(true, true, false);
      int data_num = 0;
      WireSlaveRequest slaveReq(Wire, 0x5f, 32);
      slaveReq.setRetryDelay(20);
      bool success = slaveReq.request();
      if (success) {
        while (0 < slaveReq.available()) {
          espData[(data_num++) % 2] = slaveReq.read();
          //Serial.println(slaveReq.read());
        }
        //Serial.println(slaveReq.read());
      }
      else {
        Serial.println(slaveReq.lastStatusToString());
      }

      /*
        Wire.requestFrom(0x5f, 2);
        //cocoaとwifiのデータをうけとる
        while (Wire.available()) {
        espData[(data_num++) % 2] = Wire.read();
        }
      */
      Serial.println(espData[0]);
      //ESPにスリープ信号おくる
      digitalWrite(TO_ESP, HIGH);
      //IFTTTにポスト
      postIFTTT(espData);
      //スリープするｚｚｚｚｚｚｚｚｚ
      waitingESP = false;
    }
  }
}

void function(void *data)
{
  _alarm.wakeupSequence();
  pmctrl.setLed(true, true, true);

  //gprsに接続
  ifttt.ConnectGPRS();
  /*
    //esp32を待つ
    if (ifttt.ConnectGPRS() == true) {
    bool batt_low = pmctrl.getBattLevel() < 3500;
    int env_sensors[3] = {0};
    env_sensors[0] = (int)(bme280.getTemperature());
    env_sensors[1] = (int)(bme280.getPressure());
    env_sensors[2] = (int)(bme280.getHumidity());
    StaticJsonDocument<200> doc;
    String jsonString = "";
    doc["value1"] = batt_low;
    doc["value2"][0] = env_sensors[0];
    doc["value2"][1] = env_sensors[1];
    doc["value2"][2] = env_sensors[2];
    doc["value3"][0] = 4;
    doc["value3"][1] = 5;
    serializeJson(doc, jsonString);
    Serial.println(jsonString);
    pmctrl.setLed(false, true, true);
    ifttt.Post("/trigger/lte/with/key/d4tbmn0b4e8FB0qEkuKrZL", "application/json", jsonString);
    pmctrl.setLed(false, false, true);
    Serial.print("Status code: ");
    Serial.println(ifttt.getStatusCode());
    Serial.print("Response: ");
    Serial.println(ifttt.getResponse());
    ifttt.end();
    }
    delay(2000);
  */
}

void postIFTTT(byte *espData)
{
  if (!bme280.init()) {
    Serial.println("Device error!!");
  }
  bool batt_low = pmctrl.getBattLevel() < 3500;
  int env_sensors[3] = {0};
  env_sensors[0] = (int)(bme280.getTemperature());
  env_sensors[1] = (int)(bme280.getPressure());
  env_sensors[2] = (int)(bme280.getHumidity());
  StaticJsonDocument<200> doc;
  String jsonString = "";
  doc["value1"] = batt_low;
  doc["value2"][0] = env_sensors[0];
  doc["value2"][1] = env_sensors[1];
  doc["value2"][2] = env_sensors[2];
  doc["value3"][0] = espData[0];
  doc["value3"][1] = espData[1];
  serializeJson(doc, jsonString);
  Serial.println(jsonString);
  pmctrl.setLed(false, true, true);
  ifttt.Post("/trigger/leaf/with/key/bMNUZoZRQND06rCRjcVHUK", "application/json", jsonString);
  pmctrl.setLed(false, false, true);
  Serial.print("Status code: ");
  Serial.println(ifttt.getStatusCode());
  Serial.print("Response: ");
  Serial.println(ifttt.getResponse());
  ifttt.end();
}
