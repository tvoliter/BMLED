#include <FastSPI_LED.h>

#define NUM_LEDS 102
#define LED_PIN 4

// colors
#define RED    0xff0000
#define ORANGE 0xff7f00
#define YELLOW 0xffff00
#define GREEN  0x00ff00
#define BLUE   0x0000ff
#define INDIGO 0x4b00ff
#define VIOLET 0x8f00ff

#define WHITE 0xffffff

// Sometimes chipsets wire in a backwards sort of way
struct CRGB { unsigned char b; unsigned char r; unsigned char g; };
//struct CRGB { unsigned char r; unsigned char g; unsigned char b; };
struct CRGB *leds;

byte red[10][12]; //10 columns 12 LEDs each - first column, then line
byte green[10][12];
byte blue[10][12];

//int delay_time = 20;
int damping = 1;

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

void loop() {
// define origin
int x0 = random(10);
int y0 = random(12);
  
// define background color
int r0 = 0;
int g0 = 0;
int b0 = 0;
int speed = 4;

//flash color
int r1 = random(100, 256);
int g1 = random(100, 256);
int b1 = random(100, 256);

//write background color to all dots
for(int i = 0; i < 10; i++) {
  for (int j = 0; j < 12; j++) {
    red[i][j] = r0;
    green[i][j] = g0;
    blue[i][j] = b0;
  }
}
show_matrix(100/speed);

for(int f = 20; f>0; f--) {
  for(int i = 0; i < 10; i++) {
    for (int j = 0; j < 12; j++) {
      float length = sqrt(abs(i-x0)*abs(i-x0) + abs(j-y0)*abs(j-y0)); //calculate length from origin
        red[i][j] = r0 + (r1-r0)/((int) length *f); green[i][j] = g0 + (g1-g0)/((int) length *f); ; blue[i][j] = b0 + (b1-b0)/((int) length *f);
    }
  }
  red[x0][y0] = r0 + (r1-r0)/f; green[x0][y0] = g0 + (g1-g0)/f; ; blue[x0][y0] = b0 + (b1-b0)/f;
  show_matrix(20/speed);
}
for(int f = 1; f < 20; f++) {
  for(int i = 0; i < 10; i++) {
    for (int j = 0; j < 12; j++) {
      float length = abs(i-x0)*abs(i-x0) + abs(j-y0)*abs(j-y0); //calculate length from origin
        red[i][j] = r0 + (r1-r0)/((int) length *f); green[i][j] = g0 + (g1-g0)/((int) length *f); ; blue[i][j] = b0 + (b1-b0)/((int) length *f);
    }
  }
  red[x0][y0] = r0 + (r1-r0)/f; green[x0][y0] = g0 + (g1-g0)/f; ; blue[x0][y0] = b0 + (b1-b0)/f;
  show_matrix(10/speed);
}
//


}

void show_matrix(int delay_time) {
  // 0. column 0 to 5 counting up
  leds[0].r = red[0][6]; leds[0].g = green[0][6]; leds[0].b = blue[0][6];
  leds[1].r = red[0][7]; leds[1].g = green[0][7]; leds[1].b = blue[0][7];
  leds[2].r = red[0][8]; leds[2].g = green[0][8]; leds[2].b = blue[0][8];
  leds[3].r = red[0][9]; leds[3].g = green[0][9]; leds[3].b = blue[0][9];
  leds[4].r = red[0][10]; leds[4].g = green[0][10]; leds[4].b = blue[0][10];
  leds[5].r = red[0][11]; leds[5].g = green[0][11]; leds[5].b = blue[0][11];
  // 1. column 14 to 6 counting down
  leds[6].r = red[1][11]; leds[6].g = green[1][11]; leds[6].b = blue[1][11];
  leds[7].r = red[1][10]; leds[7].g = green[1][10]; leds[7].b = blue[1][10];
  leds[8].r = red[1][9]; leds[8].g = green[1][9]; leds[8].b = blue[1][9];
  leds[9].r = red[1][8]; leds[9].g = green[1][8]; leds[9].b = blue[1][8];
  leds[10].r = red[1][7]; leds[10].g = green[1][7]; leds[10].b = blue[1][7];
  leds[11].r = red[1][6]; leds[11].g = green[1][6]; leds[11].b = blue[1][6];
  leds[12].r = red[1][5]; leds[12].g = green[1][5]; leds[12].b = blue[1][5];
  leds[13].r = red[1][4]; leds[13].g = green[1][4]; leds[13].b = blue[1][4];
  leds[14].r = red[1][3]; leds[14].g = green[1][3]; leds[14].b = blue[1][3];
  // 2. column 15 to 26 counting up
  leds[15].r = red[2][0]; leds[15].g = green[2][0]; leds[15].b = blue[2][0];
  leds[16].r = red[2][1]; leds[16].g = green[2][1]; leds[16].b = blue[2][1];
  leds[17].r = red[2][2]; leds[17].g = green[2][2]; leds[17].b = blue[2][2];
  leds[18].r = red[2][3]; leds[18].g = green[2][3]; leds[18].b = blue[2][3];
  leds[19].r = red[2][4]; leds[19].g = green[2][4]; leds[19].b = blue[2][4];
  leds[20].r = red[2][5]; leds[20].g = green[2][5]; leds[20].b = blue[2][5];
  leds[21].r = red[2][6]; leds[21].g = green[2][6]; leds[21].b = blue[2][6];
  leds[22].r = red[2][7]; leds[22].g = green[2][7]; leds[22].b = blue[2][7];
  leds[23].r = red[2][8]; leds[23].g = green[2][8]; leds[23].b = blue[2][8];
  leds[24].r = red[2][9]; leds[24].g = green[2][9]; leds[24].b = blue[2][9];
  leds[25].r = red[2][10]; leds[25].g = green[2][10]; leds[25].b = blue[2][10];
  leds[26].r = red[2][11]; leds[26].g = green[2][11]; leds[26].b = blue[2][11];
  // 3. column 27 to 38 counting down
  leds[27].r = red[3][11]; leds[27].g = green[3][11]; leds[27].b = blue[3][11];
  leds[28].r = red[3][10]; leds[28].g = green[3][10]; leds[28].b = blue[3][10];
  leds[29].r = red[3][9]; leds[29].g = green[3][9]; leds[29].b = blue[3][9];
  leds[30].r = red[3][8]; leds[30].g = green[3][8]; leds[30].b = blue[3][8];
  leds[31].r = red[3][7]; leds[31].g = green[3][7]; leds[31].b = blue[3][7];
  leds[32].r = red[3][6]; leds[32].g = green[3][6]; leds[32].b = blue[3][6];
  leds[33].r = red[3][5]; leds[33].g = green[3][5]; leds[33].b = blue[3][5];
  leds[34].r = red[3][4]; leds[34].g = green[3][4]; leds[34].b = blue[3][4];
  leds[35].r = red[3][3]; leds[35].g = green[3][3]; leds[35].b = blue[3][3];
  leds[36].r = red[3][2]; leds[36].g = green[3][2]; leds[36].b = blue[3][2];
  leds[37].r = red[3][1]; leds[37].g = green[3][1]; leds[37].b = blue[3][1];
  leds[38].r = red[3][0]; leds[38].g = green[3][0]; leds[38].b = blue[3][0];
  // 4. column 39 to 50 counting up
  leds[39].r = red[4][0]; leds[39].g = green[4][0]; leds[39].b = blue[4][0];
  leds[40].r = red[4][1]; leds[40].g = green[4][1]; leds[40].b = blue[4][1];
  leds[41].r = red[4][2]; leds[41].g = green[4][2]; leds[41].b = blue[4][2];
  leds[42].r = red[4][3]; leds[42].g = green[4][3]; leds[42].b = blue[4][3];
  leds[43].r = red[4][4]; leds[43].g = green[4][4]; leds[43].b = blue[4][4];
  leds[44].r = red[4][5]; leds[44].g = green[4][5]; leds[44].b = blue[4][5];
  leds[45].r = red[4][6]; leds[45].g = green[4][6]; leds[45].b = blue[4][6];
  leds[46].r = red[4][7]; leds[46].g = green[4][7]; leds[46].b = blue[4][7];
  leds[47].r = red[4][8]; leds[47].g = green[4][8]; leds[47].b = blue[4][8];
  leds[48].r = red[4][9]; leds[48].g = green[4][9]; leds[48].b = blue[4][9];
  leds[49].r = red[4][10]; leds[49].g = green[4][10]; leds[49].b = blue[4][10];
  leds[50].r = red[4][11]; leds[50].g = green[4][11]; leds[50].b = blue[4][11];
  //5. colum 51 to 62 counting down
  leds[51].r = red[5][11]; leds[51].g = green[5][11]; leds[51].b = blue[5][11];
  leds[52].r = red[5][10]; leds[52].g = green[5][10]; leds[52].b = blue[5][10];
  leds[53].r = red[5][9]; leds[53].g = green[5][9]; leds[53].b = blue[5][9];
  leds[54].r = red[5][8]; leds[54].g = green[5][8]; leds[54].b = blue[5][8];
  leds[55].r = red[5][7]; leds[55].g = green[5][7]; leds[55].b = blue[5][7];
  leds[56].r = red[5][6]; leds[56].g = green[5][6]; leds[56].b = blue[5][6];
  leds[57].r = red[5][5]; leds[57].g = green[5][5]; leds[57].b = blue[5][5];
  leds[58].r = red[5][4]; leds[58].g = green[5][4]; leds[58].b = blue[5][4];
  leds[59].r = red[5][3]; leds[59].g = green[5][3]; leds[59].b = blue[5][3];
  leds[60].r = red[5][2]; leds[60].g = green[5][2]; leds[60].b = blue[5][2];
  leds[61].r = red[5][1]; leds[61].g = green[5][1]; leds[61].b = blue[5][1];
  leds[62].r = red[5][0]; leds[62].g = green[5][0]; leds[62].b = blue[5][0];
  //6. column 63 to 74 counting up
  leds[63].r = red[6][0]; leds[63].g = green[6][0]; leds[63].b = blue[6][0];
  leds[64].r = red[6][1]; leds[64].g = green[6][1]; leds[64].b = blue[6][1];
  leds[65].r = red[6][2]; leds[65].g = green[6][2]; leds[65].b = blue[6][2];
  leds[66].r = red[6][3]; leds[66].g = green[6][3]; leds[66].b = blue[6][3];
  leds[67].r = red[6][4]; leds[67].g = green[6][4]; leds[67].b = blue[6][4];
  leds[68].r = red[6][5]; leds[68].g = green[6][5]; leds[68].b = blue[6][5];
  leds[69].r = red[6][6]; leds[69].g = green[6][6]; leds[69].b = blue[6][6];
  leds[70].r = red[6][7]; leds[70].g = green[6][7]; leds[70].b = blue[6][7];
  leds[71].r = red[6][8]; leds[71].g = green[6][8]; leds[71].b = blue[6][8];
  leds[72].r = red[6][9]; leds[72].g = green[6][9]; leds[72].b = blue[6][9];
  leds[73].r = red[6][10]; leds[73].g = green[6][10]; leds[73].b = blue[6][10];
  leds[74].r = red[6][11]; leds[74].g = green[6][11]; leds[74].b = blue[6][11];
  //7. column 75 to 86 counting down
  leds[75].r = red[7][11]; leds[75].g = green[7][11]; leds[75].b = blue[7][11];
  leds[76].r = red[7][10]; leds[76].g = green[7][10]; leds[76].b = blue[7][10];
  leds[77].r = red[7][9]; leds[77].g = green[7][9]; leds[77].b = blue[7][9];
  leds[78].r = red[7][8]; leds[78].g = green[7][8]; leds[78].b = blue[7][8];
  leds[79].r = red[7][7]; leds[79].g = green[7][7]; leds[79].b = blue[7][7];
  leds[80].r = red[7][6]; leds[80].g = green[7][6]; leds[80].b = blue[7][6];
  leds[81].r = red[7][5]; leds[81].g = green[7][5]; leds[81].b = blue[7][5];
  leds[82].r = red[7][4]; leds[82].g = green[7][4]; leds[82].b = blue[7][4];
  leds[83].r = red[7][3]; leds[83].g = green[7][3]; leds[83].b = blue[7][3];
  leds[84].r = red[7][2]; leds[84].g = green[7][2]; leds[84].b = blue[7][2];
  leds[85].r = red[7][1]; leds[85].g = green[7][1]; leds[85].b = blue[7][1];
  leds[86].r = red[7][0]; leds[86].g = green[7][0]; leds[86].b = blue[7][0];
  //8.column 87 to 95 counting up - starting at 3
  leds[87].r = red[8][3]; leds[87].g = green[8][3]; leds[87].b = blue[8][3];
  leds[88].r = red[8][4]; leds[88].g = green[8][4]; leds[88].b = blue[8][4];
  leds[89].r = red[8][5]; leds[89].g = green[8][5]; leds[89].b = blue[8][5];
  leds[90].r = red[8][6]; leds[90].g = green[8][6]; leds[90].b = blue[8][6];
  leds[91].r = red[8][7]; leds[91].g = green[8][7]; leds[91].b = blue[8][7];
  leds[92].r = red[8][8]; leds[92].g = green[8][8]; leds[92].b = blue[8][8];
  leds[93].r = red[8][9]; leds[93].g = green[8][9]; leds[93].b = blue[8][9];
  leds[94].r = red[8][10]; leds[94].g = green[8][10]; leds[94].b = blue[8][10];
  leds[95].r = red[8][11]; leds[95].g = green[8][11]; leds[95].b = blue[8][11];
  //9. column 96 to 102 counting down - ending at 6
  leds[96].r = red[9][11]; leds[96].g = green[9][11]; leds[96].b = blue[9][11];
  leds[97].r = red[9][10]; leds[97].g = green[9][10]; leds[97].b = blue[9][10];
  leds[98].r = red[9][9]; leds[98].g = green[9][9]; leds[98].b = blue[9][9];
  leds[99].r = red[9][8]; leds[99].g = green[9][8]; leds[99].b = blue[9][8];
  leds[100].r = red[9][7]; leds[100].g = green[9][7]; leds[100].b = blue[9][7];
  leds[101].r = red[9][6]; leds[101].g = green[9][6]; leds[101].b = blue[9][6];
  FastSPI_LED.show();
  delay(delay_time);
}
  
  
  
  
  
 
 
  

