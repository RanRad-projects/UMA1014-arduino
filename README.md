# UMA1014-arduino
Arduino library for interfacing with the Philips UMA1014 PLL chip

Thanks to [FelixTRG](https://github.com/OK9UWU) for testing!


## Basic usage

    #include <UMA1014.h>

    UMA1014 pllchip(0x62);

    void setup() {
     Serial.begin(9600); 
     pllchip.init();
     pllchip.state.phaseInverter = true;
     pllchip.writeState();
    
     // manually set the dividers
     pllchip.state.referenceRatio = 0b1100;
     pllchip.state.mainDividerRatio = 7368;
     pllchip.writeState();
    
     // or simply
     pllchip.setPLLfreq(92100000, 12800000);
    }

    void loop() {
      struct UMA1014::status currentStatus;
      if(pllchip.getStatusRegister(&currentStatus) == 0) {
        Serial.println("UMA1014 state:");
        if(currentStatus.disableAlarm) {
          Serial.println("    -> alarm on");
        }
        if(currentStatus.latchedPowerDip) {
          Serial.println("    -> latched power dip");
        }
        if(currentStatus.latchedOutOfLock) {
          Serial.println("    -> latched out of lock");
        }
        if(currentStatus.momentaryOutOfLock) {
          Serial.println("    -> momentary out of lock");
        }
      }
      delay(1000);
    }
