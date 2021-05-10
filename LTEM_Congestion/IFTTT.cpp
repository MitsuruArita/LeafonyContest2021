#include "IFTTT.h"
#include "Arduino.h"
#include <Lpwa.h>

const char GPRS_APN[] = "soracom.io";
const char GPRS_LOGIN[] = "sora";
const char GPRS_PASSWORD[] = "sora";

char serverAddress[] = "maker.ifttt.com"; // server address
int port = 80;
LpwaHttpClient client = LpwaHttpClient(serverAddress, port);

GPRS gprs;
LpwaAccess lpwaAccess;

IFTTT::IFTTT()
{
  connected = false;
}

bool IFTTT::ConnectGPRS()
{
  //タイムアウトは？
  connected = false;
  while (!connected) {
    if ((lpwaAccess.begin() == LPWA_READY) &&
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY))
    {
      connected = true;
    }
    else
    {
      Serial.println("connecting...");
      delay(1000);
    }
  }
  return connected;
}

void IFTTT::Post(String path, String contentType, String postData)
{
  client.clrHeader();
  client.addHeader("Accept", "*/*");
  client.post(path.c_str(), contentType, postData);
}

int IFTTT::getStatusCode()
{
  return client.responseStatusCode();
}

String IFTTT::getResponse()
{
  return client.responseBody();
}

void IFTTT::end()
{
  lpwaAccess.end();
}
