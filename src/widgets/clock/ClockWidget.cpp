 #include "Arduino.h"
 #include "ClockWidget.h"

// #########################################################################
// clockWidget constructor
// #########################################################################
 ClockWidget::ClockWidget(TFT_eSPI* tft)
 {
   sx = 0;
   sy = 0;
   mx = 0;
   my = 0;
   hx = 0;
   hy = 0;
   sdeg = 0;
   mdeg = 0;
   hdeg = 0;
   osx = 0;
   osy = 0;
   omx = 0;
   omy = 0;
   ohx = 0;
   ohy = 0;
   x0 = 0;
   x1 = 0;
   yy0 = 0;
   yy1 = 0;
   targetTime = 0;
   hh = 12;
   mm = 30;
   ss = 0;
   initial = 1;

   ctft = tft

  //ltx = 0;              // Saved x coord of bottom of needle
  //osx = 120, osy = 120; // Saved x & y coords
  //old_analog =  -999;   // Value last displayed
  //old_digital = -999;   // Value last displayed

  //mx = 0;
  //my = 0;

  //factor = 1.0;
  //scaleStart = 0.0;

  //mlabel[8] = '\0';

  //// Defaults
  //strncpy(ms0,   "0", 4);
  //strncpy(ms1,  "25", 4);
  //strncpy(ms2,  "50", 4);
  //strncpy(ms3,  "75", 4);
  //strncpy(ms4, "100", 4);

  //redStart = 0;
  //redEnd = 0;
  //orangeStart = 0;
  //orangeEnd = 0;
  //yellowStart = 0;
  //yellowEnd = 0;
  //greenStart = 0;
  //greenEnd = 0;

  //ntft = tft;
 }

 void ClockWidget::drawClockFace(uint32_t xPosition, uint32_t yPosition, uint32_t radius, uint32_t digitColour, uint32_t faceColour) {
   // Draw clock face
   tft.fillCircle(xPosition, yPosition, radius, digitColour); //120 = 118
   tft.fillCircle(xPosition, yPosition, radius - (radius * 6 / 100), faceColour); //120 = 110

   // Draw 12 lines
   for (int i = 0; i < 360; i += 30) {
     sx = cos((i - 90) * 0.0174532925);
     sy = sin((i - 90) * 0.0174532925);
     x0 = sx * (radius * 95 / 100) + xPosition;
     yy0 = sy * (radius * 95 / 100) + yPosition;
     x1 = sx * (radius * 83.33 / 100) + xPosition;
     yy1 = sy * (radius * 83.33 / 100) + yPosition;

     tft.drawLine(x0, yy0, x1, yy1, digitColour);
   }

   // Draw 60 dots
   for (int i = 0; i < 360; i += 6) {
     sx = cos((i - 90) * 0.0174532925);
     sy = sin((i - 90) * 0.0174532925);
     x0 = sx * (radius * 85 / 100) + xPosition;
     yy0 = sy * (radius * 85 / 100) + yPosition;

     // Draw minute markers
     tft.drawPixel(x0, yy0, digitColour);

     // Draw main quadrant dots
     if (i == 0 || i == 180) tft.fillCircle(x0, yy0, 2, digitColour);
     if (i == 90 || i == 270) tft.fillCircle(x0, yy0, 2, digitColour);
   }

   // Text offset adjustment
   uint32_t dialOffset = radius - (radius * 25 / 100);
   uint8_t digitOffset = 0;
   // Draw digits around clock perimeter
   float xp = 0.0, yp = 0.0; // Use float pixel position for smooth AA motion
   uint8_t font, singleDigitOffset, doubleDigitOffset, yPixelOffset;
   tft.setTextColor(digitColour, faceColour);  // Adding a background colour erases previous text automatically
   for (uint32_t h = 1; h <= 12; h++) {
     float sx1 = cos(((h * 360.0 / 12) - 90) * 0.0174532925);
     float sy1 = sin(((h * 360.0 / 12) - 90) * 0.0174532925);
     xp = sx1 * dialOffset + xPosition;
     yp = sy1 * dialOffset + yPosition;

     /*
     digitOffset and yPixelOffset for fonts:
     Font 1 = 2 : 5  & yp - 3
     Font 2 = 3 : 6  & yp - 7
     Font 3 = N/A
     Font 4 = 6 : 13 & yp - 10
     Font 5 = N/A
     Font 6 = 12 : 26 & yp - 18
     */

     if (radius >= 320) {
       font = 6; singleDigitOffset = 12; doubleDigitOffset = 26; yPixelOffset = 18;
     }
     else if (radius < 69) {
       font = 1; singleDigitOffset = 2; doubleDigitOffset = 5; yPixelOffset = 3;
     }
     else if (radius < 159) {
       font = 2; singleDigitOffset = 3; doubleDigitOffset = 6; yPixelOffset = 7;
     }
     else if (radius < 320) {
       font = 4; singleDigitOffset = 6; doubleDigitOffset = 13; yPixelOffset = 12;
     }
     digitOffset = (h < 10) ? singleDigitOffset : doubleDigitOffset;
     tft.drawNumber(h, xp - digitOffset, yp - yPixelOffset, font);
   }
   targetTime = millis() + 1000;
 }


 void ClockWidget::updateClock(uint32_t xPosition, uint32_t yPosition, uint32_t radius, uint32_t hourMinColour, uint32_t secondsColour, uint32_t faceColour) {
   // Pre-compute hand degrees, x & y coords for a fast screen update
   sdeg = ss * 6;                  // 0-59 -> 0-354
   mdeg = mm * 6 + sdeg * 0.01666667;  // 0-59 -> 0-360 - includes seconds
   hdeg = hh * 30 + mdeg * 0.0833333;  // 0-11 -> 0-360 - includes minutes and seconds
   hx = cos((hdeg - 90) * 0.0174532925);
   hy = sin((hdeg - 90) * 0.0174532925);
   mx = cos((mdeg - 90) * 0.0174532925);
   my = sin((mdeg - 90) * 0.0174532925);
   sx = cos((sdeg - 90) * 0.0174532925);
   sy = sin((sdeg - 90) * 0.0174532925);

   if (initial) { osx = xPosition; osy = yPosition; omx = xPosition; omy = yPosition; ohx = xPosition; ohy = yPosition; }
   if (ss == 0 || initial) {
     initial = 0;
     // Erase hour and minute hand positions every minute
     tft.drawLine(ohx, ohy, xPosition, yPosition, faceColour);
     ohx = hx * (radius * 45 / 100) + xPosition;
     ohy = hy * (radius * 45 / 100) + yPosition;
     tft.drawLine(omx, omy, xPosition, yPosition, faceColour);
     omx = mx * (radius * 68 / 100) + xPosition;
     omy = my * (radius * 68 / 100) + yPosition;
   }

   // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
   tft.drawLine(osx, osy, xPosition, yPosition, faceColour);
   osx = sx * (radius * 69 / 100) + xPosition;
   osy = sy * (radius * 69 / 100) + yPosition;
   tft.drawLine(osx, osy, xPosition, yPosition, secondsColour);
   tft.drawLine(ohx, ohy, xPosition, yPosition, hourMinColour);
   tft.drawLine(omx, omy, xPosition, yPosition, hourMinColour);
   tft.drawLine(osx, osy, xPosition, yPosition, secondsColour);

   tft.fillCircle(xPosition, yPosition, 3, secondsColour);
 }


//// #########################################################################
//// Draw meter meter at x, y and define full scale range & the scale labels
//// #########################################################################
//void MeterWidget::analogMeter(uint16_t x, uint16_t y, float fullScale, const char *units, const char *s0, const char *s1, const char *s2, const char *s3, const char *s4)
//{
//  analogMeter(x, y, 0.0, fullScale, units, s0, s1, s2, s3, s4);
//}
//
//void MeterWidget::analogMeter(uint16_t x, uint16_t y, float startScale, float endScale, const char *units, const char *s0, const char *s1, const char *s2, const char *s3, const char *s4)
//{
//  // Save offsets for needle plotting
//  mx = x;
//  my = y;
//  factor = 100.0/(endScale - startScale);
//  scaleStart = startScale;
//
//  strncpy(mlabel, units, 8);
//
//  strncpy(ms0, s0, 4);
//  strncpy(ms1, s1, 4);
//  strncpy(ms2, s2, 4);
//  strncpy(ms3, s3, 4);
//  strncpy(ms4, s4, 4);
//
//  // Meter outline
//  ntft->fillRect(x, y, 239, 126, TFT_GREY);
//  ntft->fillRect(x + 5, y + 3, 230, 119, TFT_WHITE);
//
//  ntft->setTextColor(TFT_BLACK);  // Text colour
//
//  // Draw ticks every 5 degrees from -50 to +50 degrees (100 deg. FSD swing)
//  for (int i = -50; i < 51; i += 5) {
//    // Long scale tick length
//    int tl = 15;
//
//    // Coordinates of tick to draw
//    float sx = cos((i - 90) * 0.0174532925);
//    float sy = sin((i - 90) * 0.0174532925);
//    uint16_t x0 = x + sx * (100 + tl) + 120;
//    uint16_t y0 = y + sy * (100 + tl) + 140;
//    uint16_t x1 = x + sx * 100 + 120;
//    uint16_t y1 = y + sy * 100 + 140;
//
//    // Coordinates of next tick for zone fill
//    float sx2 = cos((i + 5 - 90) * 0.0174532925);
//    float sy2 = sin((i + 5 - 90) * 0.0174532925);
//    int x2 = x + sx2 * (100 + tl) + 120;
//    int y2 = y + sy2 * (100 + tl) + 140;
//    int x3 = x + sx2 * 100 + 120;
//    int y3 = y + sy2 * 100 + 140;
//
//    // Red zone limits
//    if (redEnd > redStart) {
//      if (i >= redStart && i < redEnd) {
//        ntft->fillTriangle(x0, y0, x1, y1, x2, y2, TFT_RED);
//        ntft->fillTriangle(x1, y1, x2, y2, x3, y3, TFT_RED);
//      }
//    }
//
//    // Orange zone limits
//    if (orangeEnd > orangeStart) {
//      if (i >= orangeStart && i < orangeEnd) {
//        ntft->fillTriangle(x0, y0, x1, y1, x2, y2, TFT_ORANGE);
//        ntft->fillTriangle(x1, y1, x2, y2, x3, y3, TFT_ORANGE);
//      }
//    }
//
//    // Yellow zone limits
//    if (yellowEnd > yellowStart) {
//      if (i >= yellowStart && i < yellowEnd) {
//        ntft->fillTriangle(x0, y0, x1, y1, x2, y2, TFT_YELLOW);
//        ntft->fillTriangle(x1, y1, x2, y2, x3, y3, TFT_YELLOW);
//      }
//    }
//
//    // Green zone limits
//    if (greenEnd > greenStart) {
//      if (i >= greenStart && i < greenEnd) {
//        ntft->fillTriangle(x0, y0, x1, y1, x2, y2, TFT_GREEN);
//        ntft->fillTriangle(x1, y1, x2, y2, x3, y3, TFT_GREEN);
//      }
//    }
//
//    // Short scale tick length
//    if (i % 25 != 0) tl = 8;
//
//    // Recalculate coords in case tick length changed
//    x0 = x + sx * (100 + tl) + 120;
//    y0 = y + sy * (100 + tl) + 140;
//    x1 = x + sx * 100 + 120;
//    y1 = y + sy * 100 + 140;
//
//    // Draw tick
//    ntft->drawLine(x0, y0, x1, y1, TFT_BLACK);
//
//    // Check if labels should be drawn, with position tweaks
//    if (i % 25 == 0) {
//      // Calculate label positions
//      x0 = x + sx * (100 + tl + 10) + 120;
//      y0 = y + sy * (100 + tl + 10) + 140;
//      switch (i / 25) {
//        case -2: ntft->drawCentreString(ms0, x0, y0 - 12, 2); break;
//        case -1: ntft->drawCentreString(ms1, x0, y0 - 9, 2); break;
//        case  0: ntft->drawCentreString(ms2, x0, y0 - 6, 2); break;
//        case  1: ntft->drawCentreString(ms3, x0, y0 - 9, 2); break;
//        case  2: ntft->drawCentreString(ms4, x0, y0 - 12, 2); break;
//      }
//    }
//
//    // Now draw the arc of the scale
//    sx = cos((i + 5 - 90) * 0.0174532925);
//    sy = sin((i + 5 - 90) * 0.0174532925);
//    x0 = x + sx * 100 + 120;
//    y0 = y + sy * 100 + 140;
//    // Draw scale arc, don't draw the last part
//    if (i < 50) ntft->drawLine(x0, y0, x1, y1, TFT_BLACK);
//  }
//
//  ntft->drawString(mlabel, x + 5 + 230 - 40, y + 119 - 20, 2); // Units at bottom right
//  ntft->drawCentreString(mlabel, x + 120, y + 70, 4);          // Comment out to avoid font 4
//  ntft->drawRect(x + 5, y + 3, 230, 119, TFT_BLACK);           // Draw bezel line
//  
//  updateNeedle(0, 0);
//}
//    
//// #########################################################################
//// Update needle position
//// This function is blocking while needle moves, time depends on ms_delay
//// 10ms minimises needle flicker if text is drawn within needle sweep area
//// Smaller values OK if text not in sweep area, zero for instant movement but
//// does not look realistic... (note: 100 increments for full scale deflection)
//// #########################################################################
//void MeterWidget::updateNeedle(float val, uint32_t ms_delay)
//{
//  int value = (val - scaleStart) * factor;
//
//  ntft->setTextColor(TFT_BLACK, TFT_WHITE);
//  char buf[8]; dtostrf(val, 5, 1, buf);
//  
//  ntft->drawRightString(buf, mx + 50, my + 119 - 20, 2);
//
//  if (value < -10) value = -10; // Limit value to emulate needle end stops
//  if (value > 110) value = 110;
//
//  // Move the needle until new value reached
//  while (value != old_analog) {
//    if (old_analog < value) old_analog++;
//    else old_analog--;
//
//    if (ms_delay == 0) old_analog = value; // Update immediately id delay is 0
//
//    float sdeg = map(old_analog, -10, 110, -150, -30); // Map value to angle
//    // Calculate tip of needle coords
//    float sx = cos(sdeg * 0.0174532925);
//    float sy = sin(sdeg * 0.0174532925);
//
//    // Calculate x delta of needle start (does not start at pivot point)
//    float tx = tan((sdeg + 90) * 0.0174532925);
//
//    // Erase old needle image
//    ntft->drawLine(mx + 120 + 20 * ltx - 1, my + 140 - 20, mx + osx - 1, my + osy, TFT_WHITE);
//    ntft->drawLine(mx + 120 + 20 * ltx, my + 140 - 20, mx + osx, my + osy, TFT_WHITE);
//    ntft->drawLine(mx + 120 + 20 * ltx + 1, my + 140 - 20, mx + osx + 1, my + osy, TFT_WHITE);
//
//    // Re-plot text under needle
//    ntft->setTextColor(TFT_BLACK);
//    ntft->drawCentreString(mlabel, mx + 120, my + 70, 4); // // Comment out to avoid font 4
//
//    // Store new needle end coords for next erase
//    ltx = tx;
//    osx = sx * 98 + 120;
//    osy = sy * 98 + 140;
//
//    // Draw the needle in the new position, magenta makes needle a bit bolder
//    // draws 3 lines to thicken needle
//    ntft->drawLine(mx + 120 + 20 * ltx - 1, my + 140 - 20, mx + osx - 1, my + osy, TFT_RED);
//    ntft->drawLine(mx + 120 + 20 * ltx, my + 140 - 20, mx + osx, my + osy, TFT_MAGENTA);
//    ntft->drawLine(mx + 120 + 20 * ltx + 1, my + 140 - 20, mx + osx + 1, my + osy, TFT_RED);
//
//    // Slow needle down slightly as it approaches new position
//    if (abs(old_analog - value) < 10) ms_delay += ms_delay / 5;
//
//    // Wait before next update
//    delay(ms_delay);
//  }
//}
//
//// #########################################################################
//// Set red, orange, yellow and green start+end zones as a % of full scale
//// #########################################################################
//void MeterWidget::setZones(uint16_t rs, uint16_t re, uint16_t os, uint16_t oe, uint16_t ys, uint16_t ye, uint16_t gs, uint16_t ge)
//{
//  // Meter scale is -50 to +50
//  redStart = rs - 50;
//  redEnd = re - 50;
//  orangeStart = os - 50;
//  orangeEnd = oe - 50;
//  yellowStart = ys - 50;
//  yellowEnd = ye - 50;
//  greenStart = gs - 50;
//  greenEnd = ge - 50;
//}