#include "UMA1014.h"
#include "Wire.h"

UMA1014::UMA1014(uint8_t address) {
    _address = address;
}

UMA1014::init() {
    Wire.begin();
    delay(3000); // the chip may need a little time to start up properly
}

static const int refDividers[16] = {128, 160, 192, 240, 256, 320, 384, 480, 512, 640, 768, 960, 1024, 1280, 1536, 1920};

void UMA1014::setPLLfreq(uint32_t vcoFreq, uint32_t refereceFreq) {
    uint8_t bestRefDivider = 1;
    uint32_t bestMainDivider = 1;
    uint32_t bestError = 0xFFFFFFFF;

    for(int i = 0; i < 16; i++) {
        uint32_t channelStep = refereceFreq / refDividers[i];
        uint32_t mainDivider = vcoFreq / channelStep;
        
        uint32_t resultingFreq = mainDivider * channelStep;
        uint32_t error = 0;
        if(resultingFreq > vcoFreq) {
            error = resultingFreq - vcoFreq;
        } else {
            error = vcoFreq - resultingFreq;
        }

        if(error < bestError) {
            bestRefDivider = i;
            bestMainDivider = mainDivider;
            bestError = error;
        }
    }

    state.mainDividerRatio = bestMainDivider;
    state.referenceRatio = bestRefDivider;
    writeState();
}

UMA1014::writeState() {
    /* subaddress */
    uint8_t subaddress = 0;
    subaddress |= (state.disableAlarm & 0x1) << 4;
    subaddress |= (0x1 << 3); // Auto increment

    /* Register A */
    uint8_t registerA = 0;
    registerA |= (state.powerDown & 0x1) << 7;
    registerA |= (state.chargePumpCurrent & 0x1) << 5;
    registerA |= (state.referenceRatio & 0xF) << 0;

    /* Register B */
    uint8_t registerB = 0xA0;
    registerB |= (state.vcoA & 0x1) << 2;
    registerB |= (state.vcoB & 0x1) << 3;
    registerB |= (state.phaseInverter & 0x1) << 4;
    registerB |= (state.mainDividerRatio >> 16) & 0x3;
    
    /* Register C */
    uint8_t registerC = (state.mainDividerRatio >> 8) & 0xFF;

    /* Register D */
    uint8_t registerD = (state.mainDividerRatio >> 0) & 0xFF;

    Wire.beginTransmission(_address);
    Wire.write(subaddress);
    Wire.write(registerA);
    Wire.write(registerB);
    Wire.write(registerC);
    Wire.write(registerD);
    Wire.endTransmission();
}

int UMA1014::getStatusRegister(struct UMA1014::status *currentState) {
    Wire.requestFrom(_address, (uint8_t)1);
    if(!Wire.available()) {
        return 1;
    }
    
    uint8_t reg = 0;
    while(Wire.available()) {
        reg = Wire.read();
    }

    currentState->disableAlarm = (reg >> 0) & 0x1;
    currentState->latchedPowerDip = (reg >> 1) & 0x1;
    currentState->latchedOutOfLock = (reg >> 2) & 0x1;
    currentState->momentaryOutOfLock = (reg >> 4) & 0x1;
    
    return 0;
}
