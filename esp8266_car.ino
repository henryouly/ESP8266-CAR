#include <ESP8266WiFi.h>

int FORWARD = 0;
int BACKWARD = 1;
int LEFT = 2;
int RIGHT = 3;
int STOP = 4;
int PIN_DEF[][4] = {
  {HIGH, LOW, LOW, HIGH},
  {LOW, HIGH, HIGH, LOW},
  {HIGH, LOW, HIGH, LOW},
  {LOW, HIGH, LOW, HIGH},
  {HIGH, HIGH, HIGH, HIGH},
};
int* PIN = new int[4] { D3, D4, D5, D6 };

WiFiServer server(80);

void setup() {
  initHardware();
  setupWiFi();
  server.begin();
}

void initHardware() {
  Serial.begin(115200);
  for (int i = 0; i < 4; i++) 
    pinMode(PIN[i], OUTPUT);
}

void setupWiFi() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("NodeMCU_Car", "nodemcucar");
  IPAddress ip = WiFi.softAPIP();
  Serial.print("Server IP is: ");
  Serial.println(ip);
}

void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  String req = client.readStringUntil('\r');
  Serial.print("Request: ");
  Serial.println(req);
  client.flush();

  processRequest(req);
}

void processRequest(String req) {
  if (req.indexOf("forward") != -1) {
    process(FORWARD);
  } else if (req.indexOf("backward") != -1) {
    process(BACKWARD);
  } else if (req.indexOf("left") != -1) {
    process(LEFT);
  } else if (req.indexOf("right") != -1) {
    process(RIGHT);
  } else if (req.indexOf("stop") != -1) {
    process(STOP);
  }
}

void process(int cmd) {
  for (int i = 0; i < 4; i++)
    digitalWrite(PIN[i], PIN_DEF[cmd][i]);
}

