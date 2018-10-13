//************************************************************
// this is a simple example that uses the painlessMesh library to
// connect to a another network and broadcast message from a webpage to the edges of the mesh network.
// This sketch can be extended further using all the abilities of the AsyncWebserver library (WS, events, ...)
// for more details
// https://gitlab.com/painlessMesh/painlessMesh/wikis/bridge-between-mesh-and-another-network
// for more details about my version
// https://gitlab.com/Assassynv__V/painlessMesh
// and for more details about the AsyncWebserver library
// https://github.com/me-no-dev/ESPAsyncWebServer
//************************************************************

#include "IPAddress.h"
#include "painlessMesh.h"

#ifdef ESP8266
#include "Hash.h"
#include <ESPAsyncTCP.h>
#else
#include <AsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#include "Debug.hpp"
#include "Tasks_Special.hpp"

#define   MESH_PREFIX     "yoloteam"
#define   MESH_PASSWORD   "longpassword"
#define   MESH_PORT       5555

#define   STATION_SSID     "ASML Digital Gold Hackathon"
#define   STATION_PASSWORD "DigitalGold"

#define HOSTNAME "HTTP_BRIDGE"

// Prototype
IPAddress getlocalIP();

painlessMesh  mesh;
AsyncWebServer async_web_server(80);
IPAddress myIP(0,0,0,0);
IPAddress myAPIP(0,0,0,0);

Scheduler taskScheduler;
Tasks::Special specialTask(mesh);

void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  // Channel set to 6. Make sure to use the same channel for your mesh and for you other
  // network (STATION_SSID)
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6 );

  // mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  // mesh.setHostname(HOSTNAME);
  myAPIP = IPAddress(mesh.getAPIP());
  Serial.println("My AP IP is " + myAPIP.toString());
  Serial.println("My IP is " + getlocalIP().toString());


  //Async webserver
  async_web_server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<form>Text to Broadcast<br><input type='text' name='BROADCAST'><br><br><input type='submit' value='Submit'></form>");
    if (request->hasArg("BROADCAST")){
      String msg = request->arg("BROADCAST");
      Serial.println("broadcasting message: " + msg);
      mesh.sendBroadcast(msg);
      specialTask.receivedCb(mesh.getNodeId(), msg);
    }
  });
  async_web_server.begin();
  Serial.println("Server started");

  taskScheduler.addTask(specialTask);
  specialTask.enable();

  Serial.println("Setup completed for node");
  Serial.println(mesh.getNodeId());
}

void loop() {
  mesh.update();
  if(myIP != getlocalIP()){
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
  }
  taskScheduler.execute();
}

IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}
