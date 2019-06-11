#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"
#include <PacketSerial.h>

#define NUM_LEDS 10
#define DATA_PIN 7
#define LED_PIN 17

CRGB strip[NUM_LEDS];

float bulbsCurRGB[6][3];
float bulbsTarRGB[6][3];

char inByte;
float tDiff = 0.01;
int numBulbs = 3;
int curBulb = 0;
int commTimer = 0;
bool connectionEstablished = false;
bool receivedMessage = false;
bool readOrLight = true;
char inMessage[8];
int timer = 0;

PacketSerial lightBar;

void setup() {
   lightBar.begin(115200);
   lightBar.setPacketHandler(&onPacketReceived);
   FastLED.addLeds<NEOPIXEL, LED_PIN>(strip, NUM_LEDS);
   FastLED.setBrightness(255);

   pinMode(DATA_PIN, INPUT);

   pinMode(9, OUTPUT);
   pinMode(10, OUTPUT);
   pinMode(11, OUTPUT);

   pinMode(LED_BUILTIN, OUTPUT);

   for (int i = 0; i < 6; i++) {
      bulbsCurRGB[i][0] = 0.0;
      bulbsCurRGB[i][1] = 0.0;
      bulbsCurRGB[i][2] = 0.0;

      bulbsTarRGB[i][0] = 0.5;
      bulbsTarRGB[i][1] = 0.1;
      bulbsTarRGB[i][2] = 1.0;
   }

   digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
   digitalWrite(LED_BUILTIN, HIGH);
   if (digitalRead(DATA_PIN) == HIGH) {
      uint8_t packet[4] = "HIGH";
      lightBar.send(packet, 4);
      connectionEstablished = false;
      // If manual override switch is enabled, set ALL leds to max brightness (RGB+W)
      for (int i = 0; i < numBulbs; i++) {
         bulbsTarRGB[i][0] = 1.0;
         bulbsTarRGB[i][1] = 1.0;
         bulbsTarRGB[i][2] = 1.0;
      }

      if (curBulb > numBulbs)
         curBulb = 0;
      else
         curBulb += 1;
   } else {
      uint8_t packet[3] = "LOW";
      lightBar.send(packet, 3);
      if (!connectionEstablished) {
         // ... or just turn the lights off
         for (int i = 0; i < numBulbs; i++) {
            bulbsTarRGB[i][0] = 0.0;
            bulbsTarRGB[i][1] = 0.0;
            bulbsTarRGB[i][2] = 0.0;
         }
      
         if (curBulb > numBulbs)
            curBulb = 0;
         else
            curBulb += 1;
      }
   }

   updateLEDs(tDiff / 2.0);

   int LEDsPerBulb = NUM_LEDS / constrain(numBulbs, 1, 6);
   int tmr = int(animSigmoid(bulbsCurRGB[curBulb][0]) * 255.0);
   int tmg = int(animSigmoid(bulbsCurRGB[curBulb][1]) * 192.0);
   int tmb = int(animSigmoid(bulbsCurRGB[curBulb][2]) * 164.0);
   for (int i = 0; i < LEDsPerBulb; i++) {
      strip[constrain(i + curBulb * LEDsPerBulb, 0, NUM_LEDS-1)] = CRGB(tmr, tmg, tmb);
   }

   // Set brightness for white LED strips
   for (int i = 0; i < numBulbs; i++) {
      float minVal = 1.0;
      for (int j = 0; j < 3; j++) {
         if ( bulbsCurRGB[i][j] < minVal )
            minVal = bulbsCurRGB[i][j];
      }
      analogWrite(11 - i, int(animSigmoid(minVal) * 255.0));
   }

      /*
   if (Serial.available() == 8 ) {
      Serial.readBytes(inMessage, 8);
      if (  inMessage[0] == 'h'  &&
            inMessage[1] == 'e'  &&
            inMessage[2] == 'l'  &&
            inMessage[3] == 'l'  &&
            inMessage[4] == 'o'  ){
         connectionEstablished = true;
      }
      if (connectionEstablished == true) {
         bulbsTarRGB[1][0] = 0.1;
         bulbsTarRGB[1][1] = 0.1;
         bulbsTarRGB[1][2] = 0.1;
         curBulb = constrain(inMessage[1], 0, numBulbs-1);

         bulbsTarRGB[curBulb][0] = pow(float(inMessage[2]) / 128.0, 2.0);
         bulbsTarRGB[curBulb][1] = pow(float(inMessage[3]) / 128.0, 2.0);
         bulbsTarRGB[curBulb][2] = pow(float(inMessage[4]) / 128.0, 2.0);
      }
      receivedMessage = true;
   } 
      */

   FastLED.show();
   //int tmt = millis();
   //if (tmt - commTimer >= 1000) {
      /* Update Serial Communications */
      //commTimer = tmt;
      //delay(10);
      lightBar.update();
   //} else {
   //}
}

void onPacketReceived(const uint8_t* buffer, size_t size) {
   // Make a temporary buffer.
   uint8_t tempBuffer[size];
   digitalWrite(LED_BUILTIN, LOW);

   // Copy the packet into our temporary buffer.
   //memcpy(tempBuffer, buffer, size);

   /*
   if (size > 0) {
      for (int i = 0; i < size; i ++)
         tempBuffer[i] = buffer[i];

      lightBar.send(tempBuffer, size);
      receivedMessage = true;
   } else {
      uint8_t packet[35] = {'[', 'C', 'L', 'I', 'E', 'N', 'T', ']', ' ', 'P', 'a', 'c', 'k', 'e', 't', ' ', 'R', 'e', 'c', 'e', 'i', 'v', 'e', 'd', ' ', 'o', 'f', ' ', 's', 'i', 'z', 'e', ':', ' ', char(size+'0')};
      lightBar.send(packet, 35);
   }
   */

}

float animSigmoid(float x){
   if (x <= 0.0)
      return 0.0;
   if (x >= 1.0)
      return 1.0;

   float tmp = x - 0.5;
   tmp *= 8.0;
   tmp = atan(tmp);
   tmp /= 2.64;
   tmp += 0.5;
   return tmp;
}

void updateLEDs(float frameTime) {
   for (int i = 0; i < numBulbs; i++) {
      float r = bulbsCurRGB[i][0];
      float g = bulbsCurRGB[i][1];
      float b = bulbsCurRGB[i][2];

      float rT = bulbsTarRGB[i][0];
      float gT = bulbsTarRGB[i][1];
      float bT = bulbsTarRGB[i][2];

      if (  r != rT  ||
            g != gT  ||
            b != bT  ) {
         float difR = abs(r - rT);
         float difG = abs(g - gT);
         float difB = abs(b - bT);
         float rd = 0.0;
         float gd = 0.0;
         float bd = 0.0;
         float delta = frameTime;
         float threshold = 0.05;
         float tmf = float((i + 2) * 2) / float(constrain(numBulbs * 3, 1, 18));
         delta *= tmf;

         if (difR > threshold)
            if (rT > r)
               rd = delta;
            else
               rd = -delta;

         if (difG > threshold)
            if (gT > g)
               gd = delta;
            else
               gd = -delta;

         if (difB > threshold)
            if (bT > b)
               bd = delta;
            else
               bd = -delta;

         if (difR > threshold)
            difR = r + rd;
         else
            difR = rT;

         if (difG > threshold)
            difG = g + gd;
         else
            difG = gT;

         if (difB > threshold)
            difB = b + bd;
         else
            difB = bT;

         if (difR >= 1.0)
            difR = 1.0;
         else if (difR <= 0.0)
            difR = 0.0;

         if (difB >= 1.0)
            difB = 1.0;
         else if (difB <= 0.0)
            difB = 0.0;

         if (difG >= 1.0)
            difG = 1.0;
         else if (difG <= 0.0)
            difG = 0.0;

         bulbsCurRGB[i][0] = difR;
         bulbsCurRGB[i][1] = difG;
         bulbsCurRGB[i][2] = difB;
      }
   }
}

void establishContact() {
   while (Serial.available() <= 0) {
      Serial.println("Waiting for connection...");
      delay(2500);
   }
}
