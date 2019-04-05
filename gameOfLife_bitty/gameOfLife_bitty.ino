#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>

#define sck  18
#define sda  23
#define cs   17
#define res  5
#define dc   16

#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

#define HEIGHT 16
#define WIDTH  24

#define BLOCKSIZE 4

Adafruit_SSD1331 display = Adafruit_SSD1331(cs, dc, sda, sck, res);
uint8_t gameBoard[WIDTH][HEIGHT];
uint8_t previousGameBoard[WIDTH][HEIGHT];

int activeColorIndex = 0;
int activeColors[5] = {BLUE, RED, MAGENTA, CYAN, GREEN};

void setup() {
  Serial.begin(9600);
  display.begin();
  display.fillScreen(BLACK);
  
  seedGame(true);
  endRound();
  updateScreen();
}

void loop() {
  playGame();
  endRound();
  updateScreen();
  delay(0);
}

void updateScreen(void)
{
  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      if (gameBoard[x][y] == 0) {
        display.fillRect(x * BLOCKSIZE, y * BLOCKSIZE, BLOCKSIZE, BLOCKSIZE, BLACK);
        display.drawRect(x * BLOCKSIZE, y * BLOCKSIZE, BLOCKSIZE, BLOCKSIZE, BLACK);
      } else {
        display.fillRect(x * BLOCKSIZE, y * BLOCKSIZE, BLOCKSIZE, BLOCKSIZE, activeColors[activeColorIndex]);
        display.drawRect(x * BLOCKSIZE, y * BLOCKSIZE, BLOCKSIZE, BLOCKSIZE, activeColors[activeColorIndex]);
      }
    }
  }
}

void seedGame(bool reset) {
  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      int randomNumber = random(0, 3);
      bool active = false;
      if (randomNumber < 1) {
        gameBoard[x][y] = 1;
      } else {
        if (reset) {
          gameBoard[x][y] = 0;
        }
      }
    }
  }
}

void playGame() {
  bool hasChanged = false;
  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      int neighbors = countNeighbors(x,y);
      if (previousGameBoard[x][y] == 1) {
        // cell is alive
        if (neighbors <= 1) {
          gameBoard[x][y] = 0;
          hasChanged = true;
        } else if (neighbors >= 4) {
          gameBoard[x][y] = 0;
          hasChanged = true;
        }
      } else {
        // cell is dead
        if (neighbors == 3) {
          // cell comes alive
          gameBoard[x][y] = 1;
          hasChanged = true;
        }
      }
    }
  }
  if (hasChanged == false) {
    activeColorIndex = activeColorIndex + 1;
    if (activeColorIndex >= 5) {
      activeColorIndex = 0;
    }
    delay(3300);
    seedGame(false);
  }
  endRound();
}

void endRound() {
  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      previousGameBoard[x][y] = gameBoard[x][y];
    }
  }
}

int countNeighbors(int x, int y) {
  int nCount = 0;
  for (int ix = x - 1; ix <= x + 1; ix++) {
    for (int iy = y - 1; iy <= y + 1; iy++) {
      if (x == ix && y == iy) {
        continue;
      }
      if (ix >= 0 && ix < WIDTH && iy >= 0 && iy < HEIGHT) {
        if (previousGameBoard[ix][iy] == 1) {
          nCount = nCount + 1;
        }
      }
    }
  }
  return nCount;
}
