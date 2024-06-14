/*
  An example analogue clock using a TFT LCD screen to show the time
  use of some of the drawing commands with the library.

  For a more accurate clock, it would be better to use the RTClib library.
  But this is just a demo.

  Uses fonts 1, 2, 4, 6 depending on radius of clock face

  Realistic radius is 45(cramped) to ???
  Largest screen tested was 800 x 480 8bit parallel interface and looked
  good at a range of radius from 60 --> 240.

  Based on a sketch by Gilchrist 6/2/2014 1.0, and modified by Bodmer's
  TFT_eSPI clock example.
  Modified into a "Scalable" Widget by Bionicbone 13 June 2024.

  Make sure all the display driver and pin connections are correct by
  editing the User_Setup.h file in the TFT_eSPI library folder.

  #########################################################################
  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
  #########################################################################

  Requires TFT_eSPI library here:
  https://github.com/Bodmer/TFT_eSPI
  
  Requires widget library here:
  https://github.com/Bodmer/TFT_eWidget

  Requires Time library by Paul Stoffregen - Install by Arduino Library Manager
  NTP code by Paul Stoffregen
*/

const char ssid[] = "***********";       //  your network SSID (name)
const char pass[] = "***********";       // your network password


#include <WiFi.h>
#include <WiFiUdp.h>
#include <LittleFS.h>
#include <SPIFFS.h>
#include <vfs_api.h>
#include <FSImpl.h>
#include <FS.h>
#include <SPI.h>
#include <TFT_eSPI.h>     // Hardware-specific library
#include <TFT_eWidget.h>  // Widget library


TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

ClockWidget   clk1 = ClockWidget(&tft);
ClockWidget   clk2 = ClockWidget(&tft);
ClockWidget   clk3 = ClockWidget(&tft);
ClockWidget   clk4 = ClockWidget(&tft);
ClockWidget   clk5 = ClockWidget(&tft);
ClockWidget   clk6 = ClockWidget(&tft);

#include <TimeLib.h>

// NTP Servers:
static const char ntpServerName[] = "us.pool.ntp.org";
//static const char ntpServerName[] = "time.nist.gov";
//static const char ntpServerName[] = "time-a.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-b.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-c.timefreq.bldrdoc.gov";

const int timeZone = 1;     // Central European Time
//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
//const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

time_t getNtpTime();
void sendNTPpacket(IPAddress& address);


void setup(void) {
  // Set up WiFi to get Internet Time
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Udp.begin(localPort);
  setSyncProvider(getNtpTime);
  setSyncInterval(300);

  // Set up the TFT screen (SSD1963 controller)
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_GREY);

  // Draw the city labels and clocks
  tft.setTextColor(TFT_GREEN);
  tft.drawCentreString("LONDON", 400, 10, 4);
  clk1.newClock(400, 260, 220, TFT_GREEN, TFT_BLACK, TFT_WHITE, TFT_RED);
  clk1.drawClockFace();

  tft.setTextColor(TFT_BLUE);
  tft.drawCentreString("NY City, USA", 100, 10, 4);
  clk2.newClock(100, 120, 80, TFT_BLUE, TFT_BLACK, TFT_YELLOW, TFT_GREY); 
  clk2.drawClockFace();

  tft.setTextColor(TFT_MAGENTA);
  tft.drawCentreString("PARIS", 700, 10, 4);
  clk3.newClock(700, 120, 80, TFT_MAGENTA, TFT_BLACK, TFT_GOLD, TFT_YELLOW);
  clk3.drawClockFace();

  tft.setTextColor(TFT_BLACK);
  tft.drawCentreString("TOKYO", 100, 285, 4);
  clk4.newClock(100, 390, 80, TFT_BLACK, TFT_WHITE, TFT_BLACK, TFT_RED);
  clk4.drawClockFace();

  tft.setTextColor(TFT_BLACK);
  tft.drawCentreString("Hong Kong / Singapore", 720, 210, 2);
  clk5.newClock(720, 290, 60, TFT_YELLOW, TFT_BLUE, TFT_YELLOW, TFT_RED);
  clk5.drawClockFace();

  tft.setTextColor(TFT_BLACK);
  tft.drawCentreString("Sydney,", 735, 370, 2);
  tft.drawCentreString("Australia", 750, 385, 2);
  clk6.newClock(670, 420, 50, TFT_RED, TFT_YELLOW, TFT_RED, TFT_BLACK);
  clk6.drawClockFace();
}
  
time_t prevDisplay = 0; // when the digital clock was displayed

void loop() {
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      clk1.updateClock();       // LONDON
      clk2.updateClock(-5);     // NY City
      clk3.updateClock(1);      // PARIS
      clk4.updateClock(8);      // TOKYO
      clk5.updateClock(7);      // HONG KONG / SINGAPORE
      clk6.updateClock(9);      // Sydney, Australia
    }
  }
}


/*-------- NTP code by Paul Stoffregen ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0); // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 = (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress& address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
