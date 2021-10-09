#ifndef CAPAT_H_INCLUDED
#define CAPAT_H_INCLUDED

#include "Arduino.h"

#if defined(__AVR__)
#define PIN_TO_BASEREG(pin)             (portInputRegister(digitalPinToPort(pin)))
#define PIN_TO_BITMASk(pin)             (digitalPinToBitMask(pin))
#define IO_REG_TYPE uint8_t
#define DIRECT_READ(base, mask)         (((*(base)) & (mask)) ? 1 : 0)
#define DIRECT_MODE_INPUT(base, mask)   ((*((base)+1)) &= ~(mask), (*((base)+2)) &= ~(mask))
#define DIRECT_MODE_OUTPUT(base, mask)  ((*((base)+1)) |= (mask))
#define DIRECT_WRITE_LOW(base, mask)    ((*((base)+2)) &= ~(mask))
#define DIRECT_WRITE_HIGH(base, mask)   ((*((base)+2)) |= (mask))
#endif

#define MAXKEY 2                // greater or equal to cntNb !!
#define NBCNTMEM 0x0fff         // 16     // puissance de 2 !!
#define CALIB 16                // NBCNTMEM  // nbre de count() en calibration
#define DEBOUNCE 60
#define CHGSTROBE 0xffff        // millis() strobe mean's update to protect from near finger

class Capat{
    public:
        Capat();
        void init(uint8_t samples,uint8_t common,uint8_t* touch,uint8_t cntNb);
        void calibrate();
        void count();
        void capaKeysCheck();
        uint8_t capaKeysGet(uint8_t keyNum);

        uint16_t highTresh[MAXKEY];                 // key high treshold
        uint16_t lowTresh[MAXKEY];                  // key low treshold
        uint16_t cntk[MAXKEY];                      // key cnt
        uint16_t meank[MAXKEY];                     // off key mean cnt value
        uint32_t totalMem[MAXKEY];                  // key total cnt value
        bool     keyVal[MAXKEY];                    // off/on
        bool     keyChge[MAXKEY];                   // last update key change
        uint32_t keyChSt[MAXKEY];                   // key change time

    private:
        void     meanUpdate();

        uint8_t  samples;                           // samples nbr to perform
        uint8_t  keyNb;                             // nbr of active keys
        //uint16_t cntMem[NBCNTMEM*MAXKEY];           // all keys cnt memorys for mean calculation
        
        uint8_t  prevCntMem[MAXKEY];                // key prev ptr in cntMem
        uint32_t debounce[MAXKEY];                  // key last millis
        
        IO_REG_TYPE	sBit;
        volatile IO_REG_TYPE *sReg; 

        IO_REG_TYPE	rBit[MAXKEY];
        volatile IO_REG_TYPE* rReg[MAXKEY];
        
};

#endif // DS18X20_H_INCLUDED


