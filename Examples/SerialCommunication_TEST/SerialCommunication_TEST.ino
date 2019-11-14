/* Serial Communication Test*/

const byte LED_PIN = 13;

void setup()
{
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.begin(9600);
}

void loop()
{
    // wait for data
    if (Serial.available() > 0)
    {
        // process data
        char letter = Serial.read();

        if (letter == '1')
        {
            digitalWrite(LED_PIN, HIGH);
            Serial.println("LED: ON");
        }
        else if (letter == '0')
        {
            digitalWrite(LED_PIN, LOW);
            Serial.println("LED: OFF");
        }

        if (digitalRead(LED_PIN) == HIGH)
        {
            Serial.println("LED read as ON");
        }
        else
        {
            Serial.println("LED read as OFF");
        }
    }

    // wait for string
    String message = "";
    if (Serial.available() > 0)
    {
        while (Serial.available() > 0)
        {
            message += char(Serial.read());
            // delay(250); // try to decrease it to a more reasonable value if possible
        }

        Serial.println(message);
    }
}