#include <Lpwa.h>

class IFTTT
{
  public:
    IFTTT();
    bool ConnectGPRS();
    void Post(String path, String contentType, String postData);
    int getStatusCode();
    String getResponse();
    void end();
  private:
  //大きさ明示しないとエラー出る？？
    char GPRS_APN[11] = "soracom.io";
    char GPRS_LOGIN[5] = "sora";
    char GPRS_PASSWORD[5] = "sora";
    /*
    char serverAddress[16] = "maker.ifttt.com";
    int port = 80;
    LpwaHttpClient client = LpwaHttpClient(serverAddress, port);
    GPRS gprs;
    LpwaAccess lpwaAccess;
    */
    bool connected;
};
