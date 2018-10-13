#include <Arduino.h>

#include "Debug.hpp"
#include "painlessMesh.h"
#include "Facilities_MeshNetwork.hpp"
#include "Tasks_ExampleTransmitTask.hpp"
#include "Tasks_ExampleDisplayTask.hpp"
#include "Tasks_ExampleMessageTask.hpp"

// Translation unit local variables
namespace {

Scheduler                  taskScheduler;

Facilities::MeshNetwork    meshNetwork;
Tasks::ExampleTransmitTask exampleTransmitTask(meshNetwork);
Tasks::ExampleDisplayTask  exampleDisplayTask(meshNetwork);
Tasks::ExampleMessageTask  exampleMessageTask(meshNetwork);
}

//! Called once at board startup.
void setup()
{
   MY_DEBUG_BEGIN(115200);

   // Create MeshNetwork
   meshNetwork.initialize(F("Yellow-Team"), F("DigitalGold"), taskScheduler,wifi_set_channel = 4);

   // Create and add tasks.
   taskScheduler.addTask( exampleMessageTask );
   exampleMessageTask.enable();

   MY_DEBUG_PRINTLN(F("Setup completed"));
   MY_DEBUG_PRINTLN(meshNetwork.getMyNodeId());
}

//! Called repeatedly after setup().
void loop()
{
   taskScheduler.execute();
   meshNetwork.update();
}
