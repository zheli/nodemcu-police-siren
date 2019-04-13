#include <stdio.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 50

const char* wifi_ssid = "YOUR_WIFI_SSD_HERE";
const char* wifi_passwd = "YOUR_WIFI_PASSWORD_HERE";

int alerting = 0; // 0 = off, 1 = on

uint8_t pin_onboard_led = 16; //  LED PIN
uint8_t pin_external_power_switch = 5;

ESP8266WebServer http_rest_server(HTTP_REST_PORT);

int init_wifi() {
  int retries = 0;

  Serial.println("Connecting to WiFi AP..........");

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_passwd);
  // check the status of WiFi connection to be WL_CONNECTED
  while ((WiFi.status() != WL_CONNECTED) && (retries < MAX_WIFI_INIT_RETRY)) {
      retries++;
      delay(WIFI_RETRY_DELAY);
      Serial.print("#");
  }
  return WiFi.status(); // return the WiFi connection status
}

void toggleLED() {
  String post_body = http_rest_server.arg("plain");
  Serial.println("Got POST body:");
  Serial.println(post_body);
  
  alerting = !alerting;
  Serial.println("LED new status");
  Serial.println(alerting);
  http_rest_server.send(200, "text/html", "LED new status:  " + String(alerting));
}

void config_rest_server_routing() {
  http_rest_server.on("/", HTTP_GET, []() {
    http_rest_server.send(200, "text/html",
        "Welcome to the ESP8266 REST Web Server");
  });
  http_rest_server.on("/leds", HTTP_POST, toggleLED);
}

void setup() {

  // put your setup code here, to run once:
  
  pinMode(pin_onboard_led, OUTPUT);
  pinMode(pin_external_power_switch, OUTPUT);

  Serial.begin(115200);

  if (init_wifi() == WL_CONNECTED) {
    Serial.print("Connetted to ");
    Serial.print(wifi_ssid);
    Serial.print("--- IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.print("Error connecting to: ");
    Serial.println(wifi_ssid);
  }

  config_rest_server_routing();
  http_rest_server.begin();
  Serial.println("HTTP REST Server Started");

  digitalWrite(pin_onboard_led, HIGH);
  analogWrite(pin_external_power_switch, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  http_rest_server.handleClient();

  if (alerting == 1) {
    digitalWrite(pin_onboard_led, LOW);
    analogWrite(pin_external_power_switch, 1023);
  } else {
    digitalWrite(pin_onboard_led, HIGH);
    analogWrite(pin_external_power_switch, 0);
  }
  delay(300);
}
