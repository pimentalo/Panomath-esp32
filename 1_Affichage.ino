#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>



/*Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
  32, 8,
  1, 3,  // 1 tile X, 3 tiles Y
  PIN_LED_MATRIX,
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT +              // First pixel of matrix is top and left 
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE +   // Progressive matrix
  NEO_TILE_TOP + NEO_TILE_LEFT + NEO_TILE_ZIGZAG, // Matrix tiles disposed in column and direction shift each row 
  NEO_GRB + NEO_KHZ800);
*/
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
  32, 8,
  1, 3,  // 1 tile X, 3 tiles Y
  PIN_LED_MATRIX,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT +     // First pixel of matrix is top and left 
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG +   // Zig zag matrix
  NEO_TILE_TOP + NEO_TILE_LEFT + 
  NEO_TILE_ROWS + NEO_TILE_ZIGZAG, // Matrix tiles disposed in column and direction shift each row 
  NEO_GRB + NEO_KHZ800);



const uint16_t colors[] = {
  matrix.Color(0x00,0x00,0x00), // BLACK
  matrix.Color(0x00,0x00,0xAA), // BLUE
  matrix.Color(0X00,0XAA,0X00), // GREEN 
  matrix.Color(0X00,0xAA,0XAA), // CYAN 
  matrix.Color(0XAA,0X00,0X00), // RED
  matrix.Color(0xAA,0x00,0xAA), // MAGENTA 
  matrix.Color(0xAA,0x55,0x00), // BROWN
  matrix.Color(0xAA,0xAA,0xAA), // LIGHTGREY
  matrix.Color(0x55,0x55,0x55), // GREY
  matrix.Color(0x55,0x55,0xFF), // LIGHTBLUE
  matrix.Color(0X55,0XFF,0X55), // LIGHTGREEN
  matrix.Color(0X55,0xFF,0XFF), // LIGHTCYAN
  matrix.Color(0XFF,0X55,0X55), // LIGHTRED
  matrix.Color(0xFF,0x55,0xFF), // LIGHTMAGENTA
  matrix.Color(0xFF,0x55,0xFF), // YELLOW 
  matrix.Color(0xFF,0xFF,0xFF), // WHITE
};

Affichage::Affichage()
{
}

// Clear display
void Affichage::clear() {
  matrix.clear();
}

/*
  Set line background
*/ 
void Affichage::setLineBackground(int line, int type, void* content)
{
  LOG_ERROR("setLineBackground non implémenté");
}

/*
  Set line text
*/ 
void Affichage::setLineText(int line, char* content,int foreGround, int backGround)
{
  if (line >= AFFICHAGE_NB_LIGNES || line < 0) {
    LOG_ERROR("setLineText: line %i out of range", line);
  }
  if (foreGround > 15 || foreGround < 0) {
    LOG_ERROR("setLineText: color %i out of range", foreGround);
  }
  matrix.setTextSize(1);
  matrix.setCursor(0, line * 8);
  matrix.setTextColor(colors[foreGround]);
  matrix.print(content);
}

void Affichage::show()
{
  matrix.show();
}

void Affichage::setup()
{
  Serial.println("Affichage()");
  matrix.setTextWrap(false);
  matrix.setBrightness(42);
  matrix.setTextColor(colors[0]);
  matrix.begin();
}
