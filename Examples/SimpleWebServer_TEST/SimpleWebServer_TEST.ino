/*
    SimpleWebServer_TEST.ino
    
    Author: Daniel Nistor, 2019
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server;

const char ssid[] = "Home";
const char password[] = "ToDo";

char webpage[] PROGMEM = R"=====(
<html>
<head></head>
<body>
 TODO
</body>
<script>
 TODO
</script>
</html>
)=====";

void toggleLED()
{
    const byte LED_PIN = 2;
    static bool isOn = digitalRead(LED_PIN);
    digitalWrite(LED_PIN, (isOn) ? LOW : HIGH);
    isOn = !isOn;
}

void setup()
{
    // initialize
    WiFi.begin(ssid, password);
    Serial.begin(115200);
    
    // wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    
    // connected
    Serial.println(" Connected.");
    Serial.print("IP Address: ";
    Serial.println(WiFi.localIP());
    
    // route handling
    server.on("/", [](){ server.send(200, "text/plain", "<h1>Hello Daniel!</h1>"); });
    server.on("/page", [](){ server.send_P(200, "text/plain", webpage); });
    server.on("/toggle", toggleLED);
    server.begin();
}

void loop()
{
    server.handleClient();
}