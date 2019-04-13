#include <stdio.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 50

const char* wifi_ssid = "";
const char* wifi_passwd = "";

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

void config_rest_server_routing() {
    http_rest_server.on("/", HTTP_GET, []() {
        http_rest_server.send(200, "text/html",
            "Welcome to the ESP8266 REST Web Server");
    });
//    http_rest_server.on("/leds", HTTP_GET, get_leds);
//    http_rest_server.on("/leds", HTTP_POST, post_put_leds);
//    http_rest_server.on("/leds", HTTP_PUT, post_put_leds);
}

void setup() {
  // put your setup code here, to run once:
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
}

void loop() {
  // put your main code here, to run repeatedly:
  http_rest_server.handleClient();
}
