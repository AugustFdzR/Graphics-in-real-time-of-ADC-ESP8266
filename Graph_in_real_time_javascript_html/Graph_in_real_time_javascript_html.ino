#include<ESP8266WiFi.h>
#include<WifiClient.h>
#include<ESP8266WebServer.h>
#include<ESP8266mDNS.h>
#include <ESP8266WiFiMulti.h>

#define LED 2  //On board LED

const char index_html[] PROGMEM= {"<!doctype html>\n<html>\n \n<head>\n\t<title>TITULO | TITULO</title>\n\t<!--For offline ESP graphs see this tutorial https://circuits4you.com/2018/03/10/esp8266-jquery-and-ajax-web-server/ -->\n\t<script src = \"https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.7.3/Chart.min.js\"></script>\t\n\t<style>\n\tcanvas{\n\t\t-moz-user-select: none;\n\t\t-webkit-user-select: none;\n\t\t-ms-user-select: none;\n\t}\n \n\t/* Data Table Styling */\n\t#dataTable {\n\t  font-family: \"Trebuchet MS\", Arial, Helvetica, sans-serif;\n\t  border-collapse: collapse;\n\t  width: 100%;\n\t}\n \n\t#dataTable td, #dataTable th {\n\t  border: 1px solid #ddd;\n\t  padding: 8px;\n\t}\n \n\t#dataTable tr:nth-child(even){background-color: #f2f2f2;}\n \n\t#dataTable tr:hover {background-color: #ddd;}\n \n\t#dataTable th {\n\t  padding-top: 12px;\n\t  padding-bottom: 12px;\n\t  text-align: left;\n\t  background-color: #4CAF50;\n\t  color: white;\n\t}\n\t</style>\n</head>\n \n<body>\n    <div style=\"text-align:center;\"><b>grafica</b><br>FERNANDEZ RIOS CESAR AUGUSTO</br> </div>\n    <div class=\"chart-container\" position: relative; height:350px; width:100%\">\n        <canvas id=\"Chart\" width=\"400\" height=\"400\"></canvas>\n    </div>\n<div>\n\t<table id=\"dataTable\">\n\t  <tr><th>Time</th><th>Value</th></tr>\n\t</table>\n</div>\n<br>\n<br>\t\n \n<script>\n//Graphs visit: https://www.chartjs.org\nvar values = [];\nvar timeStamp = [];\nfunction showGraph()\n{\n    for (i = 0; i < arguments.length; i++) {\n    \tvalues.push(arguments[i]);    \n    }\n \n    var ctx = document.getElementById(\"Chart\").getContext('2d');\n    var Chart2 = new Chart(ctx, {\n        type: 'line',\n        data: {\n            labels: timeStamp,\t//Bottom Labeling\n            datasets: [{\n                label: \"BPM\",\n                fill: false,\t//Try with true\n                backgroundColor: 'rgba( 243, 156, 18 , 1)', //Dot marker color\n                borderColor: 'rgba( 243, 156, 18 , 1)',\t//Graph Line Color\n                data: values,\n            }],\n        },\n        options: {\n            title: {\n                    display: true,\n                    text: \"Something\"\n                },\n            maintainAspectRatio: false,\n            elements: {\n            line: {\n                    tension: 0.5 //Smoothening (Curved) of data lines\n                }\n            },\n            scales: {\n                    yAxes: [{\n                        ticks: {\n                            beginAtZero:true\n                        }\n                    }]\n            }\n        }\n    });\n \n}\n \n//On Page load show graphs\nwindow.onload = function() {\n\tconsole.log(new Date().toLocaleTimeString());\n\tshowGraph(5,10,4,58);\n};\n \n//Ajax script to get ADC voltage at every 5 Seconds \n//Read This tutorial https://circuits4you.com/2018/02/04/esp8266-ajax-update-part-of-web-page-without-refreshing/\n \nsetInterval(function() {\n  // Call a function repetatively with 5 Second interval\n  getData();\n}, 400); //5000mSeconds update rate\n \nfunction getData() {\n  var xhttp = new XMLHttpRequest();\n  xhttp.onreadystatechange = function() {\n    if (this.readyState == 4 && this.status == 200) {\n     //Push the data in array\n\tvar time = new Date().toLocaleTimeString();\n\tvar ADCValue = this.responseText; \n      values.push(ADCValue);\n      timeStamp.push(time);\n      showGraph();\t//Update Graphs\n\t//Update Data Table\n\t  var table = document.getElementById(\"dataTable\");\n\t  var row = table.insertRow(1);\t//Add after headings\n\t  var cell1 = row.insertCell(0);\n\t  var cell2 = row.insertCell(1);\n\t  cell1.innerHTML = time;\n\t  cell2.innerHTML = ADCValue;\n    }\n  };\n  xhttp.open(\"GET\", \"readADC\", true);\t//Handle readADC server on ESP8266\n  xhttp.send();\n}\n\t\t\n</script>\n</body>\n \n</html>"};
//SSID and Password of your WiFi router
const char* ssid = "";
const char* password = "";
 
ESP8266WebServer server(80); //Server on port 80
 
//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
String s = index_html; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}
 
void handleADC() {
 int a = analogRead(A0);
 String adcValue = String(a);
 digitalWrite(LED,!digitalRead(LED)); //Toggle LED on data request ajax
 server.send(200, "text/plane", adcValue); //Send ADC value only to client ajax request
}
//==============================================================
//                  SETUP
//==============================================================
void setup(void){
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
 
  //Onboard LED port Direction output
  pinMode(LED,OUTPUT); 
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
 
  server.on("/", handleRoot);      //Which routine to handle at root location. This is display page
  server.on("/readADC", handleADC); //This page is called by java Script AJAX
 
  server.begin();                  //Start server
  Serial.println("HTTP server started");
}
//==============================================================
//                     LOOP
//==============================================================
void loop(void){
  server.handleClient();    
}
