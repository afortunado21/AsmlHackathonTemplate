//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Mesh Network wrapper class to provide a container to add specific
//! mesh network behaviour.

#include "Facilities_MeshNetwork.hpp"

#include "Debug.hpp"
#include "painlessMesh.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include <stdio.h>

AsyncWebServer server(80);

namespace Facilities {

const uint16_t MeshNetwork::PORT = 5555;

//! Construct only.
//! \note Does not construct and initialize in one go to be able to initialize after serial debug port has been opened.
MeshNetwork::MeshNetwork()
{
    curIp = "0.0.0.0";
   m_mesh.onReceive(std::bind(&MeshNetwork::receivedCb, this, std::placeholders::_1, std::placeholders::_2));
   //m_mesh.onNewConnection(...);
  m_mesh.onChangedConnections(std::bind(&MeshNetwork::changedCb,this));
   //m_mesh.onNodeTimeAdjusted(...);
}

void MeshNetwork::parseShapeStr(String& str, byte ar[], int width)
{
    const char* s=str.c_str();
    int i=0;
    while(*s++) {
        ar[i]=s[i]-'0';
        i++;
    }
}

// Initialize mesh network.
void MeshNetwork::initialize(const __FlashStringHelper *prefix, const __FlashStringHelper *password, Scheduler& taskScheduler)
{
   // Set debug messages before init() so that you can see startup messages.
   m_mesh.setDebugMsgTypes( ERROR | STARTUP );  // To enable all: ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE

    m_mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages
    m_mesh.init( prefix, password, PORT, WIFI_AP_STA, 6 );
    m_mesh.stationManual(STATION_SSID, STATION_PASSWORD);
    m_mesh.setHostname(HOSTNAME);
    IPAddress myAPIP(0,0,0,0);
    myAPIP = IPAddress(m_mesh.getAPIP());

    server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
        request->send(200, "text/html", "<form>Text to Broadcast<br><input type='text' name='BROADCAST'><br><br><input type='submit' value='Submit'></form>");
        if (request->hasArg("BROADCAST")){
            String msg = request->arg("BROADCAST");
            MY_DEBUG_PRINTLN("arg: " + msg);
            // m_mesh.sendBroadcast("REQST " + msg);
        }
    });
    server.on("/ACTIVE_NODE_COUNT", HTTP_GET, [&](AsyncWebServerRequest *request){
	    unsigned int activeNodeCount=m_mesh.getNodeList().size();
        char activeNodeCountStr[100];
        sprintf(activeNodeCountStr, "%d", activeNodeCount);
        request->send(200, "text/plain", activeNodeCountStr);
	    MY_DEBUG_PRINTLN("Request: ACTIVE_NODE_COUNT");
    });
    server.on("/SHAPES", HTTP_GET, [&](AsyncWebServerRequest *request){
        uint8_t imgA1[8*32];
        uint8_t imgA2[16*32];
        uint8_t imgA3[24*32];
        uint8_t imgA4[32*32];

        uint8_t imgB1[8*32];
        uint8_t imgB2[16*32];
        uint8_t imgB3[24*32];
        uint8_t imgB4[32*32];

	    String strA1 = request->arg("A1");
        parseShapeStr(strA1, imgA1, 8*1);

	    String strA2 = request->arg("A2");
        parseShapeStr(strA2, imgA2, 8*2);

	    String strA3 = request->arg("A3");
        parseShapeStr(strA3, imgA3, 8*3);

	    String strA4 = request->arg("A4");
        parseShapeStr(strA4, imgA4, 8*4);

	    String strB1 = request->arg("B1");
        parseShapeStr(strB1, imgB1, 8*1);

	    String strB2 = request->arg("B2");
        parseShapeStr(strB2, imgB2, 8*2);

	    String strB3 = request->arg("B3");
        parseShapeStr(strB3, imgB3, 8*3);

	    String strB4 = request->arg("B4");
        parseShapeStr(strB4, imgB4, 8*4);

        request->send(200, "text/plain", "Received");
	    MY_DEBUG_PRINTLN("Request: SHAPES");
    });
    server.begin();
}

//! Update mesh; forward call to painlessMesh.
void MeshNetwork::update()
{
    m_mesh.update();
}

void MeshNetwork::sendBroadcast(String &message)
{
//   MY_DEBUG_PRINT("Broadcasting message: "); MY_DEBUG_PRINTLN(message);
   m_mesh.sendBroadcast(message, false); // false: Do not include self.
}

MeshNetwork::NodeId MeshNetwork::getMyNodeId()
{
   return m_mesh.getNodeId();
}

std::list<MeshNetwork::NodeId> MeshNetwork::getNodeList() {
    return m_mesh.getNodeList();
}

std::vector<MeshNetwork::NodeId> MeshNetwork::getSortedNodeVector() {
    std::list<NodeId> node_list = getNodeList();
    node_list.sort();
    return std::vector<NodeId>(node_list.begin(), node_list.end());
}

void MeshNetwork::onReceive(receivedCallback_t receivedCallback)
{
   m_mesh.onReceive(receivedCallback);
}

void MeshNetwork::receivedCb(NodeId transmitterNodeId, String& msg)
{
   MY_DEBUG_PRINTF("Data received from node: %u; msg: %s\n", transmitterNodeId, msg.c_str());
}


void MeshNetwork::changedCb()
{
    MY_DEBUG_PRINTF("Connections Changed");
   MeshNetwork::nodes_present = MeshNetwork::getSortedNodeVector();
   
}

} // namespace Facilities
