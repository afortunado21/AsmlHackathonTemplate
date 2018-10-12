//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Example Task

#ifndef __Tasks_ExampleMessageTask__
#define __Tasks_ExampleMessageTask__

#include "Facilities_MeshNetwork.hpp"

#include <painlessMesh.h>
#include <LEDMatrixDriver.hpp>

#include <functional>

// Forward declarations.
namespace Facilities { class MeshNetwork; }


namespace Tasks {

class ExampleMessageTask : public Task
{
public:
   explicit ExampleMessageTask(Facilities::MeshNetwork& mesh);
   ~ExampleMessageTask() {};

   // Disallow copy-ing
	ExampleMessageTask(const ExampleMessageTask& other) = delete;
	ExampleMessageTask(ExampleMessageTask&& other) = delete;
	ExampleMessageTask& operator=(const ExampleMessageTask& other) = delete;

   void execute();

private:
   static const int LEDMATRIX_WIDTH;
   static const int LEDMATRIX_HEIGHT;
   static const int LEDMATRIX_SEGMENTS;
   static const int LEDMATRIX_INTENSITY;
   static const int LEDMATRIX_CS_PIN;
   static const unsigned long POLL_DELAY_MS;

   Facilities::MeshNetwork& m_mesh;
   LEDMatrixDriver m_lmd;

   int m_x;
   int img[32][32];

   void receivedCb(Facilities::MeshNetwork::NodeId nodeId, String& msg);


};

} // namespace Tasks

#endif //  __Tasks_ExampleMessageTask__
