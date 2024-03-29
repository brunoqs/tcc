#include "IPAddress.h"
#include "painlessMesh.h"
#include "Hash.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
extern "C" {
#include "user_interface.h"
}

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

#define   STATION_SSID     "Ap 201"
#define   STATION_PASSWORD "oreiaseca"

#define HOSTNAME "HTTP_BRIDGE"

SimpleList<uint32_t> nodes;

// Prototype
void receivedCallback( const uint32_t &from, const String &msg );
IPAddress getlocalIP();

painlessMesh  mesh;
AsyncWebServer server(80);
IPAddress myIP(0,0,0,0);
IPAddress myAPIP(0,0,0,0);

const int ledPin = 1;
int pinState;

void setup() {
  Serial.begin(115200);
  
  pinMode(ledPin, OUTPUT);
  pinState = HIGH;
  digitalWrite(ledPin, pinState);
  
  system_phy_set_max_tpw(1); // altera o potencia do wifi
  
  mesh.setDebugMsgTypes( ERROR | DEBUG | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  // Channel set to 6. Make sure to use the same channel for your mesh and for you other
  // network (STATION_SSID)
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6);
  mesh.onReceive(&receivedCallback);

  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  mesh.setHostname(HOSTNAME);

  // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
  mesh.setRoot(true);
  // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
  mesh.setContainsRoot(true);

  myAPIP = IPAddress(mesh.getAPIP());
  Serial.println("My AP IP is " + myAPIP.toString());

  //Async webserver
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    bool success = true;
    if (request->hasArg("text") && request->hasArg("uuid")) {
      String msg = request->arg("text");
      uint32_t uuid = request->arg("uuid").toInt();
      success = mesh.sendSingle(uuid, msg);
    }
    if (success)
      request->send(200, "text/html", "<form>Text to Send<br><input type='text' name='text'><br>UUID ESP<br><input type='text' name='uuid'><br><br><input type='submit' value='Submit'></form>");
    request->send(400, "text/plain", "Error");
  });

  server.on("/mesh_topology", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", mesh.subConnectionJson());
  });

  server.on("/nodes", HTTP_GET, [](AsyncWebServerRequest *request){
    String jsonNodes = "{'nodes': [";
    nodes = mesh.getNodeList();
    SimpleList<uint32_t>::iterator node = nodes.begin();
    if (node == nodes.end()) {
      jsonNodes += "], 'rootUUID': ";
    } else {
      while (node != nodes.end()) {
        jsonNodes += "'" + String(*node) + "',";
        node++;
      }
      jsonNodes.remove(jsonNodes.length()-1);
      jsonNodes += "], 'rootUUID': ";
    }
    
    jsonNodes += String(mesh.getNodeId()) + "}";

    request->send(200, "application/json", jsonNodes);
  });
  server.begin();

}

void loop() {
  mesh.update();
  if(myIP != getlocalIP()){
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
  }
}

void receivedCallback( const uint32_t &from, const String &msg ) {
  if (pinState == HIGH) {
    digitalWrite(ledPin, LOW);
    pinState = LOW;
  } else {
    digitalWrite(ledPin, HIGH);
    pinState = HIGH;
  }
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
}

IPAddress getlocalIP() {
 return IPAddress(mesh.getStationIP());
}
