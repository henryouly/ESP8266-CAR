#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


int FORWARD = 0;
int BACKWARD = 1;
int LEFT = 2;
int RIGHT = 3;
int STOP = 4;
int PIN_DEF[][4] = {
  {HIGH, LOW, LOW, HIGH},
  {LOW, HIGH, HIGH, LOW},
  {HIGH, LOW, HIGH, HIGH},
  {HIGH, HIGH, LOW, HIGH},
  {HIGH, HIGH, HIGH, HIGH},
};
int* PIN = new int[4] { D3, D4, D5, D6 };
int* PWM_PIN = new int[2] { D0, D2 };
String WEBPAGE = 
  "<h1>ESP8266 CAR</h1>"
  "<a href=\"/forward\"><button>FORWARD</button></a>"
  "<a href=\"/backward\"><button>BACKWARD</button></a>"
  "<a href=\"/left\"><button>LEFT</button></a>"
  "<a href=\"/right\"><button>RIGHT</button></a>"
  "<a href=\"/stop\"><button>STOP</button></a>";

ESP8266WebServer server(80);

void setup() {
  initHardware();
  setupWiFi();
  setupWebServer();
}

void setupWebServer() {
  server.on("/", []() {
    server.send(200, "text/html", WEBPAGE);
  });
  server.on("/forward", []() { process(FORWARD); });
  server.on("/backward", []() { process(BACKWARD); });
  server.on("/left", []() { process(LEFT); });
  server.on("/right", []() { process(RIGHT); });
  server.on("/stop", []() { process(STOP); });
  server.on("/pwm1", []() {
    int pwm=server.arg("pwm").toInt();
    analogWrite(PWM_PIN[0], pwm);
    server.send(200, "text/html", WEBPAGE);    
  });
  server.on("pwm2", []() {
    int pwm=server.arg("pwm").toInt();
    analogWrite(PWM_PIN[1], pwm);
    server.send(200, "text/html", WEBPAGE);    
  });
  server.begin();
  Serial.println("HTTP server started");
}

void initHardware() {
  Serial.begin(115200);
  for (int i = 0; i < 4; i++) 
    pinMode(PIN[i], OUTPUT);
  for (int i = 0; i < 2; i++)
    pinMode(PWM_PIN[i], OUTPUT);
  analogWrite(PWM_PIN[0], 1023);
  analogWrite(PWM_PIN[1], 1023);
}

void setupWiFi() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("NodeMCU_Car", "nodemcucar");
  IPAddress ip = WiFi.softAPIP();
  Serial.print("Server IP is: ");
  Serial.println(ip);
}

void loop() {
  server.handleClient();
}

void process(int cmd) {
  for (int i = 0; i < 4; i++)
    digitalWrite(PIN[i], PIN_DEF[cmd][i]);
  server.send(200, "text/html", WEBPAGE);
}

