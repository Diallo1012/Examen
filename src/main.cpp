//inclusions de bibliothèques
#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <WebServer.h>
#include <SPI.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

/*#define BME_SCK  (13)
#define BME_MISO (12)
#define BME_MOSI (11)
#define BME_CS   (10)*/

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // la communication I2C

const char *ssid = "Diallo";  // Le nom de notre WIFI
const char *password = "12345ib."; // Le mot de passe du WIFI

//configuration de notre serveur Web sur le port 80.
AsyncWebServer server(80);

void handle_NotFound();

const char Home_html[] PROGMEM = R"rawliteral(