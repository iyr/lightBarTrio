#include "Arduino.h"
#include "heavenliLamp.h"

/*
 * Implements a heavenli lamp
 */
hliLamp::hliLamp() {
   this->numBulbs = 1;
   //this->isMetaLamp = false;
   this->bulbsTargetRGB[10][3];
   this->bulbsCurrentRGB[10][3];
   //this->alias[16] = 'demo';
   int extraEntropy = 0;
   int seed = 0;
   for (int i = 0; i < 9; i++)
      seed ^= __TIME__[i]^__DATE__[i];

   randomSeed(seed^extraEntropy);
   this->id[0] = random(1, 254);
   this->id[1] = random(1, 254);
   uint8_t RGB[3] = {128, 0, 77};
   this->setBulbsTargetRGB(RGB);
   RGB[0]=0; RGB[1]=200; RGB[2]=0;
   this->setBulbsCurrentRGB(RGB);
}

/*
 * Constructor overload for adding extra RNG entropy
 */
hliLamp::hliLamp(long extraEntropy) {
   this->numBulbs = 1;
   //this->isMetaLamp = false;
   this->bulbsTargetRGB[10][3];
   this->bulbsCurrentRGB[10][3];
   //this->alias[16] = 'demo';
   long EE = extraEntropy;
   long seed = 0;
   for (int i = 0; i < 9; i++)
      seed ^= __TIME__[i]^__DATE__[i];

   randomSeed(seed^extraEntropy);
   this->id[0] = random(1, 254);
   this->id[1] = random(1, 254);

   uint8_t RGB[3] = {128, 0, 77};
   this->setBulbsTargetRGB(RGB);
   RGB[0]=0; RGB[1]=0; RGB[2]=0;
   this->setBulbsCurrentRGB(RGB);
}

/*
 * Constructor overload for adding extra RNG entropy
 * and Setting Lamp alias (limited to 16 uint8_tacters)
 */
/*
hliLamp::hliLamp(uint8_t* alias, size_t numChars, long extraEntropy) {
   this->numBulbs = 1;
   //this->isMetaLamp = false;
   this->bulbsTargetRGB[10][3];
   this->bulbsCurrentRGB[10][3];
   for (int i = 0; i < 16; i++)
      if (i <= numChars)
         this->alias[i] = alias[i];
      else
         this->alias[i] = 0;

   // Generate a seed for the Random Number Generator
   // to produce unique-ish (not really, use extraEntropy) per-device IDs
   long seed = 0;
   for (int i = 0; i < 9; i++)
      seed ^= __TIME__[i]^__DATE__[i];

   // Allows tweaking/tuning of entropy for the RNG
   long EE = extraEntropy;
   randomSeed(seed^EE);
   this->id[0] = random(1, 254);
   this->id[1] = random(1, 254);

   uint8_t RGB[3] = {128, 0, 77};
   this->setBulbsTargetRGB(RGB);
   RGB[0]=0; RGB[1]=0; RGB[2]=0;
   this->setBulbsCurrentRGB(RGB);
}
*/

/*
 * Constructor overload for Setting Lamp alias (limited to 16 uint8_tacters)
 */
/*
hliLamp::hliLamp(uint8_t* alias, size_t numChars) {
   this->numBulbs = 1;
   //this->isMetaLamp = false;
   this->bulbsTargetRGB[10][3];
   this->bulbsCurrentRGB[10][3];
   for (int i = 0; i < 16; i++)
      if (i <= numChars)
         this->alias[i] = alias[i];
      else
         this->alias[i] = 0;

   // Generate a seed for the Random Number Generator
   // to produce unique-ish (not really, use extraEntropy) per-device IDs
   long seed = 0;
   for (int i = 0; i < 9; i++)
      seed ^= __TIME__[i]^__DATE__[i];

   // Allows tweaking/tuning of entropy for the RNG
   long EE = 0;
   randomSeed(seed^EE);
   this->id[0] = random(1, 254);
   this->id[1] = random(1, 254);

   // Set initial colors
   uint8_t RGB[3] = {128, 0, 77};
   this->setBulbsTargetRGB(RGB);
   RGB[0]=0; RGB[1]=0; RGB[2]=0;
   this->setBulbsCurrentRGB(RGB);
}
*/

// Eh
void hliLamp::init() {
   return;
}

// Getter/Setter for whether lamp has beend addressed by ID
bool hliLamp::setAddressed(bool add){
   this->lampAddressed = add;
   return this->lampAddressed;
}

bool hliLamp::isAddressed(){
   return this->lampAddressed;
}

/*
void hliLamp::getAlias(uint8_t*& knickname) {
   knickname = new uint8_t[16];
   for (uint8_t i = 0; i < 16; i++) {
      knickname[i] = this->alias[i];
   }
   return;
}

void hliLamp::setAlias(uint8_t* newKnickname) {
   this->alias = new uint8_t[16];
   for (uint8_t i = 0; i < 16; i++) {
      this->alias[i] = newKnickname[i];
   }
   return;
}
*/

float hliLamp::getAngularOffset() {
   return this->angularOffset;
}

void hliLamp::setAngularOffset(float newAO) {
   this->angularOffset = newAO;
   return;
}

void hliLamp::setAngularOffset(int8_t newAO) {
   this->angularOffset = float(newAO);
   return;
}

uint8_t  hliLamp::getArrangement() {
   return this->arrangement;
}

void hliLamp::setArrangement(uint8_t newArn) {
   this->arrangement = constrain(newArn, 0, 1);
   return;
}

void hliLamp::getBulbCurrentRGB(uint8_t bulb, uint8_t* RGB) {
   RGB[0] = this->bulbsCurrentRGB[bulb][0];
   RGB[1] = this->bulbsCurrentRGB[bulb][1];
   RGB[2] = this->bulbsCurrentRGB[bulb][2];
   return;
}

void hliLamp::setBulbCurrentRGB(uint8_t bulb, uint8_t* newRGB) {
   this->bulbsCurrentRGB[bulb][0] = newRGB[0];
   this->bulbsCurrentRGB[bulb][1] = newRGB[1];
   this->bulbsCurrentRGB[bulb][2] = newRGB[2];
   return;
}

void hliLamp::getBulbsCurrentRGB(uint8_t** RGB) {
   return;
}

void hliLamp::setBulbsCurrentRGB(uint8_t* newRGB) {
   uint8_t RGB[3] = {0, 0, 0};
   // Sanity Check to verrify bounds
   /*
   if (sizeof(RGB)/sizeof(*RGB) != sizeof(newRGB)/sizeof(*newRGB))
      return;
   else
   {
      for (int i = 0; i < 10; i++)
      {
         this->bulbsCurrentRGB[i][0] = newRGB[0];
         this->bulbsCurrentRGB[i][1] = newRGB[1];
         this->bulbsCurrentRGB[i][2] = newRGB[2];
      }
      return;
   }
   */
   for (int i = 0; i < 10; i++)
   {
      this->bulbsCurrentRGB[i][0] = newRGB[0];
      this->bulbsCurrentRGB[i][1] = newRGB[1];
      this->bulbsCurrentRGB[i][2] = newRGB[2];
   }
   return;
}

void hliLamp::getBulbTargetRGB(uint8_t bulb, uint8_t* RGB) {
   RGB[0] = this->bulbsTargetRGB[bulb][0];
   RGB[1] = this->bulbsTargetRGB[bulb][1];
   RGB[2] = this->bulbsTargetRGB[bulb][2];
   return;
}

void hliLamp::setBulbTargetRGB(uint8_t bulb, uint8_t* newRGB) {
   this->bulbsTargetRGB[bulb][0] = newRGB[0];
   this->bulbsTargetRGB[bulb][1] = newRGB[1];
   this->bulbsTargetRGB[bulb][2] = newRGB[2];
   return;
}

void hliLamp::getBulbsTargetRGB(uint8_t** RGB) {
   return;
}

void hliLamp::setBulbsTargetRGB(uint8_t* newRGB) {
   uint8_t RGB[3] = {0, 0, 0};
   // Sanity Check
   /*
   if (sizeof(RGB)/sizeof(*RGB) != sizeof(newRGB)/sizeof(*newRGB))
      return;
   else
   {
      for (int i = 0; i < 10; i++)
      {
         this->bulbsTargetRGB[i][0] = newRGB[0];
         this->bulbsTargetRGB[i][1] = newRGB[1];
         this->bulbsTargetRGB[i][2] = newRGB[2];
      }
      return;
   }
   */
   for (int i = 0; i < 10; i++)
   {
      this->bulbsTargetRGB[i][0] = newRGB[0];
      this->bulbsTargetRGB[i][1] = newRGB[1];
      this->bulbsTargetRGB[i][2] = newRGB[2];
   }
   return;
}

//int hliLamp::getID() {
   //return this->id;
//}

void hliLamp::getID(uint8_t* ID) {
   ID[0] = this->id[0];
   ID[1] = this->id[1];
   return;
}

void hliLamp::setID(const uint8_t* newID) {
   this->id[0] = newID[0];
   this->id[1] = newID[1];
   return;
}
      
int8_t hliLamp::getMasterSwitchBehavior() {
   return this->masterSwitchBehavior;
}

void hliLamp::setMasterSwitchBehavior(int8_t newBehavior) {
   this->masterSwitchBehavior = newBehavior;
   return;
}

uint8_t hliLamp::getNumBulbs() {
   return this->numBulbs;
}

void hliLamp::setNumBulbs(uint8_t newNumBulbs) {
   if (newNumBulbs > 6)
      this->numBulbs = 6;
   else if (newNumBulbs < 1)
      this->numBulbs = 1;
   else
      this->numBulbs = newNumBulbs;
   return;
}

bool hliLamp::getBulbCountMutability() {
   return this->mutableBulbCount;
}

uint8_t hliLamp::getMetaLampLevel() {
   return this->metaLampLevel;
}

void hliLamp::getValidBulbQuantities(uint8_t* quantities) {
   //quantities = new uint8_t[10];
   for (int i = 0; i < 10; i++) {
      quantities[i] = this->validBulbCounts[i];
   }
   return;
}

void hliLamp::update(float frameTime) {
   float curR, curG, curB;
   float tarR, tarG, tarB;
   float difR, difG, difB;
   float rd = 0, gd = 0, bd = 0;
   uint8_t fR, fG, fB;
   float delta = frameTime;
   float threshold = 0.05;
   float tmf;
   for (int i = 0; i < 10; i++) {
      curR = float(this->bulbsCurrentRGB[i][0])/255.0;
      curG = float(this->bulbsCurrentRGB[i][1])/255.0;
      curB = float(this->bulbsCurrentRGB[i][2])/255.0;
  
      tarR = float(this->bulbsTargetRGB[i][0])/255.0;
      tarG = float(this->bulbsTargetRGB[i][1])/255.0;
      tarB = float(this->bulbsTargetRGB[i][2])/255.0;

      if (  curR != tarR  ||
            curG != tarG  ||
            curB != tarB  ){
         difR = abs(curR - tarR);
         difG = abs(curG - tarG);
         difB = abs(curB - tarB);
         rd = 0.0;
         gd = 0.0;
         bd = 0.0;
         delta = frameTime;
         tmf = float((i+2)*2)/float(this->numBulbs*3);
         delta *= tmf;

         if (difR > threshold)
            if (tarR > curR)
               rd = delta;
            else
               rd = -delta;

         if (difG > threshold)
            if (tarG > curG)
               gd = delta;
            else
               gd = -delta;

         if (difB > threshold)
            if (tarB > curB)
               bd = delta;
            else
               bd = -delta;

         if (difR > threshold)
            difR = curR + rd;
         else
            difR = tarR;

         if (difG > threshold)
            difG = curG + gd;
         else
            difG = tarG;

         if (difB > threshold)
            difB = curB + bd;
         else
            difB = tarB;

         fR = constrain(int(difR*255.0), 0, 255);
         fG = constrain(int(difG*255.0), 0, 255);
         fB = constrain(int(difB*255.0), 0, 255);

         this->bulbsCurrentRGB[i][0] = fR;
         this->bulbsCurrentRGB[i][1] = fG;
         this->bulbsCurrentRGB[i][2] = fB;
      }
   }
   return;
}
