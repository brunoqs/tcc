#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "Ap 201"
#define STAPSK "oreiaseca"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);

void handleRoot()
{
  digitalWrite(LED_BUILTIN, LOW);
	server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(LED_BUILTIN, HIGH);
}

void handleNotFound()
{
  digitalWrite(LED_BUILTIN, LOW);
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";
	for (uint8_t i = 0; i < server.args(); i++)
	{
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	}
	server.send(404, "text/plain", message);
  digitalWrite(LED_BUILTIN, HIGH);
}

void handleData()
{
	if (server.hasArg("data")){
    digitalWrite(LED_BUILTIN, LOW);
		String data = server.arg("data");
		server.send(200, "text/plain", data);
	} else {
		server.send(400, "text/plain", "dado incorreto");
	}
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(115200);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	Serial.println("");

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	if (MDNS.begin("esp8266"))
	{
		Serial.println("MDNS responder started");
	}

	server.on("/", handleRoot);

	server.on("/inline", []() {
		server.send(200, "text/plain", "this works as well");
	});

	server.on("/sendData", handleData);

	server.onNotFound(handleNotFound);

	server.begin();
	Serial.println("HTTP server started");
}

void loop()
{
	server.handleClient();
	MDNS.update();
}
