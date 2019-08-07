#include "heavenliClient.h"
#include "FastLED.h"
#include <PacketSerial.h>
//#include <Adafruit_NeoPixel.h>

#define NUM_LEDS 120
#define DATA_PIN A0
#define LED_PIN 12

//Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
CRGB strip[NUM_LEDS];

heavenliClient client;
PacketSerial   commPort;

uint8_t prevColors[10][3];

void setup() {
   client.init();
   client.lamp.setNumBulbs(3);
   client.lamp.setAngularOffset(float(180.0));

   commPort.setPacketHandler(&packetReceived);
   commPort.begin(115200);

   pinMode(DATA_PIN, INPUT);

   pinMode(5, OUTPUT);
   pinMode(9, OUTPUT);
   pinMode(10, OUTPUT);
   //pinMode(11, INPUT);
   analogWrite(10, 0);
   analogWrite(9, 0);
   analogWrite(5, 0);

   pinMode(LED_BUILTIN, OUTPUT);
   digitalWrite(LED_BUILTIN, LOW);

   //strip.begin();
   //strip.show();
   //strip.setBrightness(255);
   FastLED.addLeds<NEOPIXEL, LED_PIN>(strip, NUM_LEDS);
   FastLED.setBrightness(255);

   for (int i = 0; i < 3; i++)
      for (int j = 0; j < 10; j++)
         prevColors[j][i] = 0;
}

void loop() {
   uint8_t rgb[3];

   // Use manual override to enable all lights
   if (digitalRead(DATA_PIN) == HIGH) {
      client.useHostColors = false;
      rgb[0] = 255;
      rgb[1] = (255*2)/3;
      rgb[2] = 255/2;
      client.lamp.setBulbsTargetRGB(rgb);
      uint8_t LEDsPerBulb = NUM_LEDS / constrain(client.lamp.getNumBulbs(), 1, 6);
      for (int i = 0; i < client.lamp.getNumBulbs(); i++) {
         client.lamp.getBulbCurrentRGB(i, rgb);
         for (int j = 0; j < LEDsPerBulb; j++) {
            //strip.setPixelColor(i*LEDsPerBulb + j, strip.Color(rgb[0], (rgb[1]*2/3), rgb[2]/2));
            //strip.setPixelColor(i*LEDsPerBulb + j, strip.Color(rgb[0], rgb[1], rgb[2]));
            strip[constrain(j+i*LEDsPerBulb, 0, NUM_LEDS-1)] = CRGB(rgb[0], rgb[1], rgb[2]);
         }
      }
   } else {
      if (client.connectionEstablished) {
         client.useHostColors = true;
         uint8_t LEDsPerBulb = NUM_LEDS / constrain(client.lamp.getNumBulbs(), 1, 6);
         for (int i = 0; i < client.lamp.getNumBulbs(); i++) {
            client.lamp.getBulbCurrentRGB(i, rgb);
            for (int j = 0; j < LEDsPerBulb; j++) {
               //strip.setPixelColor(i*LEDsPerBulb + j, strip.Color(rgb[0], (rgb[1]*2/3), rgb[2]/2));
               //strip.setPixelColor(i*LEDsPerBulb + j, strip.Color(rgb[0], rgb[1], rgb[2]));
               strip[constrain(j+i*LEDsPerBulb, 0, NUM_LEDS-1)] = CRGB(rgb[0], rgb[1], rgb[2]);
            }
         }
      } else {
         client.useHostColors = false;
         rgb[0] = 0;
         rgb[1] = 0;
         rgb[2] = 0;
         client.lamp.setBulbsTargetRGB(rgb);
         uint8_t LEDsPerBulb = NUM_LEDS / constrain(client.lamp.getNumBulbs(), 1, 6);
         for (int i = 0; i < client.lamp.getNumBulbs(); i++) {
            client.lamp.getBulbCurrentRGB(i, rgb);
            for (int j = 0; j < LEDsPerBulb; j++) {
               //strip.setPixelColor(i*LEDsPerBulb + j, strip.Color(rgb[0], (rgb[1]*2/3), rgb[2]/2));
               //strip.setPixelColor(i*LEDsPerBulb + j, strip.Color(rgb[0], rgb[1], rgb[2]));
               strip[constrain(j+i*LEDsPerBulb, 0, NUM_LEDS-1)] = CRGB(rgb[0], rgb[1], rgb[2]);
            }
         }
      }
   }

   bool updateLEDs = false;
   for (int i = 0; i < client.lamp.getNumBulbs(); i++) {
      client.lamp.getBulbCurrentRGB(i, rgb);
      for (int j = 0; j < 3; j++) {
         if (prevColors[i][j] != rgb[j]) {
            updateLEDs = true;
            prevColors[i][j] = rgb[j];
         }
      }
   }

   if (updateLEDs) {
      //strip.show();
      FastLED.show();
      uint8_t bW[3];
      // Set brightness for white LED strips
      for (int i = 0; i < client.lamp.getNumBulbs(); i++) {
         uint8_t minVal = 255;
         client.lamp.getBulbCurrentRGB(i, rgb);
         /*
         if (  rgb[0] == 255  &&
               rgb[1] == 255  &&
               rgb[2] == 255  ){
            analogWrite(11-i, 255);
         } else {
            analogWrite(11-i, 0);
         }
         */
         for (int j = 0; j < 3; j++) {
            if ( rgb[j] < minVal )
               minVal = rgb[j];
         }

         bW[i] = minVal;
         
         switch(i) {
            case 0:
            analogWrite(5, minVal);
            break;

            case 1:
            analogWrite(10, minVal);
            break;

            case 2:
            analogWrite(9, minVal);
            break;
         }
         //analogWrite(11 - i, minVal);
      }
      bW[0] = map(bW[0], 0, 255, 0, 180);
      bW[1] = map(bW[1], 0, 255, 0, 180);
      bW[2] = map(bW[2], 0, 255, 0, 180);
   }

   size_t size;
   uint8_t buffer[56];
   size = client.outPacket(buffer);
   commPort.send(buffer, size);
   client.update();
   commPort.update();
}

void packetReceived(const uint8_t* buffer, size_t size) {
   client.processPacket(buffer, size); 
   return;
}
