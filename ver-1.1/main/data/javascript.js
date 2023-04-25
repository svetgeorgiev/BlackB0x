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

setInterval(function() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      const div = document.getElementById("temperature");
      const temp = +this.responseText;
      div.innerHTML = temp;
      
      let color = "#D4EDDA"; // default
      if (temp < 20) color = "#CCE5FF";
      else if (temp > 30) color = "#F8D7DA";
      div.closest(".alert.alert-light").style.backgroundColor=color; 
      
      let tempIcon = "";
      if (temp < 20) {
        tempIcon = "<i class=\"fas fa-thermometer-empty\"></i>";
      } else if (temp > 30) {
        tempIcon = "<i class=\"fas fa-thermometer-full\"></i>";
      } else {
        tempIcon = "<i class=\"fas fa-thermometer-half\"></i>";
      }
      document.getElementById("tempIcon").innerHTML = tempIcon;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000);

setInterval(function() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      const div = document.getElementById("humidity");
      const humidity = this.responseText;
      div.innerHTML = humidity;
      
      let color = "#D4EDDA"; // default
      if (humidity < 35) color = "#F8D7DA";
      else if (humidity > 90) color = "#F8D7DA";
      div.closest(".alert.alert-light").style.backgroundColor=color; 
      
      let humIcon = "";
      if (humidity < 35) {
        humIcon = "<i class=\"fas fa-tint-slash\"></i>";
      } else if (humidity > 90) {
        humIcon = "<i class=\"fas fa-water\"></i>";
      } else {
        humIcon = "<i class=\"fas fa-tint\"></i>";
      }
      document.getElementById("humIcon").innerHTML = humIcon;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000);

setInterval(function() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      const div = document.getElementById("light");
      const light = this.responseText;
      div.innerHTML = light;
      
      let color = "#FFF3CD"; // default
      if (light < 5) color = "#2E2626";
      else if (light > 5) color = "#FFF3CD";
      div.closest(".alert.alert-light").style.backgroundColor=color; 
      
      let lightStatus = "";
      if (light < 4) {
        lightStatus = "<i class=\"fas fa-moon\"></i> Night";
      } else {
        lightStatus = "<i class=\"fas fa-sun\"></i> Day";
      }
      document.getElementById("lightStatus").innerHTML = lightStatus;
    }
  };
  xhttp.open("GET", "/lux", true);
  xhttp.send();
}, 10000);
