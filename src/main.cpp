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
  <!DOCTYPE html> 
<html lang="en">

<head>
    <meta content="text/html; charset=UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css" rel="stylesheet"
        integrity="sha384-rbsA2VBKQhggwzxH7pPCaAqO46MgnOM80zW1RWuH61DGLwZJEdK2Kadq2F9CUG65" crossorigin="anonymous">
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.10.3/font/bootstrap-icons.css">
    <style>
      .ib1{
    margin-top:60px;
    background: red;
    height: 100%;
    display: block;  
}
.container {
    background-color:whitesmoke;
    width: 400px;
}
.elem1 {
    background-color: blueviolet;
}
.bordure {
    border: solid white 2px;
    border-radius: 8px;
    margin: 20px;
    background-color: white;
    font-size: 0.8em;
}
.element {
    font-size: 20px;
    background-color: whitesmoke;
    padding: 20px;
    margin: 20px;
    box-shadow: 6px 6px 25px rgba(0, 0, 0, 0.5);
    text-align: center;
}
.ib2{
    color: rgb(6, 133, 88);
}
.ib3{
    color: rgb(45, 98, 173);
}
.ib4{
    color: rgb(72, 169, 103);
}
.ib5{
    color: rgb(149, 26, 26);
}
.ib6{
    padding: 5px;
    border: white;
    top: 10%;
    left: 35vw;
}
    </style>
</head>

<body class="text-center ib1">

    <div class="container">
        <div class="row elem1">
            <h2>BME280 WEB SERVER</h2>
        </div>
        <div class="ib6">
            <div class="row bordure element ib2">
                <div class="row">
                    <h5><span><i class="bi bi-thermometer-half temp-icon"></i> TEMPERATURE </h5></span>
                </div>
                <div><span id="tempVal"></span><sup> &deg;C</sup></div>
            </div>
            <div class="row bordure element ib3">
                <div class="row">
                    <h5><i class="bi bi-cloud-drizzle humid-icon"></i> HUMIDITY </h5>
                </div>
                <div><span id="humidVal"></span><sup> %</sup></div>
            </div>
            <div class="row bordure element ib4">
                <div class="row">
                    <h5><i class="bi bi-clock-history pressure-icon"></i> PRESSURE </h5>
                </div>
                <div><span id="pressVal"></span><sup> hPa</sup></div>
            </div>
            <div class="row bordure element ib5">
                <div class="row">
                    <h5><i class="bi bi-triangle altitude-icon"></i> ALTITUDE </h5>
                </div>
                <div><span id="altVal"></span><sup> M</sup></div>
            </div>
        </div>
    </div>

    <script src="/js/stock.js"></script>
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.6.3/jquery.min.js"></script>
    
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/js/bootstrap.bundle.min.js"
        integrity="sha384-kenU1KFdBIe4zVF0s0G1M5b4hcpxyD9F7jL+jjXkk+Q2h455rYXK/7HAuoJl+0I4"
        crossorigin="anonymous"></script>
</body>
</html>)rawliteral";

const char Js_Objet[] PROGMEM = R"rawliteral(
    setInterval(function () {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                var recup = JSON.parse(this.responseText);
                document.getElementById("tempVal").innerHTML = recup.temperature;
                document.getElementById("humidVal").innerHTML = recup.humidite;
                document.getElementById("pressVal").innerHTML = recup.pressure;
                document.getElementById("altVal").innerHTML = recup.altitude;
            }
        };
        xhttp.open("GET", "/data", true);
        xhttp.send();
    },3000);
)rawliteral";

void setup()
{
    Serial.begin(9600);
    while (!Serial)
        // delay(1000);  wait for native usb
        Serial.println(F("BME280 test"));
    // status = BME.begin(BME280_ADDRESS_ALT, BME280_CHIPID);
    bme.begin(0x76);

    Serial.println("Connecting to ");
    Serial.println(ssid);

    // connect to your local wi-fi network
    WiFi.begin(ssid, password);

     // check wi-fi is connected to wi-fi network
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected..!");
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP());


  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", Home_html); });

 server.on("/js/stock.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "application/javascript", Js_Objet); });

  // Créez une route pour envoyer les données du capteur au format JSON
    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String objet = "{";
    objet += "\"temperature\": " + String(bme.readTemperature()) + ",";
    objet += "\"humidite\": " + String(bme.readHumidity()) + ",";
    objet += "\"pressure\": " + String(bme.readPressure() / 100.0F) + ",";
    objet += "\"altitude\": " + String(bme.readAltitude(1013.25));
    objet += "}";

    
    request->send(200, "application/json", objet); });

    Serial.println("Serveur web disponible");
    AsyncElegantOTA.begin(&server); // Start ElegantOTA
    Serial.println("HTTP server started");

    server.begin();
}

void loop()
{
}

