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
const unsigned long ExampleMessageTask::POLL_DELAY_MS = 2000;

//! Initializes the LED Matrix display.
ExampleMessageTask::ExampleMessageTask(Facilities::MeshNetwork &mesh) : Task(POLL_DELAY_MS, TASK_FOREVER, std::bind(&ExampleMessageTask::execute, this)),
                                                                        m_mesh(mesh),
                                                                        m_lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN),
                                                                        m_x(0)
{
   m_lmd.setEnabled(true);
   m_lmd.setIntensity(LEDMATRIX_INTENSITY);

   m_mesh.onReceive(std::bind(&ExampleMessageTask::receivedCb, this, std::placeholders::_1, std::placeholders::_2));

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
void ExampleMessageTask::execute()
{
   if (m_mesh.getMyNodeId() == 978880587)
   {
      vector<uint32_t> slave_ids = {978879811, 978879937, 3256120749};
      for (int i = 0; i < 3; i++)
      {
         String msg;
         msg += "PRINT ";
         msg += slave_ids[i];
         msg += ' ';
         for (int j = 0; j < 32; j++)
         {
            for (int k = 0; k < 8; k++)
            {
               msg += img[j][8 * i + k];
               msg += ' ';
            }
         }
         m_mesh.sendBroadcast(msg);
      }
      // Draw own image.
      m_lmd.clear();
      for (int i = 0; i <= LEDMATRIX_WIDTH; i++)
      {
         for (int j = 0; j <= LEDMATRIX_HEIGHT; j++)
         {
            std::pair<int, int> mapped_coordinates = map_single(i, j);
            m_lmd.setPixel(mapped_coordinates.first,
                           mapped_coordinates.second, img[i][3 * 8 + j]);
         }
      }
      m_lmd.display();
   }
}

void ExampleMessageTask::receivedCb(Facilities::MeshNetwork::NodeId nodeId, String &msg)
{
   MY_DEBUG_PRINTLN("Received data in ExampleMessageTask:");
   MY_DEBUG_PRINTLN(msg.c_str());

   if (msg.indexOf("PRINT ") == 0)
   {
      msg = msg.substring(6);
      uint32_t id = read_uint32_t(&msg);
      if (id == m_mesh.getMyNodeId())
      {
         m_lmd.clear();
         m_lmd.display();
         m_lmd.clear();
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
