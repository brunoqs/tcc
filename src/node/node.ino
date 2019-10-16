#include "painlessMesh.h"

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
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  // Channel set to 6. Make sure to use the same channel for your mesh and for you other
  // network (STATION_SSID)
  // void init(String ssid, String password, uint16_t port = 5555, enum nodeMode connectMode = STA_AP, _auth_mode authmode = AUTH_WPA2_PSK, uint8_t channel = 1, phy_mode_t phymode = PHY_MODE_11G, uint8_t maxtpw = 82, uint8_t hidden = 0, uint8_t maxconn = 4);
  // uint8_t maxtpw = 82 determina altera o dBm
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6, PHY_MODE_11G, 10);
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
