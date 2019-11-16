/*
    SimpleWebServer_TEST.ino
    
    Author: Daniel Nistor, 2019
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server;

const char ssid[] = "Home";
const char password[] = "CDBD*D00-C!93-!!$0";

char webpage[] PROGMEM = R"=====(
<html>
<head></head>
<body>
 <h1>ESP8266 Simple Web Server</h1>
 <h2>LED State: <span id="led-state" style="color:red">__</span></h2>
 <button onclick="toggleLEDState()">Toggle LED</button>
</body>
<script>
 function readLEDState()
 {
     var xhr = new XMLHttpRequest();
     var url = "/ledState";

     xhr.onreadystatechange = function(){
         if (this.readyState == 4 && this.status == 200){
             document.getElementById("led-state").innerHTML = this.responseText;
         }
     };

     xhr.open("GET", url, true);
     xhr.send();
 }

function toggleLEDState()
{
     var xhr = new XMLHttpRequest();
     var url = "/toggle";
     xhr.open("GET", url, true);
     xhr.send();
}

document.addEventListener('DOMContentLoaded', readLEDState, false);

</script>
</html>
)=====";

const byte LED_PIN = 2;

void setup()
{
    // initialize
    WiFi.begin(ssid, password);
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    
    // wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    
    // connected
    Serial.println(" Connected.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    // route handling
    server.on("/", [](){ server.send_P(200, "text/html", webpage); });
    server.on("/ledState", getLEDState);
    server.on("/toggle", toggleLED);
    server.begin();
}

void loop()
{
    server.handleClient();
}

void toggleLED()
{
    digitalWrite(LED_PIN, digitalRead(LED_PIN) ? LOW : HIGH);
    server.send(200);
}

void getLEDState()
{
    String ledState = digitalRead(LED_PIN) ? "ON" : "OFF";
    server.send(200, "text/plain", ledState);
}
