#ifndef UMA1014_h
#define UMA1014_h

#include "Arduino.h"

class UMA1014 {
public:
    UMA1014(uint8_t address);
    
    /* sleeps and calls wire.h init, if you are using wire.h already you probably won't have to call it */
    init();

    /* figures out the best divider combination */
    void setPLLfreq(uint32_t vcoFreq, uint32_t referenceFreq);

    struct state {
        bool disableAlarm = true;
        
        /* register A*/
        bool powerDown = false;
        bool chargePumpCurrent = false; // false -> 0.5mA, true -> 1mA
        uint8_t referenceRatio = 0b1110; // refer to documentation for details

        /* register B */
        bool phaseInverter = false;
        bool vcoA = true;
        bool vcoB = false;

        uint32_t mainDividerRatio = 80000;
    } state;

    /* sends the current state to the PLL chip */
    writeState();

    struct status {
        bool disableAlarm;
        bool latchedPowerDip;
        bool latchedOutOfLock;
        bool momentaryOutOfLock;
    };

    /* returns 0 on success */
    int getStatusRegister(struct UMA1014::status *currentState);

private:
    uint8_t _address;
};

#endif
