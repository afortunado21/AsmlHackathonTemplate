//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Example Task to output something to the LED Matrix.
#include "Tasks_ExampleMessageTask.hpp"

#include "Debug.hpp"
#include "Facilities_MeshNetwork.hpp"
#include "Util.h"

#include <LEDMatrixDriver.hpp>

#include <functional>
// #include <sstream>

namespace Tasks
{

// const int ExampleMessageTask::MASTER_ID = 978880587;
const int ExampleMessageTask::LEDMATRIX_WIDTH = 31;
const int ExampleMessageTask::LEDMATRIX_HEIGHT = 7;
const int ExampleMessageTask::LEDMATRIX_SEGMENTS = 4;
const int ExampleMessageTask::LEDMATRIX_INTENSITY = 5;
const int ExampleMessageTask::LEDMATRIX_CS_PIN = 16;
const unsigned long ExampleMessageTask::POLL_DELAY_MS = 100;

//! Initializes the LED Matrix display.
ExampleMessageTask::ExampleMessageTask(Facilities::MeshNetwork &mesh) : Task(POLL_DELAY_MS, TASK_FOREVER, std::bind(&ExampleMessageTask::execute, this)),
                                                                        m_mesh(mesh),
                                                                        m_lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN),
                                                                        m_x(0)
{
   m_lmd.setEnabled(true);
   m_lmd.setIntensity(LEDMATRIX_INTENSITY);

   m_mesh.onReceive(std::bind(&ExampleMessageTask::receivedCb, this, std::placeholders::_1, std::placeholders::_2));

   
  // set up test image
  for(int i = 0; i < 32; ++i ) {
    for(int j = 0; j < 32; ++j) {
     image[i][j] = (i%3 ==0)?0:1;
    }
  }

/*
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
   */
}

//! Update 
void ExampleMessageTask::draw_img(int img[32][32], uint8_t intensity) {
  m_lmd.setIntensity(intensity);
  MY_DEBUG_PRINTLN("CHECKING NODE IDS");
   node_ids = m_mesh.nodes_present;

   for (uint32_t node : node_ids)
   {
      MY_DEBUG_PRINTLN(node);
   }
   uint32_t my_id = m_mesh.getMyNodeId();

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
      MY_DEBUG_PRINTLN("NOT MASTER");
      return;
   }

   MY_DEBUG_PRINTLN("WE ARE MASTER");
   for (int i = 0; i < node_ids.size(); i++)
   {
      String msg;
      msg += "PRINT ";
      msg += ' ';
      msg += intensity; 
      msg += ' ';
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
   }
   MY_DEBUG_PRINTLN("PRINTED MASTER STUFF");
   
   // Draw own image.
   m_lmd.clear();
   MY_DEBUG_PRINTLN("PRINTING OWN IMAGE");
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

}



void ExampleMessageTask::execute()
{
 
  MY_DEBUG_PRINTLN("PRINTING TEST IMG");

  for(int i = 0; i < 32; ++i ) {
        for (int j = 0; j < 32; ++j) {
            if( i%4 == 0 || i%4 == 1) {
                bars[i][j] = 0;
            } else {
                bars[i][j] = 1;
            }
        }
    }

      for(int i = 0; i < 32; ++i ) {
        for (int j = 0; j < 32; ++j) {
            if( (j - 16) > 10 || (i - 10) > 10 ){
                rectangle[i][j] = 1;
            } else {
                rectangle[i][j] = 0;
            }
        }
    }

      for(int i = 0; i < 32; ++i ) {
        for (int j = 0; j < 32; ++j) {
            if( i*i + j*j <200) {
                circle[i][j] = 1;
            } else {
                circle[i][j] = 0;
            }
        }
    }

   draw_img(image,5);
   
}

void ExampleMessageTask::receivedCb(Facilities::MeshNetwork::NodeId nodeId, String &msg)
{
//   MY_DEBUG_PRINTLN("Received data to print");
  // MY_DEBUG_PRINTLN(msg.c_str());

   if (msg.indexOf("PRINT ") == 0)
   {
      msg = msg.substring(6);
      uint32_t id = read_uint32_t(&msg);
      if (id == m_mesh.getMyNodeId())
      {
        
         m_lmd.clear();
         int intensity = read_uint8_t(&msg);
         m_lmd.setIntensity(intensity);
         for (int i = 0; i <= LEDMATRIX_WIDTH; i++)
         {
            for (int j = 0; j <= LEDMATRIX_HEIGHT; j++)
            {
               int value = read_uint32_t(&msg);
               std::pair<int, int> mapped_coordinates = map_single(i, j);
               m_lmd.setPixel(mapped_coordinates.first,
                              mapped_coordinates.second, value);
            
            }
         }
         m_lmd.display();
      }
   }

   if (++m_x > LEDMATRIX_WIDTH)
   {
      m_x = 0;
   }
}

} // namespace Tasks
