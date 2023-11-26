#include <Thing.h>
#include <BlinkPattern.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

#define PIN D4
#define NUM_LEDS 7

#define BUFFER_SIZE 1024

using namespace ootb;

Thing thing;
BlinkPattern led(BUILTIN_LED);

BlinkPattern::Pattern<2> start{{1,9},25};
BlinkPattern::Pattern<2> normal{{1,39},25};
BlinkPattern::Pattern<0> disable{{},1000};

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

bool state = false;

void setup()
{
    Serial.begin(230400);
    Serial.println();
    Serial.println("Client:" + thing.clientId());

    led.setPattern(start);

    pixels.begin();

    pixels.setPixelColor(0, pixels.Color(10, 0, 0));
    pixels.show();

    thing.onStateChange([](const String& msg){
        Serial.println(msg);
    });

    thing.begin();

    String topic = "things/" + thing.clientId() + "/rgbled/color";
    thing.addActuator(topic, [](Value& value){
        state = (bool)value;
        StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
        String msg = value;
        JsonObject& root = jsonBuffer.parseObject(msg);

        if (!root.success()) {
            Serial.println("parseObject() failed");
            return false;
        }

        int r = root["r"];
        int g = root["g"];
        int b = root["b"];
        Serial.println(r);
        Serial.println(g);
        Serial.println(b);
        for (int i = 0 ; i < NUM_LEDS ; ++i)
        {
            pixels.setPixelColor(i, pixels.Color(r, g, b));
            pixels.show();
            delay(50);
        }
        Serial.println("Got:" + String(value));
        //thing.publish(thing.clientId() + "/rbgled/color", value);
        return true;
    });

    led.setPattern(normal);
}

void loop()
{
    thing.handle();
    led.handle();
}
