#include <Wire.h>               // library for I2C communication
#include <WiFi.h>               // library for WiFi
#include <BH1750.h>             // library for BH1750 light sensor
#include <DHT.h>                // library for DHT temperature and humidity sensor
#include <Arduino.h>            // library for Arduino core functions
#include <Adafruit_Sensor.h>    // library for Adafruit sensors
#include <ESPAsyncWebSrv.h>     // library for asynchronous web server
#include <HTTPClient.h>         // library for httpclient to handle API's


// Replace with your network credentials
const char* ssid = "iS-Home2";
const char* password = "NDXSTNYYDC";

// Replace with your website details
const char* website = "192.168.1.10/api/store_data.php";
const char* api_key = "svetlio";
const int crop_id = 1;

// Replace with your sensor pins
#define DHTPIN 13 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE); // DHT object
BH1750 lightMeter; // BH1750 object

// Read sensor data
float temperature;
float humidity;
float light_intensity;
  
const int GREEN_LED_PIN = 2; // Define green LED pin
unsigned long previousMillis = 0; // Time when previous update occurred
const long interval = 10000; // Interval between updates (in milliseconds)

void setup() {
    // Serial port for debugging purposes
    Serial.begin(115200); // initialize serial communication with a baud rate of 9600
    dht.begin(); // initialize the DHT sensor
    Wire.begin(); // initialize I2C communication with SDA on GPIO5 and SCL on GPIO4
    lightMeter.begin(); // initialize the BH1750 sensor
    //
    pinMode(GREEN_LED_PIN, OUTPUT); // set the green LED pin as an output

    // Connect to Wi-Fi
    WiFi.begin(ssid, password); // Connect to Wi-Fi with specified SSID and password
    Serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) { // Wait until Wi-Fi is connected
        delay(5000);
        Serial.print(".");
    }

    // Print ESP32 Local IP Address
    Serial.println(WiFi.localIP()); // Print local IP address of ESP32
 }

void loop() {
static unsigned long previousMillis = 0;
const long interval = 10000;
unsigned long currentMillis = millis();

if (currentMillis - previousMillis >= interval) {
previousMillis = currentMillis;

arduino

// Read temperature as Celsius (the default)
float newT = dht.readTemperature(); // Read temperature from DHT sensor
if (isnan(newT)) {
  Serial.println("Failed to read temperature from DHT sensor!");
} else {
  temperature = newT; // Update temperature value
  Serial.println("Temperature: " + String(temperature) + "°C");
}

// Read humidity
float newH = dht.readHumidity(); // Read humidity from DHT sensor
if (isnan(newH)) {
  Serial.println("Failed to read humidity from DHT sensor!");
} else {
  humidity = newH; // Update humidity value
  Serial.println("Humidity: " + String(humidity) + "%");
}

// Read light
float newLUX = lightMeter.readLightLevel(); // Read light level from light sensor
if (isnan(newLUX)) {
  Serial.println("Failed to read light level from sensor!");
} else {
  light_intensity = newLUX; // Update light value
  Serial.println("Light intensity: " + String(light_intensity) + " lx");
}

// Format data as JSON
String json_data = "{\"crop_id\": " + String(crop_id) + ", \"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + ", \"light_intensity\": " + String(light_intensity) + "}";

// Send data to API endpoint
WiFiClient client;
HTTPClient http;
Serial.print("Sending data to API endpoint...");
digitalWrite(GREEN_LED_PIN, HIGH); // turn on LED to indicate data is being sent
if (http.begin(client, website)) {
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(api_key));
  int http_code = http.POST(json_data);
  if (http_code == 200) {
    Serial.println("Success");
  } else {
    Serial.println("Failed (HTTP error code " + String(http_code) + ")");
  }
  http.end();
} else {
  Serial.println("Unable to connect to API endpoint");
}
digitalWrite(GREEN_LED_PIN, LOW); // turn off LED to indicate data transmission is complete

}
}
