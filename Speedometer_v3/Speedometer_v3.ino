#include <inttypes.h>
#include <U8g2lib.h>
#include <Arduino_GFX_Library.h>
#include <Wire.h>

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
struct rectGraph powerGraph;
struct rectGraph brakeGraph;
struct rectGraph acceleratorGraph;
struct rectGraph speedCoordinates; // Not exactly a graph but the struct is good for all the info like coordinates, size, and text

int prevSpeed = 0;

double drawVerticalGraph (struct rectGraph rect, int percentValue) {
  
  rect.x++;
  rect.y++;
  rect.width -= 2;
  rect.height -=2;

  // pv = percentValue, which is how much of the graph is to be coloured in
  // e.g., 100 is the full graph, 25 is the bottom quarter
  int16_t y, pvHeight, yPvDiff;
  uint16_t c = GREEN;
  int pvDiff;

  // Calculate the difference between previouis graph shown, and what will be shown this time
  pvDiff = rect.height * ((rect.oldValue - percentValue)/100.00) +1;
  if (pvDiff < 0) pvDiff = 0;
  // Calculate how the graph tall that will shown is
  pvHeight = rect.height * (percentValue/100.00);
  // Find the y value of the graph
  y = rect.height - pvHeight + rect.y;
  // Find the y value for the Blank rectangle that will be drawn over the old graph
  yPvDiff = y - pvDiff;

  // Blank old graph
  gfx->fillRect(rect.x,yPvDiff,rect.width,pvDiff,BACKGROUND);

  // Draw new graph, and only draw it if the colour is changing
  // if ((rect.oldValue <= percentValue) || (percentValue < 50 && rect.oldValue >= 50) || (percentValue < 25 && rect.oldValue >= 25)) {
  if (rect.oldValue <= percentValue) {
    gfx->fillRect(rect.x,y,rect.width,pvHeight,c);
  } 

  return percentValue;
}

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

// Each character is 5x7 pixels, with 1 pixel space between them
void drawSpeedNumbers(int x, int y, int speed) {

  if (prevSpeed == speed) return;
  // printf("Drawing a new speed\n");

  int numOfChars = 0;
  int h = charHeight*speedCharScale;
  int w = charWidth*speedCharScale;
  int s = charSpace*speedCharScale;

  if ((prevSpeed > 99 && speed < 100) || (prevSpeed > 9 && speed < 10)) {
    // printf("Manually Blanking the speed display\n");
    // printf("BLANKING --> X: %d, Y: %d, Width: %d, Height: %d\n", x, y-h, (3*w + 3*s), h+speedCharScale);

    gfx->fillRect(x, y-h, (3*w + 3*s), h+1, BACKGROUND);
  } 

  if (speed < 10) {
    numOfChars = 1;
    x += 2*(w+s)+s;
    gfx->fillRect(x,y-h,w+s,h+speedCharScale,BACKGROUND);
  } else if (speed < 100) {
    numOfChars = 2;
    x += w + 2*s;
    gfx->fillRect(x,y-h,2*(w+s),h+speedCharScale,BACKGROUND);
  }
  else {
    numOfChars = 3;
    gfx->fillRect(x,y-h,3*(w+s)+s,h+speedCharScale,BACKGROUND);
  }

  gfx->setCursor(x, y);
  gfx->setFont(u8g2_font_logisoso62_tn);
  // First colour is the Text Color, the next is the background color
  gfx->setTextColor(WHITE, BACKGROUND);
  gfx->setTextSize(speedCharScale);
  gfx->println(speed);

  prevSpeed = speed;
}

void setup() {
  Wire.begin(I2Caddress);
  Wire.onReceive(receiveNumber);

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
  powerGraph = {"PWR", screenWidth - 100,200,50,200, 0.0};
  brakeGraph = {"BRK", screenWidth - 200,200,50,200, 0.0};
  acceleratorGraph = {"ACL", screenWidth - 300,200,50,200, 0.0};
  speedCoordinates = {"KM/H",10,375,3*charWidth*speedCharScale,charHeight*speedCharScale, 0.0};

  // Draw and setup graph outlines and texts
  drawGraphSetup(batterySocGraph);
  drawGraphSetup(powerGraph);
  drawGraphSetup(brakeGraph);
  drawGraphSetup(acceleratorGraph);
  drawString(speedCoordinates.x+360, speedCoordinates.y-21, speedCoordinates.str);
  // drawString(speedCoordinates.x+390, speedCoordinates.y+50, speedCoordinates.str);

  printf("---END OF SETUP---\n");
}

void loop() {
  for (int i = 0; i < 100; i++){
  // for (int i = 99; i > -1; i--){
    batterySocGraph.oldValue = drawHorizontalGraph(batterySocGraph, 100-i);
    powerGraph.oldValue = drawVerticalGraph(powerGraph, ((100-i)%50)*2);
    brakeGraph.oldValue = drawVerticalGraph(brakeGraph, ((100-i)%20)*5);
    acceleratorGraph.oldValue = drawVerticalGraph(acceleratorGraph, ((i)%20)*5);
    drawSpeedNumbers(speedCoordinates.x, speedCoordinates.y, i);

    delay(100);
  }
}

void receiveNumber(int numBytes) {
  Serial.println("In receiveNumber method");
  int receivedData;
  while (Wire.available()) {
    Wire.readBytes((uint8_t*)&receivedData, sizeof(receivedData)); // Read the integer from the I2C bus
    Serial.print("Received integer: ");
    Serial.println(receivedData);
    drawSpeedNumbers(speedCoordinates.x, speedCoordinates.y, receivedData);
  }
}
