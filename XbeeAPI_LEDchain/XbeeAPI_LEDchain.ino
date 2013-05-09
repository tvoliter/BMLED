#include <FastSPI_LED.h>
#include "XBee.h"

//#define LEADER // comment out for follower

#define NUM_LEDS 75
#define LED_PIN 4

// xbee addresses
#define LEADER_ADDR_HI 0x13a200
#define LEADER_ADDR_LO 0x4093d477
#define FOLLOWER_ADDR_HI 0x13a200
#define FOLLOWER_ADDR_LO 0x4093d4b5

// message definitions
#define MSG_SIZE 4

#define MSG_START    0xff
#define MSG_FORWARD  0x10
#define MSG_BACKWARD 0x11
#define MSG_TWINKLE  0x12

// colors
#define RED    0xff0000
#define ORANGE 0xff7f00
#define YELLOW 0xffff00
#define GREEN  0x00ff00
#define BLUE   0x0000ff
#define INDIGO 0x4b00ff
#define VIOLET 0x8f00ff

struct CRGB { unsigned char b; unsigned char r; unsigned char g; };
struct CRGB * leds;

XBee xbee;
XBeeAddress64 leaderAddr, followerAddr;
uint8_t txData[MSG_SIZE];
ZBTxRequest txPacket;
ZBTxStatusResponse txStatus;
ZBRxResponse rxPacket;

// variables used in loop() placed here so that they don't get stack-allocated each time loop() is called
uint8_t message[MSG_SIZE];
int i;
#ifdef LEADER
#else
int j, success;
#endif

void setup()
{
  FastSPI_LED.setLeds(NUM_LEDS);
  FastSPI_LED.setChipset(CFastSPI_LED::SPI_TM1809);
  FastSPI_LED.setPin(LED_PIN);
  FastSPI_LED.init();
  FastSPI_LED.start();
  leds = (struct CRGB*) FastSPI_LED.getRGBData(); 
  
  Serial.begin(9600);
  xbee = XBee();
  xbee.setSerial(Serial);

  leaderAddr = XBeeAddress64(LEADER_ADDR_HI, LEADER_ADDR_LO);
  followerAddr = XBeeAddress64(FOLLOWER_ADDR_HI, FOLLOWER_ADDR_LO);
#ifdef LEADER
  txPacket = ZBTxRequest(followerAddr, txData, sizeof(txData));
#else
  txPacket = ZBTxRequest(leaderAddr, txData, sizeof(txData));
#endif
  txStatus = ZBTxStatusResponse();
  rxPacket = ZBRxResponse();
  
  delay(100); // do not delete  
}


void loop() {

#ifdef LEADER
  
  for(i=0; i<3; i++) {
    // tell any follower that just joined to start (up to once per second approximately)
    message[0] = MSG_START;
    message[1] = i; // send width to follower
    sendPacketWithoutRetry(2, message);

    // draw forward rainbow
    drawTravellingRainbow(i, 0);
    
    // tell follower to draw forward rainbow
    message[0] = MSG_FORWARD;
    sendPacketWithTimeout(1, message, 1000);
    
    drawTravellingRainbowAtJoint(i, 0, 0);
    
    // wait for follower to finish drawing forward and backward rainbows
    message[0] = MSG_BACKWARD;
    receivePacketWithTimeout(followerAddr, 1, message, 3000);
  
    // draw backward rainbow
    drawTravellingRainbowAtJoint(i, 1, 0);
    drawTravellingRainbow(i, 1);
  }
    
  // generate random color to twinkle
  message[1] = random() & 0xff; // r
  message[2] = random() & 0xff; // g
  message[3] = random() & 0xff; // b
  // tell follower to twinkle with color rgb
  message[0] = MSG_TWINKLE;
  sendPacketWithTimeout(4, message, 1000);
  // twinkle with color rgb
  twinkle(100, ((long) message[1] << 16) | ((long) message[2] << 8) | (long) message[3]);
  
#else

  // wait for leader to say START
  message[0] = MSG_START;
  receivePacketWithoutTimeout(leaderAddr, 2, message);
  j = message[1]; // get width sent by leader

  // start display
  while(1) {
    for(i=j; i<3; i++) {
      // wait for leader to finish drawing forward rainbow
      message[0] = MSG_FORWARD;
      success = receivePacketWithTimeout(leaderAddr, 1, message, 3000);
      if(!success)
        break;
      
      // draw forward rainbow
      drawTravellingRainbowAtJoint(i, 0, 1);
      drawTravellingRainbow(i, 0);

      // draw backward rainbow
      drawTravellingRainbow(i, 1);
      
      // tell leader to draw backward rainbow
      message[0] = MSG_BACKWARD;
      success = sendPacketWithTimeout(1, message, 1000);
      if(!success)
        break;
      
      drawTravellingRainbowAtJoint(i, 1, 1);      
    }
    j = 0;

    // wait for leader to say TWINKLE
    message[0] = MSG_TWINKLE;
    success = receivePacketWithTimeout(leaderAddr, 4, message, 3000);
    if(!success)
      break;
    
    // twinkle with color rgb
    twinkle(100, ((long) message[1] << 16) | ((long) message[2] << 8) | (long) message[3]);
  }
 
#endif
   
}


//////////////////////////////////////////////////////////////////////////////


void blinkLED(int pin) {
    digitalWrite(pin, HIGH);
    delay(500);
    digitalWrite(pin, LOW);
    delay(500);
}


void setColor(int ledNum, long hexValue) {
  leds[ledNum].r = (hexValue >> 16) & 0xff;
  leds[ledNum].g = (hexValue >> 8) & 0xff;
  leds[ledNum].b = hexValue & 0xff;
}

#define GRID_WIDTH 8
#define GRID_HEIGHT 12
#define ARM_LENGTH 20
// row = 0 is top row, row = GRID_HEIGHT-1 is bottom row,
// col = 0 is nearest right arm, col = GRID_WIDTH-1 is nearest left arm;
// row = -1 means right arm (col = 0 is at right hand),
// row = GRID_HEIGHT means left arm (col = 0 is at left shoulder);
void setGridColor(int row, int col, long color) {
  int ledNum;
  
  // right arm
  if(row == -1)
    ledNum = col; // 0 <= col <= ARM_LENGTH-1
  // left arm
  else if(row == GRID_HEIGHT)
    ledNum = ARM_LENGTH + (GRID_WIDTH * GRID_HEIGHT) + col; // 0 <= col <= ARM_LENGTH-1
  // grid
  else {
    if(col & 0x1) // odd column
      ledNum = ARM_LENGTH + (col * GRID_HEIGHT) + (GRID_HEIGHT - 1 - row);
    else // even column
      ledNum = ARM_LENGTH + (col * GRID_HEIGHT) + row;
  }
  
  setColor(ledNum, color);
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

void drawTravellingRainbowAtJoint(int width, int backward, int frontedge) {
  int h, i, j;
  
  for(j=1; j<(7 << width); j++) {
    memset(leds, 0, NUM_LEDS * 3);
    
    if(frontedge)
      i = backward ? -j : (j - (7 << width));
    else
      i = backward ? (NUM_LEDS - j) : (NUM_LEDS - (7 << width) + j);
    
    for(h=0; h<(1<<width); h++, i++)
      if(i >= 0 && i < NUM_LEDS) setColor(i, RED);
    for(h=0; h<(1<<width); h++, i++)
      if(i >= 0 && i < NUM_LEDS) setColor(i, ORANGE);
    for(h=0; h<(1<<width); h++, i++)
      if(i >= 0 && i < NUM_LEDS) setColor(i, YELLOW);
    for(h=0; h<(1<<width); h++, i++)
      if(i >= 0 && i < NUM_LEDS) setColor(i, GREEN);
    for(h=0; h<(1<<width); h++, i++)
      if(i >= 0 && i < NUM_LEDS) setColor(i, BLUE);
    for(h=0; h<(1<<width); h++, i++)
      if(i >= 0 && i < NUM_LEDS) setColor(i, INDIGO);
    for(h=0; h<(1<<width); h++, i++)
      if(i >= 0 && i < NUM_LEDS) setColor(i, VIOLET);

    FastSPI_LED.show(); 
      
    if(backward)
      delay(10);
    else
      delay((width + 1) * 10);
  }
  
  if(frontedge && backward || !frontedge && !backward) {
    memset(leds, 0, NUM_LEDS * 3);
    FastSPI_LED.show();
  }    
}

void twinkle(int numFlashes, long color) {
  int i, j;
  
  for(i=0; i<numFlashes; i++) {
    for(j=0; j<NUM_LEDS; j++) {
      if(random() & 0x7)
        setColor(j, 0);
      else
        setColor(j, color);
    }
    FastSPI_LED.show();
  }
  memset(leds, 0, NUM_LEDS*3);
  FastSPI_LED.show();
}


void sendPacketWithoutRetry(uint8_t length, uint8_t * message) {
  for(int i=0; i<length; i++)
    txData[i] = message[i];

  xbee.send(txPacket);
  // get tx status packet 
  xbee.readPacket(10);
}

void sendPacketWithoutTimeout(uint8_t length, uint8_t * message) {
  int ready = 0;

  for(int i=0; i<length; i++)
    txData[i] = message[i];

  while(!ready) {
    xbee.send(txPacket);
 
    xbee.readPacket(500);
    if(xbee.getResponse().isAvailable()) {
      if(xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
        xbee.getResponse().getZBTxStatusResponse(txStatus);
        if(txStatus.getDeliveryStatus() == SUCCESS)
          ready = 1;
      }
    }
  }
}

int sendPacketWithTimeout(uint8_t length, uint8_t * message, int timeout) {
  unsigned long startTime = millis();
  
  for(int i=0; i<length; i++)
    txData[i] = message[i];

  while((millis() - startTime) < timeout) {
    xbee.send(txPacket);
 
    xbee.readPacket(500);
    if(xbee.getResponse().isAvailable()) {
      if(xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
        xbee.getResponse().getZBTxStatusResponse(txStatus);
        if(txStatus.getDeliveryStatus() == SUCCESS)
          return 1;
      }
    }
  }
  
  return 0;
}

void receivePacketWithoutTimeout(XBeeAddress64 senderAddr, uint8_t length, uint8_t * message) {
  int ready = 0;
  
  while(!ready) {
    xbee.readPacket();
    if(xbee.getResponse().isAvailable()) {
      if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        xbee.getResponse().getZBRxResponse(rxPacket);
        if(rxPacket.getRemoteAddress64().getMsb() == senderAddr.getMsb() && rxPacket.getRemoteAddress64().getLsb() == senderAddr.getLsb()) {
          if(rxPacket.getData(0) == message[0]) {
            for(int i=1; i<length; i++)
              message[i] = rxPacket.getData(i);
            ready = 1;
          }
        }
      }
    }
  }
}

int receivePacketWithTimeout(XBeeAddress64 senderAddr, uint8_t length, uint8_t * message, int timeout) {
  unsigned long startTime = millis();
  
  while((millis() - startTime) < timeout) {
    xbee.readPacket();
    if(xbee.getResponse().isAvailable()) {
      if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        xbee.getResponse().getZBRxResponse(rxPacket);
        if(rxPacket.getRemoteAddress64().getMsb() == senderAddr.getMsb() && rxPacket.getRemoteAddress64().getLsb() == senderAddr.getLsb()) {
          if(rxPacket.getData(0) == message[0]) {
            for(int i=1; i<length; i++)
              message[i] = rxPacket.getData(i);
            return 1;
          }
        }
      }
    }
  }
  
  return 0;
}

