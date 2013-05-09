#include <FastSPI_LED.h>

#define NUM_LEDS 75
#define LED_PIN 4

// colors
#define RED    0xff0000
#define ORANGE 0xff7f00
#define YELLOW 0xffff00
#define GREEN  0x00ff00
#define BLUE   0x0000ff
#define INDIGO 0x4b00ff
#define VIOLET 0x8f00ff

// Sometimes chipsets wire in a backwards sort of way
struct CRGB { unsigned char b; unsigned char r; unsigned char g; };
//struct CRGB { unsigned char r; unsigned char g; unsigned char b; };
struct CRGB *leds;

void setup()
{
  FastSPI_LED.setLeds(NUM_LEDS);
  //FastSPI_LED.setChipset(CFastSPI_LED::SPI_SM16716);
  FastSPI_LED.setChipset(CFastSPI_LED::SPI_TM1809);
  //FastSPI_LED.setChipset(CFastSPI_LED::SPI_LPD6803);
  //FastSPI_LED.setChipset(CFastSPI_LED::SPI_HL1606);
  //FastSPI_LED.setChipset(CFastSPI_LED::SPI_595);
  //FastSPI_LED.setChipset(CFastSPI_LED::SPI_WS2801);

  FastSPI_LED.setPin(LED_PIN);
  
  FastSPI_LED.init();
  FastSPI_LED.start();

  leds = (struct CRGB*)FastSPI_LED.getRGBData(); 
}

void setColor(int ledNum, long hexValue) {
  leds[ledNum].r = (hexValue >> 16) & 0xff;
  leds[ledNum].g = (hexValue >> 8) & 0xff;
  leds[ledNum].b = hexValue & 0xff;
}

void loop() {
  // travelling rainbow
  int h,i,j,k;
  for(k=0; k<4; k++) {
    for(j=0; j<=NUM_LEDS-(7<<k); j++) {
      memset(leds, 0, NUM_LEDS * 3);
      
      i = j;
      for(h=0; h<(1<<k); h++, i++)
        setColor(i, RED);
      for(h=0; h<(1<<k); h++, i++)
        setColor(i, ORANGE);
      for(h=0; h<(1<<k); h++, i++)
        setColor(i, YELLOW);
      for(h=0; h<(1<<k); h++, i++)
        setColor(i, GREEN);
      for(h=0; h<(1<<k); h++, i++)
        setColor(i, BLUE);
      for(h=0; h<(1<<k); h++, i++)
        setColor(i, INDIGO);
      for(h=0; h<(1<<k); h++, i++)
        setColor(i, VIOLET);
        
      FastSPI_LED.show();
      delay((k+1)*10);
    }
    for(j=NUM_LEDS-(7<<k); j>=0; j--) {
      memset(leds, 0, NUM_LEDS * 3);
      
      i = j;
      for(h=0; h<(1<<k); h++, i++)
        setColor(i, RED);
      for(h=0; h<(1<<k); h++, i++)
        setColor(i, ORANGE);
      for(h=0; h<(1<<k); h++, i++)
        setColor(i, YELLOW);
      for(h=0; h<(1<<k); h++, i++)
        setColor(i, GREEN);
      for(h=0; h<(1<<k); h++, i++)
        setColor(i, BLUE);
      for(h=0; h<(1<<k); h++, i++)
        setColor(i, INDIGO);
      for(h=0; h<(1<<k); h++, i++)
        setColor(i, VIOLET);
        
      FastSPI_LED.show();
      delay((k+1)*10);
    }
  }

/*
  // one at a time
  for(int j = 0; j < 3; j++) { 
    for(int i = 0 ; i < NUM_LEDS; i++ ) {
      memset(leds, 0, NUM_LEDS * 3);
      switch(j) { 
        case 0: leds[i].r = 255; break;
        case 1: leds[i].g = 255; break;
        case 2: leds[i].b = 255; break;
      }
      FastSPI_LED.show();
      delay(10);
    }
  }

  // growing/receeding bars
  for(int j = 0; j < 3; j++) { 
    memset(leds, 0, NUM_LEDS * 3);
    for(int i = 0 ; i < NUM_LEDS; i++ ) {
      switch(j) { 
        case 0: leds[i].r = 255; break;
        case 1: leds[i].g = 255; break;
        case 2: leds[i].b = 255; break;
      }
      FastSPI_LED.show();
      delay(10);
    }
    for(int i = NUM_LEDS-1 ; i >= 0; i-- ) {
      switch(j) { 
        case 0: leds[i].r = 0; break;
        case 1: leds[i].g = 0; break;
        case 2: leds[i].b = 0; break;
      }
      FastSPI_LED.show();
      delay(1);
    }
  }
  
  // Fade in/fade out
  for(int j = 0; j < 3; j++ ) { 
    memset(leds, 0, NUM_LEDS * 3);
    for(int k = 0; k < 256; k++) { 
      for(int i = 0; i < NUM_LEDS; i++ ) {
        switch(j) { 
          case 0: leds[i].r = k; break;
          case 1: leds[i].g = k; break;
          case 2: leds[i].b = k; break;
        }
      }
      FastSPI_LED.show();
      delay(3);
    }
    for(int k = 255; k >= 0; k--) { 
      for(int i = 0; i < NUM_LEDS; i++ ) {
        switch(j) { 
          case 0: leds[i].r = k; break;
          case 1: leds[i].g = k; break;
          case 2: leds[i].b = k; break;
        }
      }
      FastSPI_LED.show();
      delay(3);
    }
  }
*/
}

