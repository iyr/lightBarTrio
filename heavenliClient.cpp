#include "Arduino.h"
#include "heavenliClient.h"
#include "heavenliLamp.h"
//#include <EEPROM.h>

heavenliClient::heavenliClient() {
   this->isConnected       = false; 
   this->synackReceived    = false;
   this->outBufferFull     = false;
   this->__CID_sent        = false;
   this->client_addressed  = false;
   this->tma               = 0;
   this->tmb               = 0;
   this->runtimeCounter1   = millis();
   this->timeoutCounter    = millis();
   this->updateTimer       = millis();
   this->CHB_timer         = millis();

   // Determine if device has an ID set (cannot be 0, 255, or FF)
   this->id[0] = random(1, 254);
   this->id[1] = random(1, 254);
   //this->id[0] = EEPROM.read(this->IDaddress+0);
   //this->id[1] = EEPROM.read(this->IDaddress+1);
}

// Initialize client with exactly one lamp
void heavenliClient::init() {
   this->numLamps = 1;
   this->lamp.init();
   return;
}

// Initialize client with more than one lamp
void heavenliClient::init(hliLamp* lamps, uint8_t numLamps) {
   this->numLamps = numLamps;
   for (int i = 0; i < numLamps; i++){
      lamps[i].init();
   }
   return;
}

void heavenliClient::update()
{
   if (millis() - this->updateTimer > 16) {
      this->lamp.update(2.72/100.0);
      this->updateTimer = millis();
   }

   if (millis() - this->CHB_timer > 100) {
      this->__CHB = true;
      this->CHB_timer = millis();
   }

   if ((millis() - this->timeoutCounter) > 1500) {
      //this->isConnected = false;
      this->synackReceived = false;
      this->__CHB = true;
      this->__CID_sent = false;
      this->timeoutCounter = millis();
   }
   return;
}

/*
 * Update Lamp state, keep tab on connection to host
 */
void heavenliClient::update(hliLamp* lamp) {
   this->lamp.update(2.72);
   if ((millis() - this->timeoutCounter) > 1500) {
      //this->isConnected = false;
      this->synackReceived = false;
      this->__CID_sent = false;
      this->timeoutCounter = millis();
   }
   return;
}

/*
 * Update Lamp states, keep tab on connection to host
 */
void heavenliClient::update(hliLamp* lamps, uint8_t numLamps)
{
   if ((millis() - this->timeoutCounter) > 15) {
      //this->isConnected = false;
      this->synackReceived = false;
      this->__CID_sent = false;
      this->timeoutCounter = millis();
   }
   return;
}

size_t heavenliClient::writeACK(uint8_t* buffer) {
   size_t paramBytes = 0;
   buffer[paramBytes] = 'A'; paramBytes++;
   buffer[paramBytes] = 'C'; paramBytes++;
   buffer[paramBytes] = 'K'; paramBytes++;
   buffer[paramBytes] = 0; paramBytes++;
   return paramBytes;
}

size_t heavenliClient::writeSYN(uint8_t* buffer) {
   size_t paramBytes = 0;
   buffer[paramBytes] = 'S'; paramBytes++;
   buffer[paramBytes] = 'Y'; paramBytes++;
   buffer[paramBytes] = 'N'; paramBytes++;
   buffer[paramBytes] = 0; paramBytes++;
   return paramBytes;
}

size_t heavenliClient::writeCID(uint8_t* buffer) {
   size_t paramBytes = 0;
   buffer[paramBytes] = 'C'; paramBytes++;
   buffer[paramBytes] = 'I'; paramBytes++;
   buffer[paramBytes] = 'D'; paramBytes++;
   buffer[paramBytes] = '!'; paramBytes++;
   buffer[paramBytes] = this->id[0]; paramBytes++;
   buffer[paramBytes] = this->id[1]; paramBytes++;
   buffer[paramBytes] = 0; paramBytes++;
   return paramBytes;
}

size_t heavenliClient::writeCNL(uint8_t* buffer) {
   size_t paramBytes = 0;
   buffer[paramBytes] = 'C'; paramBytes++;
   buffer[paramBytes] = 'I'; paramBytes++;
   buffer[paramBytes] = 'D'; paramBytes++;
   buffer[paramBytes] = ':'; paramBytes++;
   buffer[paramBytes] = this->id[0]; paramBytes++;
   buffer[paramBytes] = this->id[1]; paramBytes++;
   buffer[paramBytes] = 'C'; paramBytes++;
   buffer[paramBytes] = 'N'; paramBytes++;
   buffer[paramBytes] = 'L'; paramBytes++;
   buffer[paramBytes] = '!'; paramBytes++;
   buffer[paramBytes] = this->numLamps; paramBytes++;
   return paramBytes;
}

size_t heavenliClient::writeLPR(uint8_t* buffer) {
   size_t paramBytes = 0;
   uint8_t tmid[2];
   this->lamp.getID(tmid);

   buffer[paramBytes] = 'C'; paramBytes++;
   buffer[paramBytes] = 'I'; paramBytes++;
   buffer[paramBytes] = 'D'; paramBytes++;
   buffer[paramBytes] = ':'; paramBytes++;
   buffer[paramBytes] = this->id[0]; paramBytes++;
   buffer[paramBytes] = this->id[1]; paramBytes++;

   buffer[paramBytes] = 'L'; paramBytes++;
   buffer[paramBytes] = 'I'; paramBytes++;
   buffer[paramBytes] = 'D'; paramBytes++;
   buffer[paramBytes] = ':'; paramBytes++;
   buffer[paramBytes] = tmid[0]; paramBytes++;
   buffer[paramBytes] = tmid[1]; paramBytes++;
            
   buffer[paramBytes] = 'N'; paramBytes++;
   buffer[paramBytes] = 'B'; paramBytes++;
   buffer[paramBytes] = '!'; paramBytes++;
   buffer[paramBytes] = this->lamp.getNumBulbs(); paramBytes++;

   buffer[paramBytes] = 'C'; paramBytes++;
   buffer[paramBytes] = 'M'; paramBytes++;
   buffer[paramBytes] = '!'; paramBytes++;
   buffer[paramBytes] = this->lamp.getBulbCountMutability(); paramBytes++;

   buffer[paramBytes] = 'A'; paramBytes++;
   buffer[paramBytes] = 'R'; paramBytes++;
   buffer[paramBytes] = '!'; paramBytes++;
   buffer[paramBytes] = this->lamp.getArrangement(); paramBytes++;

   buffer[paramBytes] = 'A'; paramBytes++;
   buffer[paramBytes] = 'O'; paramBytes++;
   buffer[paramBytes] = '!'; paramBytes++;

   float tmf = this->lamp.getAngularOffset();
   uint8_t tmbb[4];
   memcpy(&tmbb, &tmf, sizeof(float));

   buffer[paramBytes] = tmbb[0]; paramBytes++;
   buffer[paramBytes] = tmbb[1]; paramBytes++;
   buffer[paramBytes] = tmbb[2]; paramBytes++;
   buffer[paramBytes] = tmbb[3]; paramBytes++;
   /*
   */

   /*
   uint32_t f0, f1, f2, f3;
   float tmf = this->lamp.getAngularOffset();
   uint8_t tmbb = 0;

   f0 = (11111111000000000000000000000000 & tmf) >> 24;
   f1 = (00000000111111110000000000000000 & tmf) >> 16;
   f2 = (00000000000000001111111100000000 & tmf) >> 8;
   f3 = (00000000000000000000000011111111 & tmf);

   tmbb = f0;
   buffer[paramBytes] = tmbb; paramBytes++;
   tmbb = f1;
   buffer[paramBytes] = tmbb; paramBytes++;
   tmbb = f2;
   buffer[paramBytes] = tmbb; paramBytes++;
   tmbb = f3;
   buffer[paramBytes] = tmbb; paramBytes++;
   */

   buffer[paramBytes] = 'L'; paramBytes++;
   buffer[paramBytes] = 'L'; paramBytes++;
   buffer[paramBytes] = '!'; paramBytes++;
   buffer[paramBytes] = this->lamp.getMetaLampLevel(); paramBytes++;

   buffer[paramBytes] = 'S'; paramBytes++;
   buffer[paramBytes] = 'B'; paramBytes++;
   buffer[paramBytes] = '!'; paramBytes++;
   buffer[paramBytes] = this->lamp.getMasterSwitchBehavior(); paramBytes++;

   //buffer[paramBytes] = 'V'; paramBytes++;
   //buffer[paramBytes] = 'Q'; paramBytes++;
   //buffer[paramBytes] = '!'; paramBytes++;

   return paramBytes;
}

/*
 * Prepares Packet to be sent to host
 */
size_t heavenliClient::outPacket(uint8_t* buffer) {

   if (millis() - this->runtimeCounter1 > 500) {

      // Establish connection before we can send data
      if (this->isConnected == false) {
         if (this->synackReceived == true) {
            this->isConnected = true;
            this->runtimeCounter1 = millis();
            return this->writeACK(buffer);
         } else {
            this->runtimeCounter1 = millis();
            return this->writeSYN(buffer);
         }
      } 
      else // Respond to Data Requests
      {

         // Plugin has requested the ID of the Client Device (HOST: getClientID)
         if (  this->__CID_requested   == true  && 
               this->isConnected       == true  ){

            this->__CID_requested = false;
            return this->writeCID(buffer);
         }

         // Plugin has requested the Number of lamps on the Client Device (HOST: requestNumLamps)
         if (  this->__CNL_requested   == true  && 
               this->isConnected       == true  &&
               this->client_addressed  == true  ){

            this->__CNL_requested = false;
            return this->writeCNL(buffer);
         }

         // Plugin has requested all base parameters of a lamp
         if (  this->__ALL_requested   == true  && 
               this->client_addressed  == true  &&
               this->isConnected       == true  ){

            this->__ALL_requested = false;
            return this->writeLPR(buffer);

         }

         // Update plugin
         if (  this->isConnected       == true  &&
               this->__CHB             == true  &&
               this->client_addressed  == true  ){

            this->__CHB = false;
            return this->writeLPR(buffer);
         }
      }

      this->runtimeCounter1 = millis();
      this->client_addressed = false;
   }

   //return numBytes;
   return 0;
}


/*
 * Unpack/Parse/Process data received from HeavenLi host over Serial
 */
void heavenliClient::processPacket(const uint8_t* buffer, size_t size) {

   // Before we can start sending or receiving data
   char tmp[] = "SYNACK";
   char tmb[size];
   for (int i = 0; i < size; i++) {
      tmb[i] = buffer[i];
   }

   if (strcmp(tmp, tmb) == 0) {
      this->synackReceived = true;
      this->connectionEstablished = true;
   } else 
   if (this->connectionEstablished == true) {

      // Read buffer for paramters
      for (int i = 0; i < size; i++) {

         if (i+5 <= size) {

            // Host has requested the Client ID
            if (  buffer[i+0] == 'C'   && 
                  buffer[i+1] == 'I'   &&
                  buffer[i+2] == 'D'   &&
                  buffer[i+3] == '?'   ){
               this->__CID_requested = true;
            }

            // Host is assigning Client ID
            if (  buffer[i+0] == 'C'   && 
                  buffer[i+1] == 'I'   &&
                  buffer[i+2] == 'D'   &&
                  buffer[i+3] == '!'   ){
               uint8_t tmp[2];
               tmp[0] = buffer[i+4];
               tmp[1] = buffer[i+5];
               this->setID(tmp);
               this->__CID_requested = false;
            }

            // Host is addressing this client
            if (  buffer[i+0] == 'C'   &&
                  buffer[i+1] == 'I'   &&
                  buffer[i+2] == 'D'   &&
                  buffer[i+3] == ':'   &&
                  buffer[i+4] == this->id[0] &&
                  buffer[i+5] == this->id[1] ){
               this->tma = buffer[i+4];
               this->tmb = buffer[i+5];
               this->client_addressed = true;
            }

            // Host has requested the Client Number of Lamps
            if (  buffer[i+0] == 'C'   && 
                  buffer[i+1] == 'N'   &&
                  buffer[i+2] == 'L'   &&
                  buffer[i+3] == '?'   &&
                  this->client_addressed == true){
               this->__CNL_requested = true;
            }

            // Host has requested lamp parameters
            if (  buffer[i+0] == 'P'   &&
                  buffer[i+1] == 'A'   &&
                  buffer[i+2] == 'R'   &&
                  buffer[i+3] == '?'   ){
               this->__ALL_requested = true;
            }

            if (  buffer[i+0] == 'L'   &&
                  buffer[i+1] == 'I'   &&
                  buffer[i+2] == 'D'   &&
                  buffer[i+3] == ':'   ){

               uint8_t index = i+4;

               // Host has requested parameters for the client's sole lamp
               if (  buffer[index+0]   == 255   &&
                     buffer[index+1]   == 255   &&
                     this->numLamps    == 1     ){
                  this->__ALL_requested = true;
               } else {

                  // Host is addressing client's sole lamp (defensive sanity check)
                  /*
                  uint8_t* tmlid;
                  this->lamp.getID(tmlid);
                  if (  buffer[index+0]   == tmlid[0] &&
                        buffer[index+1]   == tmlid[1] &&
                        this->numLamps    == 1        ){
                     this->__ALL_requested = false;
                     this->selectedLamp = 0;
                     this->lamp.setAddressed(true);
                  }
                  */
               }
            }
            /*
            // Host has requested lamp parameters
            if (  buffer[i+0] == 'L'   &&
                  buffer[i+1] == 'I'   &&
                  buffer[i+2] == 'D'   &&
                  buffer[i+3] == ':'   &&
                  buffer[i+4] == 255   &&
                  buffer[i+5] == 255   ){
               this->__ALL_requested = true;
            }

            // Host has requested lamp parameters
            uint8_t* tmlid;
            this->lamp->getID(tmlid);
            if (  buffer[i+0] == 'L'   &&
                  buffer[i+1] == 'I'   &&
                  buffer[i+2] == 'D'   &&
                  buffer[i+3] == ':'   &&
                  buffer[i+4] == tmlid[0]   &&
                  buffer[i+5] == tmlid[1]   ){
               this->__lamp_addressed = true;
            }
            */

            // Host is setting lamp bulbs target colors
            if (  this->client_addressed == true &&
                  buffer[i+0] == 'B'   &&
                  buffer[i+1] == 'T'   &&
                  buffer[i+2] == 'C'   &&
                  buffer[i+3] == '!'   ){
               this->__ALL_requested = true;
               /*
               uint8_t tmc[3];
               tmc[0] = buffer[i+5];
               tmc[1] = buffer[i+6];
               tmc[2] = buffer[i+7];
               this->lamp.setBulbTargetRGB(buffer[i+4], tmc);
               */
               //if (this->lamp.isAddressed()) {
               if (this->useHostColors) {
                  uint8_t tmc[3];
                  for (int j = 0; j < 10; j++) {
                     tmc[0] = buffer[j*3 + i + 4];
                     tmc[1] = buffer[j*3 + i + 5];
                     tmc[2] = buffer[j*3 + i + 6];
                     this->lamp.setBulbTargetRGB(j, tmc);
                  }
               }
            }
         }
      }
   }

   this->timeoutCounter = millis();
   return;
}

// Performs necessary three-way handshake with client device
bool heavenliClient::establishConnection() {
   return false;
}

// IMPLEMENTED THIS FOR FUTURE USE NOW PLS
/*
int heavenliClient::getID() {
   return this->id;
}
*/

void heavenliClient::setID(uint8_t* newID) {
   // Ensure ID is not 0, 255, or FF
   if (  (newID[0] == 0 && newID[1] == 0)    ||
         (newID[0] == 255 && newID[1] == 255)){
      return;
   } else {
      this->id[0] = newID[0];
      this->id[1] = newID[1];
      //EEPROM.update(this->IDaddress+0, this->id[0]);
      //EEPROM.update(this->IDaddress+1, this->id[1]);
      return;
   }
}
