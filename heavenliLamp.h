#ifndef heavenliLamp
#define heavenliLamp

#include "Arduino.h"
/*
 * heavenli lampClass
 */
class hliLamp
{
   public:
      hliLamp();
      hliLamp(long extraEntropy);
      //hliLamp(uint8_t* alias, size_t numChars);
      //hliLamp(uint8_t* alias, size_t numChars, long extraEntropy);

      // Getter/Setter for whether lamp has beend addressed by ID
      bool     setAddressed(bool add);
      bool     isAddressed();

      // Getter/Setter for lamp's 16-Character "knick-name"
      //void     getAlias(uint8_t*& knickname);
      //void     setAlias(uint8_t* newKnickname);

      // Getter/Setter for lamp's angular offset for adjusting heavenli rotation
      float    getAngularOffset();
      void     setAngularOffset(float newAO);
      void     setAngularOffset(int8_t newAO);

      // Getter/Setter for lamp's element arrangement (eg. circular, linear, etc)
      uint8_t  getArrangement();
      void     setArrangement(uint8_t newArn);

      // Getter/Setter for the current color of individual bulbs on lamp
      void     getBulbCurrentRGB(uint8_t bulb, uint8_t* RGB);
      void     setBulbCurrentRGB(uint8_t bulb, uint8_t* newRGB);
      // Get current colors for all bulbs on the lamp
      void     getBulbsCurrentRGB(uint8_t** RGB);
      // Set current colors of all bulbs on the lamp to ONE colors
      void     setBulbsCurrentRGB(uint8_t* newRGB);

      // Getter/Setter for the target color of individual bulbs on lamp
      void     getBulbTargetRGB(uint8_t bulb, uint8_t* RGB);
      void     setBulbTargetRGB(uint8_t bulb, uint8_t* newRGB);
      // Get target colors for all bulbs on the lamp
      void     getBulbsTargetRGB(uint8_t** RGB);
      // Set target colors of all bulbs on the lamp to ONE colors
      void     setBulbsTargetRGB(uint8_t* newRGB);

      // Getter/Setter the lamp's 2-byte ID
      //int      getID();
      void     getID(uint8_t* ID);
      void     setID(const uint8_t* newID);
      
      // Getter/Setter for how lamp responds to input from the master switch
      int8_t   getMasterSwitchBehavior();
      void     setMasterSwitchBehavior(int8_t newBehavior);

      // Getter/Setter for number of bulbs on the lamp
      void     setNumBulbs(uint8_t newNumBulbs);
      uint8_t  getNumBulbs();

      // Return whether or not the number of bulbs can be changed (for many-LED strips)
      bool     getBulbCountMutability();

      // Number of layers of sub-lamps of lamp
      uint8_t  getMetaLampLevel();

      // If bulb-count mutability is true, list valid quantities for the lamp
      void     getValidBulbQuantities(uint8_t* quantities);

      // Eh
      void     init();

      // Update bulb colors to animate transition from the current color to the target color
      // using the frametime to adjust the rate of change
      void     update(float frameTime);

   private:
      float       angularOffset = 0.0f;
      
      uint8_t     bulbsCurrentRGB[10][3];
      uint8_t     bulbsTargetRGB[10][3];
      uint8_t*    validBulbCounts;
      uint8_t     numBulbs = 1;
      uint8_t     arrangement = 1;
      uint8_t     metaLampLevel = 0;

      //uint8_t*       alias = "quack";
      uint8_t     id[2];
      int8_t      masterSwitchBehavior = -1;

      bool        mutableBulbCount = false;
      bool        lampAddressed = false;
};

#endif
