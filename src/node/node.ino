#include "painlessMesh.h"
#include "Hash.h"
#include <ESPAsyncTCP.h>

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

// Prototype
void receivedCallback( const uint32_t &from, const String &msg );

painlessMesh  mesh;
const int ledPin = 1;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  // Channel set to 6. Make sure to use the same channel for your mesh and for you other
  // network (STATION_SSID)
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6 );
  mesh.onReceive(&receivedCallback);
}

void loop() {
  mesh.update();
}

void receivedCallback( const uint32_t &from, const String &msg ) {
  digitalWrite(ledPin, LOW);
  mesh.sendSingle(from, "recebi o pacote");
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
}
