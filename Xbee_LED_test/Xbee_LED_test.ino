#include <FastSPI_LED.h>

#define NUM_LEDS 150
#define PIN 4

struct CRGB { unsigned char b; unsigned char r; unsigned char g; };
struct CRGB *leds;

int pos;
char input;
long hexValue;

void setup()
{
  FastSPI_LED.setLeds(NUM_LEDS);
  FastSPI_LED.setChipset(CFastSPI_LED::SPI_TM1809);
  FastSPI_LED.setPin(PIN);
  FastSPI_LED.init();
  FastSPI_LED.start();
  leds = (struct CRGB*)FastSPI_LED.getRGBData(); 
  
  Serial.begin(9600);

  pos = 20;
  hexValue = 0;
}

void setColor(int ledNum, long hexValue) {
  leds[ledNum].r = (hexValue >> 16) & 0xff;
  leds[ledNum].g = (hexValue >> 8) & 0xff;
  leds[ledNum].b = hexValue & 0xff;
}

void loop() { 
  if(Serial.available()) {
    input = Serial.read();
    
    if(input >= '0' && input <= '9') {
      Serial.print(input);
      hexValue |= ((long)(input - '0') << pos);
      pos -= 4;
    }
    else if(input >= 'a' && input <= 'f') {
      Serial.print(input);
      hexValue |= ((long)(input - 'a' + 10) << pos);
      pos -= 4;
    }
    else if(input >= 'A' && input <= 'F') {
      Serial.print(input);
      hexValue |= ((long)(input - 'A' + 10) << pos);
      pos -= 4;
    }
    
    if(pos == -4) {
      Serial.print("\nDisplaying color = 0x");
      Serial.print(hexValue, HEX);
      Serial.print("\n");
      // flash LEDs
      for(int i=0; i<50; i++) {
        for(pos=0; pos<NUM_LEDS; pos++) {
          if(random() & 0x7)
            setColor(pos, 0);
          else
            setColor(pos, hexValue);
        }
        FastSPI_LED.show();
      }
      delay(100);
      memset(leds, 0, NUM_LEDS*3);
      FastSPI_LED.show();
      
      pos = 20;
      hexValue = 0;
    }
  }
}

