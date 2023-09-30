#include <inttypes.h>
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
#define MARK_COLOR WHITE
#define SUBMARK_COLOR DARKGREY // LIGHTGREY
#define HOUR_COLOR WHITE
#define MINUTE_COLOR BLUE // LIGHTGREY
#define SECOND_COLOR RED
#define GREEN_BATTERY GREEN
#define ORANGE_BATTERY ORANGE
#define RED_BATTERY RED

#define SIXTIETH 0.016666667
#define TWELFTH 0.08333333
#define SIXTIETH_RADIAN 0.10471976
#define TWELFTH_RADIAN 0.52359878
#define RIGHT_ANGLE_RADIAN 1.5707963

static int16_t screenWidth, screenHeight, center;
static float sdeg, mdeg, hdeg;
static int16_t hHandLen, mHandLen, sHandLen, markLen;
double oldSOC, newSOC;

static const int numOfPoints = 70;
int precalcPoints[numOfPoints][2] = {{99, 381}, {89, 371}, {81, 361}, {73, 350}, {66, 338}, {59, 326}, {54, 313}, {49, 301}, {46, 287}, {43, 274}, {41, 260}, {40, 247}, {40, 233}, {41, 220}, {43, 206}, {46, 193}, {49, 179}, {54, 167}, {59, 154}, {66, 142}, {73, 130}, {81, 119}, {89, 109}, {99, 99}, {109, 89}, {119, 81}, {130, 73}, {142, 66}, {154, 59}, {167, 54}, {179, 49}, {193, 46}, {206, 43}, {220, 41}, {233, 40}, {247, 40}, {260, 41}, {274, 43}, {287, 46}, {301, 49}, {313, 54}, {326, 59}, {338, 66}, {350, 73}, {361, 81}, {371, 89}, {381, 99}, {391, 109}, {399, 119}, {407, 130}, {414, 142}, {421, 154}, {426, 167}, {431, 179}, {434, 193}, {437, 206}, {439, 220}, {440, 233}, {440, 247}, {439, 260}, {437, 274}, {434, 287}, {431, 301}, {426, 313}, {421, 326}, {414, 338}, {407, 350}, {399, 361}, {391, 371}, {381, 381}};

struct rectGraph {
  int x;
  int y;
  int width;
  int height;
};

struct rectGraph batterySOC = {};

void drawFatLine(int16_t 	x0, int16_t 	y0, int16_t 	x1, int16_t 	y1, uint16_t 	color) {
  gfx->drawLine(x0, y0, x1, y1, color);
  gfx->drawLine(x0+1, y0+1, x1+1, y1+1, color);
  gfx->drawLine(x0-1, y0-1, x1-1, y1-1, color);
  gfx->drawLine(x0+1, y0-1, x1+1, y1-1, color);
  gfx->drawLine(x0-1, y0+1, x1-1, y1+1, color);
}

void draw_round_clock_mark(int16_t innerR1, int16_t outerR1, int16_t innerR2, int16_t outerR2, int16_t innerR3, int16_t outerR3)
{
  Serial.printf("draw round clock mark");
  float x, y;
  int16_t x0, x1, y0, y1, innerR, outerR;
  uint16_t c;

  for (uint8_t i = 0; i < 60; i++)
  {
    if ((i % 15) == 0)
    {
      innerR = innerR1;
      outerR = outerR1;
      // c = MARK_COLOR;
      c = HOUR_COLOR;
    }
    else if ((i % 5) == 0)
    {
      innerR = innerR2;
      outerR = outerR2;
      // c = MARK_COLOR;
      c = MINUTE_COLOR;
    }
    else
    {
      innerR = innerR3;
      outerR = outerR3;
      // c = SUBMARK_COLOR;
      c = SECOND_COLOR;
    }

    mdeg = (SIXTIETH_RADIAN * i) - RIGHT_ANGLE_RADIAN;
    x = cos(mdeg);
    y = sin(mdeg);
    x0 = x * outerR + center;
    y0 = y * outerR + center;
    x1 = x * innerR + center;
    y1 = y * innerR + center;

    gfx->drawLine(x0, y0, x1, y1, c);

    if (i == 22) i+=15;
  }
}

void drawVerticalGraph (int sw, int sh, int gw, int soc) {
  
  int16_t x, y, height, ySocDiff;
  uint16_t c;
  int socDiff;

  // Serial.printf("soc: %d  ", soc);
  newSOC = soc;
  socDiff = sh * ((oldSOC - newSOC)/100.00) +1;
  if (socDiff < 0) socDiff = 0;

  if (soc >= 50) {
    c = GREEN_BATTERY;
  } else if (soc >= 25) {
    c = ORANGE_BATTERY;
  } else {
    c = RED_BATTERY;
  }

  gw = 100;
  // Serial.printf("Percent of charge: %f  ", (double)(soc/100.00));
  height = sh * (soc/100.00);
  // printf("height: %" PRIu32 "\n",height);

  x = sw - gw;
  y = sh - height;
  ySocDiff = y - socDiff;
  
  // Serial.printf("soc: %d  ", soc);
  // Serial.printf("newSOC: %d  ", newSOC);
  // Serial.printf("oldSOC: %d  ", oldSOC);
  // printf("y: %" PRIu32 "  ",y);
  // printf("socDiff: %" PRIu32 "  ",socDiff);
  // printf("ySocDiff: %" PRIu32 "\n",ySocDiff);

  // Blank old graph
  gfx->fillRect(x,ySocDiff,gw,socDiff,BACKGROUND);
  // Draw new graph
  gfx->fillRect(x,y,gw,height,c);
  // Draw Outline
  gfx->drawRect(screenWidth-gw,0,gw,screenHeight,WHITE);

  oldSOC = soc;
}

// Each character is 5x7 pixels, with 1 pixel space between them
void drawSpeedNumbers(int centerHorizontal, int top, int speed) {
  unsigned char c = speed;
  int scale = 15;
  int left;
  // gfx->drawChar(left, top, c, PURPLE, BACKGROUND);
  // gfx->print();

  left = centerHorizontal - (5.5*scale);
  gfx->fillRect(left, top, (5+1+5)*scale, 7*scale, BACKGROUND);
  gfx->setCursor(left, top);
  gfx->setTextColor(WHITE);
  gfx->setTextSize(15);
  gfx->println(speed);
}

void setup() {
  Serial.begin(115200);
  gfx->begin();
  gfx->fillScreen(BACKGROUND);

  // init LCD constant
  screenWidth = gfx->width();
  screenHeight = gfx->height();

  if (screenWidth < screenHeight)
  {
    center = screenWidth / 2;
  }
  else
  {
    center = screenHeight / 2;
  }
  
  markLen = center * 5 / 36;
  draw_round_clock_mark(
  // draw_square_clock_mark(
    center - markLen, center,
    center - (markLen * 2 / 3), center,
    center - (markLen / 2), center
  );
}

void loop() {
  for (int i = 20; i < 21; i++){
  // for (int i = 70; i > 0; i--){
    drawFatLine(center, center, precalcPoints[i][0], precalcPoints[i][1], HOUR_COLOR);
    // gfx->drawLine(center, center, precalcPoints[i][0], precalcPoints[i][1], SECOND_COLOR);
    drawVerticalGraph(screenWidth, screenHeight, 100, 70-i);

    drawSpeedNumbers(center, screenHeight-120, i);

    delay(100);

    // gfx->drawLine(center, center, precalcPoints[i][0], precalcPoints[i][1], BACKGROUND);
    drawFatLine(center, center, precalcPoints[i][0], precalcPoints[i][1], BACKGROUND);
  }
}
