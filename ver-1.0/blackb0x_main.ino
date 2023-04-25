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
const char* ssid = "SetYourWiFi";
const char* password = "SetWiFiPassword";

#define DHTPIN 2          // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22     // DHT 22 (AM2302)
#define GREEN_LED_PIN 16  //set GREEN LED pin

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

<!DOCTYPE HTML>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css" integrity="sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm" crossorigin="anonymous">
  <script src="https://code.jquery.com/jquery-3.2.1.slim.min.js" integrity="sha384-KJ3o2DKtIkvYIK3UENzmM7KCkRr/rE9/Qpg6aAZGJwFDMVNA/GpGFF93hXpG5KkN" crossorigin="anonymous"></script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.12.9/umd/popper.min.js" integrity="sha384-ApNbgh9B+Y1QKtv3Rn7W3mgPxhU9K/ScQsAP7hUibX39j7fakFPskvXusvfa0b4Q" crossorigin="anonymous"></script>
  <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/js/bootstrap.min.js" integrity="sha384-JZR6Spejh4U02d8jOt6vLEHfe/JQGiRRSQQxSfFWpi1MquVdAyjUar5+76PVCmYl" crossorigin="anonymous"></script>
  <script src="javascript.js"></script>

<style type="text/css">
    a[disabled="disabled"] {
        pointer-events: none;
    }
</style>


</head>
<title>BlackB0x - Grow Monitor</title>
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
        
<!--Here goes the 24 hour temp & humidity history-->
      
    </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-dark btn-sm" data-dismiss="modal">Close</button>       
      </div>
    </div>
  </div>
</div>

<!--TITLE-->
<h4 class="mb-1 text-light"><i class="fas fa-warehouse"></i> BlackB0x</h4>


 <!-- Button trigger settings modal -->
<button type="button" class="btn btn-dark btn-sm" data-toggle="modal" data-target="#settingsModal">
<i class="fas fa-cogs"></i> 
</button>

<div class="modal" id="settingsModal">
  <div class="modal-dialog modal-lg">
      <div class="modal-content">
        <div class="modal-header">
          <h4 class="modal-title">Settings</h4>    
          <button type="button" class="close" data-dismiss="modal" aria-hidden="true">×</button>
        </div><div class="container"></div>
        <div class="modal-body">

<table class="table table-hover text-justify">
<tbody>
    <tr>
      <th scope="row" class="border-0"><i class="fas fa-warehouse"></i><code> Title:</code> $NAME </th> <th scope="row" class="border-0"><div class="text-right"><a data-toggle="modal" href="#nameModal" class="btn btn-primary"><i class="fas fa-pencil-alt"></i>
</a></div></th>
  </tr>
  <tr>
      <th scope="row"><i class="fas fa-calendar"></i><code> Date:</code> $DATE </th> <th scope="row"><div class="text-right"><a data-toggle="modal" href="#dateModal" class="btn btn-primary"><i class="fas fa-pencil-alt"></i>
</a></div></th>
  </tr>
  <tr>
      <th scope="row"><i class="fas fa-wifi"></i><code> WiFi:</code> $WIFI </th> <th scope="row"><div class="text-right"><a data-toggle="modal" href="#wifiModal" class="btn btn-primary"><i class="fas fa-pencil-alt"></i>
</a></div></th>
  </tr>

 </tbody>
</table>

        </div>
        <div class="modal-footer">
          <a href="#" data-dismiss="modal" class="btn">Close</a>
        </div>
      </div>
    </div>
</div>
<!--NAME SETTINGS MODAL START-->
<div class="modal" id="nameModal" data-backdrop="static">
  <div class="modal-dialog">
      <div class="modal-content">
        <div class="modal-header">
          <h4 class="modal-title">Name Settings</h4>
          <button type="button" class="close" data-dismiss="modal" aria-hidden="true">×</button>
        </div><div class="container"></div>
        <div class="modal-body">

<form action="/get">
  Please choose name: <input type="text" name="name">
  


        </div>
        <div class="modal-footer">
          <a href="#" data-dismiss="modal" class="btn">Close</a>
          <input type="submit" value="Submit" class="btn btn-primary"></form>
        </div>
      </div>
    </div>
</div>
<!--NAME SETTINGS MODAL END-->

<!--DATE SETTINGS MODAL START-->
<div class="modal" id="dateModal" data-backdrop="static">
  <div class="modal-dialog">
      <div class="modal-content">
        <div class="modal-header">
          <h4 class="modal-title">Date Settings</h4>
          <button type="button" class="close" data-dismiss="modal" aria-hidden="true">×</button>
        </div><div class="container"></div>
        <div class="modal-body">

<form action="/get">
  Please choose date: <input type="date" name="date">
  


        </div>
        <div class="modal-footer">
          <a href="#" data-dismiss="modal" class="btn">Close</a>
          <input type="submit" value="Submit" class="btn btn-primary"></form>
        </div>
      </div>
    </div>
</div>
<!--DATE SETTINGS MODAL END-->

<!--WIFI SETTINGS MODAL START-->
<div class="modal" id="wifiModal" data-backdrop="static">
  <div class="modal-dialog">
      <div class="modal-content">
        <div class="modal-header">
          <h4 class="modal-title">WiFi Settings</h4>
          <button type="button" class="close" data-dismiss="modal" aria-hidden="true">×</button>
        </div><div class="container"></div>
        <div class="modal-body">

<form action="/get">
  <input type="text" name="ssid" value="SSID name:"><br>
  <input type="password" name="password" value="SSID pass:"><br>

        </div>
        <div class="modal-footer">
          <a href="#" data-dismiss="modal" class="btn">Close</a>
          <input type="submit" value="Submit" class="btn btn-primary"></form>
        </div>
      </div>
    </div>
</div>
<!--WIFI SETTINGS MODAL END-->
 
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
<!--DAY & WEEK END-->
 </div>  
<!--STAGE START-->
<div class="alert alert-success" role="alert">
  <span class="h4"><p id="stage" class="mb-0"></p></span>
</div>  
<!--STAGE END-->

<!-- WEATHER WIDGET STARTS HERE GO TO https://weatherwidget.io TO SET UP YOURS-->
<div class="alert alert-light" role="alert">
<span class="h4"><p class="mb-1"><i class="fas fa-cloud-sun"></i> Forecast</p></span>
<a class="weatherwidget-io" href="https://forecast7.com/en/52d350d15/cb6-3ty/" data-font="Arial" data-icons="Climacons Animated" data-mode="Forecast" data-days="3" data-shadow="#ffffff" data-textcolor="#818182" data-highcolor="#ff0000" data-lowcolor="#0b6aa5" data-suncolor="#ffe60e" data-mooncolor="#bbbbbb" data-cloudcolor="#787676" data-cloudfill="#959595" data-raincolor="#4a00ff" data-snowcolor="#6edff2" disabled="disabled">BlackB0x Weather Widged</a>
<script>
!function(d,s,id){var js,fjs=d.getElementsByTagName(s)[0];if(!d.getElementById(id)){js=d.createElement(s);js.id=id;js.src='https://weatherwidget.io/js/widget.min.js';fjs.parentNode.insertBefore(js,fjs);}}(document,'script','weatherwidget-io-js');
</script>
<!-- WEATHER WIDGET END -->

</div>
</div>
</div>
<!-- DO NOT REMOVE - FOOTER START - DO NOT REMOVE -->
<div class="footer text-center">
 <i class="fas fa-square"></i> BlackB0x Grow Monitor &reg;</span>
</div>  
<!-- DO NOT REMOVE - FOOTER END - DO NOT REMOVE -->

</body>

<!--JavaScript Code-->

<script> 
let today = new Date().toISOString().slice(0, 10)

const startDate  = '2022-01-01'; //  <-- HERE SET THE START DATE 
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
}, 5000) ;

//Settings Modal's 

$("#nameModal").on('show.bs.modal', function (a) {
    $("#settingsModal").modal("show");
});
  $("#dateModal").on('show.bs.modal', function (b) {
    $("#settingsModal").modal("show");
});
  $("#wifiModal").on('show.bs.modal', function (c) {
    $("#settingsModal").modal("show");
});
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
  Wire.begin(5,4);
  lightMeter.begin();
  // Green LED to indicate when is ON
  pinMode(GREEN_LED_PIN, OUTPUT);
  
  //Hostname 
  String routername = "BlackB0x Grow Monitor";
  wifi_station_set_hostname(routername.c_str());
  WiFi.mode(WIFI_AP_STA); 
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.println(".");
  }
  
  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t,1).c_str());
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
  digitalWrite(GREEN_LED_PIN, HIGH);
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
      Serial.println(t);
      
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      Serial.println(h);
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
