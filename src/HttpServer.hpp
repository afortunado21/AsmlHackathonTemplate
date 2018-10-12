// HTTP server

#ifndef __Facilities_HTTP_Server__
#define __Facilities_HTTP_Server__

#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#define SERVER_HOST_NAME "yellow-team"

#define TCP_PORT 9000
#define DNS_PORT 53

namespace Facilities {

class HttpServer
{

public:
   HttpServer();
   ~HttpServer() {};

   // Disallow copy-ing
	HttpServer(const HttpServer& other) = delete;
	HttpServer(HttpServer&& other) = delete;
	HttpServer& operator=(const HttpServer& other) = delete;

   void setup(const char* wifi_ssid, const char* wifi_passwd);
   void init_led_resource();
   int init_wifi(const char* wifi_ssid, const char* wifi_passwd);
   void loop(void);

private:
   static const uint16_t PORT;
};

} // namespace Facilities

#endif // __Facilities_HTTP_Server__