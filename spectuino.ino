#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <Adafruit_AS7341.h>

  #define TFT_CS 10
  #define TFT_RST 12
  #define TFT_DC 7

  Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
  Adafruit_AS7341 as7341;
  uint16_t colors[12] = {0x701D,0x281F,0x06BF,0x1FE0,0,0,0xB7E0,0xFEE0,0xFA60,0xF800,0x6000,0x8410};
  char *labels[] = {"415","445","480","515","","","555","590","630","680","910","CLR"};

void setup() {
  tft.init(240, 240);
  tft.fillScreen(ST77XX_BLACK);
  as7341.begin();
  as7341.setATIME(100);
  as7341.setASTEP(999);
  as7341.setGain(AS7341_GAIN_256X);
  as7341.enableLED(false);

  as7341.enableSpectralMeasurement(false);
  as7341.setLowThreshold(250);
  as7341.setHighThreshold(10000);
  as7341.setAPERS(AS7341_INT_COUNT_ALL);
  as7341.setSpectralThresholdChannel(AS7341_ADC_CHANNEL_4);
  as7341.enableSpectralInterrupt(true);
  as7341.enableSpectralMeasurement(true);

}

void loop() {
  uint16_t readings[12];
  float counts[12];
  as7341.readAllChannels(readings);

  for(uint8_t i = 0; i < 12; i++) {
    if(i == 4 || i == 5) continue;
    counts[i] = as7341.toBasicCounts(readings[i]);
  }

  uint8_t row = 0; 
  for(uint8_t i = 0; i < 11; i++) {
    if(i == 4 || i == 5) continue;
    drawBar(labels[i],row*21,counts[i],colors[i],counts[11]);
    row++;
  }



  drawStatus();
}

void drawBar(char *text, byte y, float val, uint16_t color, float clearLevel) {
  byte textWidth = 40;
  byte barHeight = 20;
  byte barWidth = (byte)(val * 200);
  byte clrWidth = (byte)(clearLevel * 200);
  tft.setCursor(0, y);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(false);
  tft.print(text);
  tft.fillRect(textWidth+barWidth, y, 239-barWidth-textWidth, barHeight, ST77XX_BLACK);
  tft.fillRect(textWidth, y, barWidth, barHeight, color);
  for(uint8_t i= y; i<y+barHeight; i=i+3) {
    tft.drawPixel(textWidth+clrWidth, i, ST77XX_WHITE);
  }
  
  tft.fillRect(208, y+4, 30, 11, ST77XX_BLACK);
  tft.setCursor(210, y+6);
  tft.setTextSize(1);
  tft.print(val);
}

void drawStatus() {
  uint16_t flicker_freq = 0;
  flicker_freq = as7341.detectFlickerHz();
  tft.fillRect(0, 220, 239, 239, ST77XX_BLACK);
  tft.setCursor(0, 220);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  if (flicker_freq == 0) tft.print("N/F");
  else if (flicker_freq == 1) tft.print("U/F");
  else { tft.print(flicker_freq); tft.print(" Hz"); };

//  uint8_t int_status = as7341.getInterruptStatus();
 // if (!(int_status & 0x80)) return;

  uint8_t int_source = as7341.spectralInterruptSource();
  if (int_source & 0x10) {
    tft.print("LO");
  }
  if (int_source & 0x20) {
    tft.print("HI");
  }

  as7341.clearInterruptStatus();
}
