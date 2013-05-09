#include <FastSPI_LED.h>

#define NUM_LEDS 150
#define LED_PIN 4

// colors
#define OFF    0x000000
#define RED    0xff0000
#define ORANGE 0xff7f00
#define YELLOW 0xffff00
#define GREEN  0x00ff00
#define BLUE   0x0000ff
#define INDIGO 0x4b00ff
#define VIOLET 0x8f00ff

// array of colors to be displayed by LED strip
struct CRGB { unsigned char b; unsigned char r; unsigned char g; };
struct CRGB * leds;


// this function is executed once upon power on or reset
void setup()
{
  FastSPI_LED.setLeds(NUM_LEDS);
  FastSPI_LED.setChipset(CFastSPI_LED::SPI_TM1809);
  FastSPI_LED.setPin(LED_PIN);
  FastSPI_LED.init();
  FastSPI_LED.start();
  leds = (struct CRGB*) FastSPI_LED.getRGBData(); 
  
  //pinMode(3, INPUT);
  //pinMode(5, OUTPUT);
  //pinMode(6, OUTPUT);
  //pinMode(7, OUTPUT);
  
  delay(100); // do not delete  
}


// this function is executed repeatedly after setup() is executed
void loop() {

  int i;

  ////////////////////////////////////////////////////////////////
  // Set the 6th LED to orange for 1 second, then turn it off for
  // 1 second
  ////////////////////////////////////////////////////////////////

  // set LED 5 (the 6th LED) to orange
  setColor(5, ORANGE);

  // display all the set colors
  FastSPI_LED.show();

  // wait 1000 milliseconds
  delay(1000);

  // turn off all LEDs
  turnOffAllLeds();

  // display all the set colors
  FastSPI_LED.show();

  // wait 1000 milliseconds
  delay(1000);
  
  ////////////////////////////////////////////////////////////////
  // Display a single blue LED travelling from the start to the end
  // of the LED strip, followed by a blue LED with half the brightness
  // travelling back to the start of the strip.
  ////////////////////////////////////////////////////////////////

  // travelling from start to end (LED 0 through LED 149)
  for(i=0; i<NUM_LEDS; i++) {
    setColor(i, BLUE);
    FastSPI_LED.show();
    delay(50);
    setColor(i, 0);
  }

  // travelling from end to start (LED 148 through LED 0)
  for(i=NUM_LEDS-2; i>=0; i--) {
    setRGBColor(i, 0.0, 0.0, 0.5);
    FastSPI_LED.show();
    delay(50);
    setColor(i, 0);
  }

  FastSPI_LED.show();
   
}


///////////////////////////////////////////////////////////////////////////


void turnOffAllLeds() {
  memset(leds, 0, NUM_LEDS * 3);
}


// set the color of a single LED by specifying a 24-bit hex value (RRGGBB)
void setColor(int ledNum, long hexValue) {
  if(ledNum >= 0 && ledNum < NUM_LEDS) {
    leds[ledNum].r = (hexValue >> 16) & 0xff;
    leds[ledNum].g = (hexValue >> 8) & 0xff;
    leds[ledNum].b = hexValue & 0xff;
  }
}


// set the color of a single LED by specifying the R, G, B components
// separately using a value between 0 and 1
void setRGBColor(int ledNum, float red, float green, float blue) {
  if(ledNum >= 0 && ledNum < NUM_LEDS) {
    leds[ledNum].r = ((int)(red   * 255.0)) & 0xff;
    leds[ledNum].g = ((int)(green * 255.0)) & 0xff;
    leds[ledNum].b = ((int)(blue  * 255.0)) & 0xff;
  }
}


void blinkLED(int pin) {
    digitalWrite(pin, HIGH);
    delay(500);
    digitalWrite(pin, LOW);
    delay(500);
}


///////////////////////////////////////////////////////////////////////////


void drawFlash(int duration) {
  for(int i=0; i<NUM_LEDS; i++) {
    if(random() & 0x1)
      setColor(i, random() & 0xffffff);
    else
      setColor(i, 0);
  }
  FastSPI_LED.show();
  
  delay(duration);
  
  memset(leds, 0, NUM_LEDS * 3);
  FastSPI_LED.show();
  
  delay(duration);
}  


void drawTravellingRainbow(int width, int backward) {
  int h, i, j;

  for(j=0; j<=NUM_LEDS-(7<<width); j++) {
    memset(leds, 0, NUM_LEDS * 3);
    
    i = backward ? (NUM_LEDS - (7 << width) - j) : j;
    
    for(h=0; h<(1<<width); h++, i++)
      setColor(i, RED);
    for(h=0; h<(1<<width); h++, i++)
      setColor(i, ORANGE);
    for(h=0; h<(1<<width); h++, i++)
      setColor(i, YELLOW);
    for(h=0; h<(1<<width); h++, i++)
      setColor(i, GREEN);
    for(h=0; h<(1<<width); h++, i++)
      setColor(i, BLUE);
    for(h=0; h<(1<<width); h++, i++)
      setColor(i, INDIGO);
    for(h=0; h<(1<<width); h++, i++)
      setColor(i, VIOLET);
    
    FastSPI_LED.show(); 
      
    if(backward)
      delay(10);
    else
      delay((width + 1) * 10);
  }
}

