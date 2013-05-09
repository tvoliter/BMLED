#include <FastSPI_LED.h>
#include "XBee.h"

#define NUM_NODES 4
#define MASTER_NODE 0
#define THIS_NODE 3

#define NUM_LEDS 150
#define LED_PIN 4

// xbee addresses
#define NODE0_ADDR_HI 0x13a200
#define NODE0_ADDR_LO 0x4093d477
#define NODE1_ADDR_HI 0x13a200
#define NODE1_ADDR_LO 0x4093d4b5
#define NODE2_ADDR_HI 0x13a200
#define NODE2_ADDR_LO 0x4092ff8e
#define NODE3_ADDR_HI 0x13a200
#define NODE3_ADDR_LO 0x4092fe01

// message definitions
#define MSG_SIZE 4

#define MSG_ANY      0x00
#define MSG_START    0xfe
#define MSG_FINISH   0xff
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
XBeeAddress64 nodeAddr[NUM_NODES];
uint8_t txData[MSG_SIZE];
ZBTxRequest txPacket[NUM_NODES];
ZBTxStatusResponse txStatus;
ZBRxResponse rxPacket;

// variables used in loop() placed here so that they don't get stack-allocated each time loop() is called
int i, j;
uint8_t present;
uint8_t message[MSG_SIZE];


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

  nodeAddr[0] = XBeeAddress64(NODE0_ADDR_HI, NODE0_ADDR_LO);
  nodeAddr[1] = XBeeAddress64(NODE1_ADDR_HI, NODE1_ADDR_LO);
  nodeAddr[2] = XBeeAddress64(NODE2_ADDR_HI, NODE2_ADDR_LO);
  nodeAddr[3] = XBeeAddress64(NODE3_ADDR_HI, NODE3_ADDR_LO);

  for(i=0; i<NUM_NODES; i++)
    txPacket[i] = ZBTxRequest(nodeAddr[i], txData, sizeof(txData));
  txStatus = ZBTxStatusResponse();
  rxPacket = ZBRxResponse();
  
  delay(100); // do not delete
}


void loop() {

  if(THIS_NODE == MASTER_NODE) {
    present = 0;
    
    // send start message tell the other nodes to start
    message[0] = MSG_START;
    for(i=0; i<NUM_NODES; i++) {
      if(i != THIS_NODE) {
        //sendPacketWithoutRetry(i, message);
        if(sendPacketWithTimeout(i, message, 20))
          present |= (1<< i);
      }
    }
    
    // randomly pick a sequence for the other nodes to display
    switch(random() % 3) {
      case 0:
        message[0] = MSG_FORWARD;
        break;
      case 1:
        message[0] = MSG_BACKWARD;
        break;
      case 2:
        message[0] = MSG_TWINKLE;
        // generate random color to twinkle
        message[1] = random() & 0xff; // r
        message[2] = random() & 0xff; // g
        message[3] = random() & 0xff; // b
        break;
    }

    // send display message to the other nodes
    for(i=0; i<NUM_NODES; i++) {
      if(i != THIS_NODE && (present & (1 << i)) != 0) {
        //sendPacketWithTimeout(i, message, 50);
        sendPacketWithoutRetry(i, message);
      }
    }
    
    delay(100);
    
    // wait all present nodes to finish
    message[0] = MSG_FINISH;
    while(present != 0) {
      receivePacketWithTimeout(-1, message, 10000);
      present &= ~(1 << message[1]);
    }
    
    // wait for all other nodes to finish
    //delay(50);
    
    // clear out the other MSG_FINISH packets
    //for(i=0; i<NUM_NODES; i++)
    //  xbee.readPacket();
  }
  else {
    // wait for master node to say start
    message[0] = MSG_START;
    
    /*  
    memset(leds, 0, NUM_LEDS * 3);
    while(1) {
      i = random() % NUM_LEDS;
      setColor(i, GREEN);
      FastSPI_LED.show();
      delay(5);
      setColor(i, 0);
      // check if any other node has finished
      if(receivePacketWithTimeout(MASTER_NODE, message, 10))
        break;
    }
    */
    receivePacketWithoutTimeout(MASTER_NODE, message);

    // wait for message from master
    message[0] = MSG_ANY;
    if(receivePacketWithTimeout(MASTER_NODE, message, 5000)) {
      if(message[0] == MSG_FORWARD) {
        drawTravellingRainbow(1, 0);
      }
      else if(message[0] == MSG_BACKWARD) {
        drawTravellingRainbow(1, 1);
      }
      else if(message[0] == MSG_TWINKLE) {
        // twinkle with color rgb
        twinkle(200, ((long) message[1] << 16) | ((long) message[2] << 8) | (long) message[3]);
      }
      
      message[0] = MSG_FINISH;
      message[1] = THIS_NODE;
      sendPacketWithTimeout(MASTER_NODE, message, 50);
      
    }
  }
  
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


void sendPacketWithoutRetry(int destNodeNum, uint8_t * message) {
  for(int i=0; i<MSG_SIZE; i++)
    txData[i] = message[i];

  xbee.send(txPacket[destNodeNum]);
  // get tx status packet 
  xbee.readPacket(10);
}

void sendPacketWithoutTimeout(int destNodeNum, uint8_t * message) {
  int ready = 0;

  for(int i=0; i<MSG_SIZE; i++)
    txData[i] = message[i];

  while(!ready) {
    xbee.send(txPacket[destNodeNum]);
 
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

int sendPacketWithTimeout(int destNodeNum, uint8_t * message, int timeout) {
  unsigned long startTime = millis();
  
  for(int i=0; i<MSG_SIZE; i++)
    txData[i] = message[i];

  while((millis() - startTime) < timeout) {
    xbee.send(txPacket[destNodeNum]);
 
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

// srcNodeNum == -1 means receive packet from any node
// message[0] == MSG_ANY means receive any message
// otherwise receive only the specified message[0] from the specified node
void receivePacketWithoutTimeout(int srcNodeNum, uint8_t * message) {
  boolean ready = 0;
  
  while(!ready) {
    xbee.readPacket();
    if(xbee.getResponse().isAvailable()) {
      if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        xbee.getResponse().getZBRxResponse(rxPacket);

        if((message[0] == MSG_ANY) || (message[0] == rxPacket.getData(0))) {
          if(srcNodeNum == -1)
            ready = 1;
          else if((rxPacket.getRemoteAddress64().getMsb() == nodeAddr[srcNodeNum].getMsb()) &&
                  (rxPacket.getRemoteAddress64().getLsb() == nodeAddr[srcNodeNum].getLsb()))
            ready = 1;
        }

        if(ready) {
          for(int i=0; i<MSG_SIZE; i++)
            message[i] = rxPacket.getData(i);
        }
      }
    }
  }
}

int receivePacketWithTimeout(int srcNodeNum, uint8_t * message, int timeout) {
  boolean ready = 0;
  unsigned long startTime = millis();
  
  while((millis() - startTime) < timeout) {
    xbee.readPacket();
    if(xbee.getResponse().isAvailable()) {
      if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        xbee.getResponse().getZBRxResponse(rxPacket);

        if((message[0] == MSG_ANY) || (message[0] == rxPacket.getData(0))) {
          if(srcNodeNum == -1)
            ready = 1;
          else if((rxPacket.getRemoteAddress64().getMsb() == nodeAddr[srcNodeNum].getMsb()) &&
                  (rxPacket.getRemoteAddress64().getLsb() == nodeAddr[srcNodeNum].getLsb()))
            ready = 1;
        }

        if(ready) {
          for(int i=0; i<MSG_SIZE; i++)
            message[i] = rxPacket.getData(i);
          return 1;
        }
      }
    }
  }
  
  return 0;
}

