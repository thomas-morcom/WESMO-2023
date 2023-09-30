#include <inttypes.h>
#include <time.h>
#include <Arduino_GFX_Library.h>

Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
    GFX_NOT_DEFINED /* CS */, GFX_NOT_DEFINED /* SCK */, GFX_NOT_DEFINED /* SDA */,
    40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
    5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
    8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */
);

// Uncomment for ST7262 IPS LCD 800x480
 Arduino_RPi_DPI_RGBPanel *gfx = new Arduino_RPi_DPI_RGBPanel(
   bus,
   800 /* width */, 0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 8 /* hsync_back_porch */,
   480 /* height */, 0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 8 /* vsync_back_porch */,
   1 /* pclk_active_neg */, 16000000 /* prefer_speed */, true /* auto_flush */);
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

#define BACKGROUND BLACK
#define GOOD_BATTERY GREEN
#define AVERAGE_BATTERY ORANGE
#define BAD_BATTERY RED

const int I2Caddress = 8;

static int16_t screenWidth, screenHeight, center;

// Measured in Pixels
const int charWidth = 33;
const int charHeight = 71;
const int charSpace = 5;
// 25 for defualt font
const int speedCharScale = 3;

struct rectGraph {
  char *str;
  int x;
  int y;
  int width;
  int height;
  double oldValue;
};

struct rectGraph batterySocGraph;

int prevSpeed = 0;


double drawHorizontalGraph (struct rectGraph rect, int percentValue) {
  
  rect.x++;
  rect.y++;
  rect.width -= 2;
  rect.height -=2;

  // pv = percentValue, which is how much of the graph is to be coloured in
  // e.g., 100 is the full graph, 25 is the bottom quarter
  int16_t pvWidth, xPvDiff;
  uint16_t c;
  int pvDiff;

  // Define the colour that the new graph will be.
  if (percentValue > 50) {
    c = GOOD_BATTERY;
  } else if (percentValue > 25) {
    c = AVERAGE_BATTERY;
  } else {
    c = BAD_BATTERY;
  }

  // Calculate the difference between previouis graph shown, and what will be shown this time
  pvDiff = rect.width * ((rect.oldValue - percentValue)/100.00) +1;
  if (pvDiff < 0) pvDiff = 0;
  // Calculate how the graph wide that will shown is
  pvWidth = rect.width * (percentValue/100.00);
  // Find the y value for the Blank rectangle that will be drawn over the old graph
  xPvDiff = pvWidth + rect.x ;

  // Blank old graph
  gfx->fillRect(xPvDiff,rect.y,pvDiff,rect.height,BACKGROUND);

  // Draw new graph, and only draw it if the colour is changing
  if ((rect.oldValue <= percentValue) || (percentValue < 50 && rect.oldValue >= 50) || (percentValue < 25 && rect.oldValue >= 25)) {
    gfx->fillRect(rect.x,rect.y,pvWidth,rect.height,c);
  } 

  return percentValue;
}

double drawHorizontalGraphOLD (struct rectGraph rect, int perValue) {
  
  rect.x++;
  rect.y++;
  rect.width -= 2;
  rect.height -=2;

  // pv = percentValue, which is how much of the graph is to be coloured in
  // e.g., 100 is the full graph, 25 is the bottom quarter
  int16_t pvWidth;
  uint16_t c;

  // Define the colour that the new graph will be.
  if (perValue > 50) {
    c = GOOD_BATTERY;
  } else if (perValue > 25) {
    c = AVERAGE_BATTERY;
  } else {
    c = BAD_BATTERY;
  }

  // Calculate how the graph wide that will shown is
  pvWidth = rect.width * (perValue/100.00);

  gfx->fillRect(rect.x, rect.y, rect.width, rect.height, BACKGROUND);
  gfx->fillRect(rect.x, rect.y, pvWidth, rect.height, c);

  return perValue;
}


void drawString (int x, int y, char *str) {
  int scale = 3;

  gfx->setCursor(x, y);
  gfx->setTextColor(WHITE);
  gfx->setTextSize(scale);
  gfx->println(str);
}

void drawGraphSetup (struct rectGraph rect) {
  gfx->drawRect(rect.x,rect.y,rect.width,rect.height,WHITE);
  drawString(rect.x, rect.y + rect.height + 10, rect.str);
}


void setup() {

  Serial.begin(115200);
  printf("---SETUP---\n");
  gfx->begin();
  gfx->fillScreen(BACKGROUND);

  // init LCD constant
  screenWidth = gfx->width();
  screenHeight = gfx->height();

  // Set parameteres for Rectangular Bar Graphs
  // {Str, x, y, width, height, oldValue}
  batterySocGraph = {"BATTERY SOC", 0,0,screenWidth,100, 0.0};

  // Draw and setup graph outlines and texts
  drawGraphSetup(batterySocGraph);

  printf("---END OF SETUP---\n");
}

void loop() {
  clock_t start, end;
  int timeSpent;

  // start = clock();

  for (int i = 0; i < 100; i++){
  // for (int i = 99; i > -1; i--){
    start = clock();
    // delay(100);
    batterySocGraph.oldValue = drawHorizontalGraph(batterySocGraph, 100-i);
    end = clock();
    timeSpent = end - start;
    printf("Time for Optimised: %d\n", timeSpent);
    delay(1000);

    start = clock();
    batterySocGraph.oldValue = drawHorizontalGraphOLD(batterySocGraph, 100-i);
    end = clock();
    timeSpent = end - start;
    printf("Time for Old: %d\n", timeSpent);
    delay(2000);
    printf("\n");
  }

  // end = clock();

  // timeSpent = end - start;
  // printf("Time: %d\n", timeSpent);
  // delay(1000);
}