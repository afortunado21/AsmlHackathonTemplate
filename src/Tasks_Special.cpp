//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Example Task to output something to the LED Matrix.
#include "Tasks_Special.hpp"

#include "Debug.hpp"
#include "painlessMesh.h"
#include "Util.hpp"

#include <LEDMatrixDriver.hpp>

#include <functional>
// #include <sstream>

namespace Tasks
{

// const int Special::MASTER_ID = 978880587;
const int Special::LEDMATRIX_WIDTH = 31;
const int Special::LEDMATRIX_HEIGHT = 7;
const int Special::LEDMATRIX_SEGMENTS = 4;
const int Special::LEDMATRIX_INTENSITY = 5;
const int Special::LEDMATRIX_CS_PIN = 16;
const unsigned long Special::POLL_DELAY_MS = 800;

//! Initializes the LED Matrix display.
Special::Special(painlessMesh &mesh) : Task(POLL_DELAY_MS, TASK_FOREVER, std::bind(&Special::execute, this)),
                                                                         m_mesh(mesh),
                                                                         m_lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN),
                                                                         m_x(0)
{
   m_lmd.setEnabled(true);
   m_lmd.setIntensity(LEDMATRIX_INTENSITY);

   m_mesh.onReceive(std::bind(&Special::receivedCb, this, std::placeholders::_1, std::placeholders::_2));
   m_mesh.onChangedConnections(std::bind(&Special::changedCb, this));

   // Draw image.
   for (int i = 0; i < 32; i++)
   {
      for (int j = 0; j < 32; j++)
      {
         int x = 2 * i - 31;
         int y = 2 * j - 31;
         img[i][j] = (x * x + y * y <= 31 * 31 + 1);
      }
   }
}

//! Update display
void Special::execute()
{
    /*
   changedCb();
   if (tick % 5 == 0) {
      if (tick / 5 % 3 == 0) {
         for (int i = 0; i < 32; i++)
         {
            for (int j = 0; j < 32; j++)
            {
               int x = 2 * i - 31;
               int y = 2 * j - 31;
               img[i][j] = (x * x + y * y <= 31 * 31 + 1);
            }
         }
      } else if (tick / 5 % 3 == 1) {
         for (int i = 0; i < 32; i++)
         {
            for (int j = 0; j < 32; j++)
            {
               int x = 2 * i - 31;
               int y = 2 * j - 31;
               img[i][j] = (x * x + y * y <= 9 * 9 + 1);
            }
         }
      } else {
         for (int i = 0; i < 32; i++)
         {
            for (int j = 0; j < 32; j++)
            {
               int x = 2 * i - 31;
               int y = 2 * j - 31;
               img[i][j] = (x * x + y * y <= 32 * 32 + 1 && x * x + y * y > 30 * 30 + 1);
            }
         }
      }
   }
   */

 

  MY_DEBUG_PRINTLN("CHECKING NODE IDS");

   for (uint32_t node : node_ids)
   {
      MY_DEBUG_PRINTLN(node);
   }
   uint32_t my_id = m_mesh.getNodeId();

   MY_DEBUG_PRINTLN("CHECKED NODE IDS");
   MY_DEBUG_PRINTLN("I AM:");
   MY_DEBUG_PRINTLN(my_id);
   if(node_ids.size() < 3) {
     MY_DEBUG_PRINTLN("SCALING IS NEEDED");
     int num_nodes = (node_ids.size() + 1);

    auto temp_img = img;

     float sf = 4 / num_nodes;

     for(int i = 0; i < 32; ++i) {
       for(int j = 0; j < 32; ++j) {
         int interp_from_x = floor(sf * i);
         int interp_from_y = floor(sf * j);
         if(interp_from_x > 31 || interp_from_y > 31) {
           img[i][j] = 0;
         } else {
           img[i][j] = temp_img[interp_from_x][interp_from_y];
         }
       }
     }
   }

   if (node_ids.size() > 0 && node_ids[0] < my_id)
   {
      return;
   }

   for (int i = 0; i < (int) node_ids.size(); i++)
   {
      String msg;
      msg += "PRINT ";
      msg += node_ids[i];
      msg += ' ';
      for (int j = 0; j < 32; j++)
      {
         for (int k = 0; k < 8; k++)
         {
            msg += img[j][8 * (i+1) + k];
            msg += ' ';
         }
      }
      m_mesh.sendBroadcast(msg);
      MY_DEBUG_PRINTLN("Sent broadcast!");
   }
   
   // Draw own image.
   m_lmd.clear();
   for (int i = 0; i <= LEDMATRIX_WIDTH; i++)
   {
      for (int j = 0; j <= LEDMATRIX_HEIGHT; j++)
      {
         std::pair<int, int> mapped_coordinates = map_single(i, j);
         m_lmd.setPixel(mapped_coordinates.first,
                        mapped_coordinates.second, img[i][j]);
      }
   }
   m_lmd.display();
   
   ++tick;
}

void Special::receivedCb(uint32_t nodeId, String &msg)
{
   // MY_DEBUG_PRINTLN("Received data in Special:");
   // MY_DEBUG_PRINTLN(msg.c_str());

   if (msg.indexOf("PRINT ") == 0)
   {
      unsigned int index = 6; 
      // msg = msg.substring(6);
      uint32_t id = read_uint32_t(msg, &index);
      if (id == m_mesh.getNodeId())
      {
         m_lmd.clear();
         m_lmd.display();
         m_lmd.clear();
         for (int i = 0; i <= LEDMATRIX_WIDTH; i++)
         {
            for (int j = 0; j <= LEDMATRIX_HEIGHT; j++)
            {
               int value = read_uint32_t(msg, &index);
               std::pair<int, int> mapped_coordinates = map_single(i, j);
               m_lmd.setPixel(mapped_coordinates.first,
                              mapped_coordinates.second, value);
            }
         }
         m_lmd.display();
      }
   } else if (msg.indexOf("UPDATE ") == 0) {
      unsigned int index = 7;
      // msg = msg.substring(6);
      for (int i = 0; i <= LEDMATRIX_WIDTH; i++)
      {
          for (int j = 0; j <= LEDMATRIX_WIDTH; j++)
          {
              img[i][j] = read_uint32_t(msg, &index);
          }
      }
   }
}

void Special::changedCb() {
    auto node_ids_list = m_mesh.getNodeList();
    node_ids_list.sort();
    node_ids = vector<uint32_t>(node_ids_list.begin(), node_ids_list.end());
    // MY_DEBUG_PRINT("node_ids.size(): ");
    // MY_DEBUG_PRINTLN(node_ids.size());
}

} // namespace Tasks
