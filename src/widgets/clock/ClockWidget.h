
#ifndef _ClockWidget_h_
#define _ClockWidget_h_

#include "Arduino.h"
#include "TFT_eSPI.h"

// clock class
class ClockWidget
{
  public:
    ClockWidget(TFT_eSPI* tft);

    //// Set red, orange, yellow and green start+end zones as a percentage of full scale
    //void setZones(uint16_t rs, uint16_t re, uint16_t os, uint16_t oe, uint16_t ys, uint16_t ye, uint16_t gs, uint16_t ge);
    //// Draw meter meter at x, y and define full scale range plus the scale labels
    //void analogMeter(uint16_t x, uint16_t y, float fullScale, const char *units, const char *s0, const char *s1, const char *s2, const char *s3, const char *s4);
    //// Draw meter meter at x, y and define full scale range plus the scale labels
    //void analogMeter(uint16_t x, uint16_t y, float startScale, float endScale, const char *units, const char *s0, const char *s1, const char *s2, const char *s3, const char *s4);
    //// Move needle to new position
    //void updateNeedle(float value, uint32_t ms_delay);

    // Draw clock face at x, y, with radius, digit colour and face colour
    void drawClockFace(uint32_t xPosition, uint32_t yPosition, uint32_t radius, uint32_t digitColour, uint32_t faceColour)
    // Update the displayed time on the clock
    void updateClock(uint32_t xPosition, uint32_t yPosition, uint32_t radius, uint32_t hourMinColour, uint32_t secondsColour, uint32_t faceColour)

  private:
    // Pointer to TFT_eSPI class functions
    TFT_eSPI* ctft;

    //float ltx;         // x delta of needle start
    //uint16_t osx, osy; // Saved x & y coords of needle end
    //int old_analog;    // Value last displayed
    //int old_digital;   // Value last displayed

    float sx, sy, mx, my, hx, hy;           // computed hand x & y coords
    float sdeg, mdeg, hdeg;                 // degrees per movement
    uint16_t osx, osy, omx, omy, ohx, ohy;  // saved H, M, S x & y coords for erase
    uint16_t x0, x1, yy0, yy1;              // clock face computations
    uint32_t targetTime;                    // for next 1 second timeout
    uint8_t hh, mm, ss;                     // time in hours, mins, seconds
    bool initial;                           // initialisation flag
    
    // x, y coord for center of clock
    uint32_t xPosition;
    uint32_t yPosition;
    uint32_t radius;
    uint32_t digitColour;
    uint32_t faceColour;
    uint32_t hourMinColour;
    uint32_t secondsColour;

    //// x, y coord of top left corner of meter graphic
    //uint16_t mx;
    //uint16_t my;

    //// Scale factor
    //float factor;
    //float scaleStart;

    //// Scale label
    //char     mlabel[9];

    //// Scale values
    //char     ms0[5];
    //char     ms1[5];
    //char     ms2[5];
    //char     ms3[5];
    //char     ms4[5];

    //// Scale colour zone start end end values
    //int16_t redStart;
    //int16_t redEnd;
    //int16_t orangeStart;
    //int16_t orangeEnd;
    //int16_t yellowStart;
    //int16_t yellowEnd;
    //int16_t greenStart;
    //int16_t greenEnd;
};

#endif
