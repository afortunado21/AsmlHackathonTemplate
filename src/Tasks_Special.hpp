//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Example Task

#ifndef __Tasks_Special__
#define __Tasks_Special__

#include "Facilities_MeshNetwork.hpp"

#include <painlessMesh.h>
#include <LEDMatrixDriver.hpp>

#include <functional>

// Forward declarations.
namespace Facilities { class MeshNetwork; }


namespace Tasks {

class Special : public Task
{
public:
   explicit Special(Facilities::MeshNetwork& mesh);
   ~Special() {};

   // Disallow copy-ing
	Special(const Special& other) = delete;
	Special(Special&& other) = delete;
	Special& operator=(const Special& other) = delete;

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
   vector<Facilities::MeshNetwork::NodeId> node_ids;
   int tick = 0;

   void receivedCb(Facilities::MeshNetwork::NodeId nodeId, String& msg);


};

} // namespace Tasks

#endif //  __Tasks_Special__
