#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

/*-------------CONFIG--------------------*/
const char*  WIFI_SSID      = "TeleCentro-4464";
const char*  WIFI_Password  = "cocodrilo2024";
int relayPin[4] = {0,1,2,3};
String relayState[4] = {"ON", "ON", "ON", "ON"};  // Estado inicial de los 4 relés
/*-------------CONFIG--------------------*/
ESP8266WebServer server(80);

void setup(void) {
  for (int i = 0; i < 4; i++) {
    pinMode(relayPin[i], OUTPUT);
    digitalWrite(relayPin[i], HIGH);  // Inicialmente prendidos
  }
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_Password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/state", handleState);  // Endpoint para consultar el estado
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}

void handleRoot() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "El Modulo ESP8266-01s esta conectado");
  } else {
    cors();
    int relayNumber = server.arg("relay").toInt();
    String command = server.arg("command");

    if (command == "ON") {
      relayState[relayNumber - 1] = "ON";
      digitalWrite(relayPin[relayNumber - 1], HIGH);  // Asumiendo un array de pines
      server.send(200, "text/plain", "ON");
      
    } else if (command == "OFF") {
      relayState[relayNumber - 1] = "OFF";
      digitalWrite(relayPin[relayNumber - 1], LOW);
      server.send(200, "text/plain", "OFF");
      
    } else {
      server.send(200, "text/plain", "UNKNOWN_COMMAND");
    }
    delay(100);
      Serial.println(relayNumber);
      Serial.println(command);
  }
}

void handleState() {
  cors();
  String jsonResponse = "{";
  for (int i = 0; i < 4; i++) {
    jsonResponse += "\"relay" + String(i+1) + "\":\"" + relayState[i] + "\"";
    if (i < 3) jsonResponse += ",";
  }
  jsonResponse += "}";
  
  server.send(200, "application/json", jsonResponse);
}
void cors(){
  server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}
