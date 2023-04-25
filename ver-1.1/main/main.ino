// Import required libraries
#include <FS.h>                 // file system library
#include <DHT.h>                // library for DHT temperature and humidity sensor
#include <Wire.h>               // library for I2C communication
#include <Hash.h>               // library for hash functions
#include <BH1750.h>             // library for BH1750 light sensor
#include <SPIFFS.h>             // SPI Flash File System library
#include <Arduino.h>            // library for Arduino core functions
#include <ESP8266WiFi.h>        // library for ESP8266 Wi-Fi module
#include <ESPAsyncTCP.h>        // library for asynchronous TCP/IP communication
#include <Adafruit_Sensor.h>    // library for Adafruit sensors
#include <ESPAsyncWebServer.h>  // library for asynchronous web server


// Replace with your network credentials
const char * ssid = "SetYourWiFi"; // SSID of the Wi-Fi network to connect to
const char * password = "SetWiFiPassword"; // Password of the Wi-Fi network

#define DHTPIN 2 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302)
#define GREEN_LED_PIN 16 // Digital pin connected to the green LED

DHT dht(DHTPIN, DHTTYPE); // DHT object
BH1750 lightMeter; // BH1750 object

// define SPIFFS
#define SPIFFS_START_ADDR 0x10000 // start address for SPIFFS
#define SPIFFS_SIZE 0x3F0000 // size of SPIFFS

// current temperature & humidity, updated in loop()
float t = 0.0; // temperature
float h = 0.0; // humidity
float lux = 0.0; // light intensity

// Create AsyncWebServer object on port 80
AsyncWebServer server(80); // create an asynchronous web server on port 80

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0; // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 10000; // update interval in milliseconds

// Replaces placeholder with DHT values
String processor(const String &
  var) {
  //Serial.println(var);
  if (var == "TEMPERATURE") { // replace the placeholder with temperature
    return String(t); // return temperature as a string
  } else if (var == "HUMIDITY") { // replace the placeholder with humidity
    return String(h); // return humidity as a string
  } else if (var == "LUX") { // replace the placeholder with light intensity
    return String(lux); // return light intensity as a string
  }
  return String(); // return an empty string if the placeholder is not recognized
}

void setup() {
    // Serial port for debugging purposes
    Serial.begin(9600); // initialize serial communication with a baud rate of 9600
    dht.begin(); // initialize the DHT sensor
    Wire.begin(5, 4); // initialize I2C communication with SDA on GPIO5 and SCL on GPIO4
    lightMeter.begin(); // initialize the BH1750 sensor
    pinMode(GREEN_LED_PIN, OUTPUT); // set the green LED pin as an output

    //Hostname 
    String routername = "BlackB0x Grow Monitor"; // set the hostname for the ESP8266 module
    wifi_station_set_hostname(routername.c_str()); // set the hostname for the Wi-Fi station
    WiFi.mode(WIFI_AP_STA); // set the Wi-Fi mode to station and access point

    // Connect to Wi-Fi
    WiFi.begin(ssid, password); // Connect to Wi-Fi with specified SSID and password
    Serial.println("Connecting to WiFi");
    server.on("/", HTTP_GET, handleRoot);
    server.on("/data", HTTP_GET, handleData);
    while (WiFi.status() != WL_CONNECTED) { // Wait until Wi-Fi is connected
      delay(5000);
      Serial.println(".");
    }

    // Print ESP8266 Local IP Address
    Serial.println(WiFi.localIP()); // Print local IP address of ESP8266

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) { // Handle HTTP GET request for root URL
      request -> send_P(200, "text/html", index_html, processor); // Send response with specified HTML page and data processor function
    });
    server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest * request) { // Handle HTTP GET request for temperature data
      request -> send_P(200, "text/plain", String(t, 1).c_str()); // Send response with temperature data as plain text
    });
    server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest * request) { // Handle HTTP GET request for humidity data
      request -> send_P(200, "text/plain", String(h, 1).c_str()); // Send response with humidity data as plain text
    });
    server.on("/lux", HTTP_GET, [](AsyncWebServerRequest * request) { // Handle HTTP GET request for light data
      request -> send_P(200, "text/plain", String(lux).c_str()); // Send response with light data as plain text
    });
if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    // Handle the error here (e.g. log the error, display an error message, etc.)
  } else {
    // Continue with the setup if SPIFFS is successfully mounted
    switchOnTone(); // Indicate with "Switching ON" sound
    Serial.println("Home Position Updated: " + String(homeLatString) + " / " + String(homeLngString));
  }
}

    void loop() {
      digitalWrite(GREEN_LED_PIN, HIGH); // Turn on the green LED
      unsigned long currentMillis = millis(); // Get current time
      if (currentMillis - previousMillis >= interval) { // Check if specified interval has elapsed
        // save the last time you updated the DHT values
        previousMillis = currentMillis; // Update previous time to current time
        // Read temperature as Celsius (the default)
        float newT = dht.readTemperature(); // Read temperature from DHT sensor
        // if temperature read failed, don't change t value
        if (isnan(newT)) {
          Serial.println("Failed to read from DHT sensor!");
        } else {
          t = newT; // Update temperature value
          Serial.println(t);
        }
        // Read Humidity
        float newH = dht.readHumidity(); // Read humidity from DHT sensor
        // if humidity read failed, don't change h value 
        if (isnan(newH)) {
          Serial.println("Failed to read from DHT sensor!");
        } else {
          h = newH; // Update humidity value
          Serial.println(h);
        }
        // Read Light
        float newLUX = lightMeter.readLightLevel(); // Read light level from light sensor
        // if light read failed, don't change lux value 
        if (isnan(newLUX)) {
          Serial.println("Failed to read from Light sensor!");
        } else {
          lux = newLUX; // Update light value
          Serial.println(lux);
        }
      }
    }

void handleRoot(AsyncWebServerRequest * request) {
  // Read temperature as Celsius (the default)
  float newT = dht.readTemperature(); // Read temperature from DHT sensor
  // if temperature read failed, don't change t value
  if (isnan(newT)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    t = newT; // Update temperature value
  }
  // Read Humidity
  float newH = dht.readHumidity(); // Read humidity from DHT sensor
  // if humidity read failed, don't change h value 
  if (isnan(newH)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    h = newH; // Update humidity value
  }
  // Read Light
  float newLUX = lightMeter.readLightLevel(); // Read light level from light sensor
  // if light read failed, don't change lux value 
  if (isnan(newLUX)) {
    Serial.println("Failed to read from Light sensor!");
  } else {
    lux = newLUX; // Update light value
  }
  
  // Open the HTML file
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    Serial.println("Failed to open file");
    request->send(500);
    return;
  }
  
  // Read the contents of the file into a String
  String html = file.readString();
  file.close();
  
  // Replace the placeholders in the HTML file with the sensor values
  html.replace("{TEMPERATURE}", String(t, 1));
  html.replace("{HUMIDITY}", String(h, 1));
  html.replace("{LUX}", String(lux, 1));
  
  // Send the modified HTML file back to the client
  request->send(200, "text/html", html);
}
