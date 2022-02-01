// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>


// Replace with your network credentials
const char* ssid = "iS-Home2";
const char* password = "NDXSTNYYDC";

#define DHTPIN 2     // Digital pin connected to the DHT sensor

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;
float lux = 0.0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 10000;  

const char index_html[] PROGMEM = R"rawliteral(



<!DOCTYPE HTML><html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css" integrity="sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm" crossorigin="anonymous">
  <script src="https://code.jquery.com/jquery-3.2.1.slim.min.js" integrity="sha384-KJ3o2DKtIkvYIK3UENzmM7KCkRr/rE9/Qpg6aAZGJwFDMVNA/GpGFF93hXpG5KkN" crossorigin="anonymous"></script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.12.9/umd/popper.min.js" integrity="sha384-ApNbgh9B+Y1QKtv3Rn7W3mgPxhU9K/ScQsAP7hUibX39j7fakFPskvXusvfa0b4Q" crossorigin="anonymous"></script>
  <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/js/bootstrap.min.js" integrity="sha384-JZR6Spejh4U02d8jOt6vLEHfe/JQGiRRSQQxSfFWpi1MquVdAyjUar5+76PVCmYl" crossorigin="anonymous"></script>

</head>
<title>Big Tent </title>
<body>

<nav class="navbar navbar-secondary bg-secondary">
 
 <!-- Button trigger history modal-->
 <button type="button" class="btn btn-dark btn-sm" data-toggle="modal" data-target="#historyModal">
<i class="fas fa-chart-bar"></i> 
</button>

<!-- History Modal -->
<div class="modal fade" id="historyModal" tabindex="-1" role="dialog" aria-labelledby="settomgsModalLabel" aria-hidden="true">
  <div class="modal-dialog" role="document">
    <div class="modal-content">
      <div class="modal-header">
        <h5 class="modal-title" id="exampleModalLabel"><i class="fas fa-chart-bar"></i> 24 hour log</h5>
        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">&times;</span>
        </button>
      </div>
      <div class="modal-body">
        
<!--Here goes the 25 hour temp & humidity history-->
      
    </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-dark btn-sm" data-dismiss="modal">Close</button>       
      </div>
    </div>
  </div>
</div>

<!--TITLE-->
<h4 class="mb-1 text-light"><i class="fas fa-warehouse"></i> Big Tent</h4>


 <!-- Button trigger settings modal -->
<button type="button" class="btn btn-dark btn-sm" data-toggle="modal" data-target="#settingsModal">
<i class="fas fa-cogs"></i> 
</button>

<!-- Settings Modal -->
<div class="modal fade" id="settingsModal" tabindex="-1" role="dialog" aria-labelledby="settomgsModalLabel" aria-hidden="true">
  <div class="modal-dialog" role="document">
    <div class="modal-content">
      <div class="modal-header">
        <h5 class="modal-title" id="exampleModalLabel"><i class="fas fa-cogs"></i> Settings</h5>
        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">&times;</span>
        </button>
      </div>
      <div class="modal-body">
        
<table class="table table-hover text-justify">
<tbody>
    <tr>
      <th scope="row" class="border-0"><i class="fas fa-warehouse"></i><code> Title:</code> Big Tent </th> <th scope="row" class="border-0"><div class="text-right"><a href="#"><i class="fas fa-pencil-alt"></i>
</a></div></th>
  </tr>
  <tr>
      <th scope="row"><i class="fas fa-calendar"></i><code> Date:</code> 10.10.2021 </th> <th scope="row"><div class="text-right"><a href="#"><i class="fas fa-pencil-alt"></i>
</a></div></th>
  </tr>
  <tr>
      <th scope="row"><i class="fas fa-wifi"></i><code> WiFi:</code> iS-Home2 </th> <th scope="row"><div class="text-right"><a href="#"><i class="fas fa-pencil-alt"></i>
</a></div></th>
  </tr>

 </tbody>
</table>
      
    </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-dark btn-sm" data-dismiss="modal">Close</button>       
      </div>
    </div>
  </div>
</div>
 
</nav>

<div class="card bg-secondary  border-dark m-3 mb-3">

<div class="card-body text-center">
<!--Temperature & Humidity START-->
<div class="row">
  <div class="col m-auto">
<!--Temperature-->
<div class="alert alert-light mx-auto" role="alert">
   <span class="h4" id="tempIcon"></span>
   <span class="h4" id="temperature">%TEMPERATURE%</span>
   <sup class="units">&deg;C</sup>
</div>
  </div>  
  <div class="col m-auto">
<!--Humidity-->
  <div class="alert alert-light mx-auto" role="alert">
   <span class="h4" id="humIcon"></span>
   <span class="h4" id="humidity">%HUMIDITY%</span>
   <sup class="units">&#37;</sup>
</div>
  </div>
</div>

<!--Temperature & Humidity END-->

<!--On/Off & Light START-->
<div class="alert alert-light" role="alert">
   <span class="h4" id="lightStatus"></span>
   <span class="h4" id="light"> %LUX%</span>
   <sup class="units">lx</sup>
</div>
<!--On/Off & Light END-->

<!--DAY & WEEK START-->
<div class="row">
  <div class="col m-auto">
    <div class="alert alert-light" role="alert"><h4><p id="days" class="mb-0"></p></h4></div>
  </div>
  <div class="col m-auto">
    <div class="alert alert-light" role="alert"><h4><p id="weeks" class="mb-0"></p></h4></div>
  </div>
</div>
<!--DAY & WEEK END-->
   
<!--STAGE START-->
<div class="alert alert-success" role="alert">
  <span class="h4"><p id="stage" class="mb-0"></p></span>
</div>  
<!--STAGE END-->

</div>
</div>
 
 <!--FOOTER START-->
<div class="footer text-center">
 <i class="fas fa-square"></i> BlackB0x.io &reg; All Rights Reserved</span>
</div>  
<!--FOOTER END-->

</body>

<!--JavaScript Code-->

<script> 
let today = new Date().toISOString().slice(0, 10)

const startDate  = '2022-01-25'; // Start Date 
const endDate    = today; // Today's Date
{
const diffInMs   = new Date(endDate) - new Date(startDate)
const diffInDays = diffInMs / (1000 * 60 * 60 * 24);
document.getElementById("days").innerHTML = ("Day: ") + diffInDays; // Day Counter 
const diffInWeeks = diffInMs / (1000 * 60 * 60 * 24 * 7);
document.getElementById("weeks").innerHTML = ("Week: ") + (diffInWeeks).toFixed(); // Week Counter 
if (diffInWeeks < 4) {
  stageStatus = ("<i class=\"fas fa-seedling\" ></i>  Vegetation");
} else if (diffInWeeks > 6) {
  stageStatus = "<i class=\"fas fa-cannabis\" ></i> Budding";
} else {
  stageStatus = "<i class=\"fas fa-leaf\" ></i> Flowering";
}
document.getElementById("stage").innerHTML = stageStatus;
}

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
     const div = document.getElementById("temperature"), 
     temp = +this.responseText; // cast to number
     div.innerHTML = temp;
      let color = "#D4EDDA"; // default
       if (temp < 20) color = "#CCE5FF"
     else if (temp > 30) color = "#F8D7DA" 
      div.closest(".alert.alert-light").style.backgroundColor=color; 
      if (temp < 20) {
  tempIcon = ("<i class=\"fas fa-thermometer-empty\" > ");
} else if (temp> 30) {
  tempIcon = "<i class=\"fas fa-thermometer-full\" ></i> ";
} else {
  tempIcon = "<i class=\"fas fa-thermometer-half\" ></i> ";
}
document.getElementById("tempIcon").innerHTML = tempIcon;
     }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      const div = document.getElementById("humidity"), 
    humidity = this.responseText;
      div.innerHTML = humidity;
      let color = "#D4EDDA"; // default
       if (humidity < 35) color = "#F8D7DA"
       else if (humidity > 90) color = "#F8D7DA"
     div.closest(".alert.alert-light").style.backgroundColor=color; 
      if (humidity < 35) {
  humIcon = ("<i class=\"fas fa-tint-slash\" > ");
} else if (humidity> 90) {
  humIcon = "<i class=\"fas fa-water\" ></i> ";
} else {
  humIcon = "<i class=\"fas fa-tint\" ></i> ";
}
document.getElementById("humIcon").innerHTML = humIcon;
     }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      const div = document.getElementById("light"), 
    light = this.responseText;
      div.innerHTML = light;
      let color = "#FFF3CD"; // default
       if (light < 5) color = "#2E2626"
       else if (light > 5) color = "#FFF3CD"
     div.closest(".alert.alert-light").style.backgroundColor=color; 
    if (light < 4) {
  lightStatus = ("<i class=\"fas fa-moon\" ></i>  Night");
} else {
  lightStatus = "<i class=\"fas fa-sun\" ></i> Day";
}
document.getElementById("lightStatus").innerHTML = lightStatus;
}  
  };
  xhttp.open("GET", "/lux", true);
  xhttp.send();
}, 10000) ;
</script>
</html>

)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
  }
  else if(var == "LUX"){
    return String(lux);
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(9600);
  dht.begin();
  // Light Meter 
  Wire.begin(4,5);
  lightMeter.begin();

  //Hostname 
  String routername = "test";
  wifi_station_set_hostname(routername.c_str());
  WiFi.mode(WIFI_AP_STA); 
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }
  
  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h,1).c_str());
  });
  server.on("/lux", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(lux).c_str());
  });
  // Start server
  server.begin();
}
 
void loop(){  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature(); // currently C - put "true" for F 
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
      Serial.println(t,1);
      
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      Serial.println(h,1);
    }
        // Read Light
    float newLUX = lightMeter.readLightLevel();
    // if light read failed, don't change lux value 
    if (isnan(newLUX)) {
      Serial.println("Failed to read from Light sensor!");
    }
    else {
      lux = newLUX;
      Serial.println(lux);
    }
  }
}
