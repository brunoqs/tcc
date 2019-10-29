#include "painlessMesh.h"
extern "C" {
#include "user_interface.h"
}

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

// Prototype
void receivedCallback( const uint32_t &from, const String &msg );

painlessMesh  mesh;
const int ledPin = 1;
int pinState;

void setup() {
  Serial.begin(115200);
  
  pinMode(ledPin, OUTPUT);
  pinState = HIGH;
  digitalWrite(ledPin, pinState);
  
  system_phy_set_max_tpw(1);
  
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  // Channel set to 6. Make sure to use the same channel for your mesh and for you other
  // network (STATION_SSID)
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_STA, 6);

  // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
//  mesh.setContainsRoot(true);
  
  mesh.onReceive(&receivedCallback);
}

void loop() {
  mesh.update();
}

void receivedCallback( const uint32_t &from, const String &msg ) {
  if (pinState == HIGH) {
    digitalWrite(ledPin, LOW);
    pinState = LOW;
  } else {
    digitalWrite(ledPin, HIGH);
    pinState = HIGH;
  }
  mesh.sendSingle(from, "recebi o pacote");
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
}
