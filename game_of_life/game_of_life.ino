#include "LedControl.h"

LedControl lc = LedControl(10, 5, 7, 1);              // Pins: DIN,CLK,CS, # of Display connected

const int delayTime = 200;                            // Delay between Frames
unsigned char display_num = 0;
const int RANDOMIZE_BUTTON = 12;                      // Pin for Randomize button
int START_BUTTON = 13;                                // Pin for Start/Stop button
int rbtn, sbtn;                                       // these will save the button states of Start/Stop and Randomize buttons
bool started = false;
byte world[8], new_world[8];                          // Array holding the current generation and a buffer for writing the new generation to


bool cellalive(int col, int row) {
  return ((world[row] >> (7 - col)) & 1UL);           // Read out bit (7-col) in row of generation array (1st col is bit 7, 2nd bit 6 etc.)
}

void flipcell(int col, int row) {
  new_world[row] ^= 1UL << (7 - col);                 // Flip bit (7-col) from 0 to 1 or 1 to 0 (1st col is bit 7, 2nd bit 6 etc.)
}

void resetworld() {
  for (int i = 0; i < 8 ; i++) {                      // erase the current generation
    world[i] = B00000000;
  }
  memcpy(new_world, world, sizeof(new_world));        // erase new generation buffer
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (!!random(2)) flipcell(j, i);                // force booleanize a random number and flip the cell if true
    }
  }
  memcpy(world, new_world, sizeof(world));            // update buffer to prepare for generational updates
}


void updatecell(int col, int row) {
  int n = countneighbors(col, row);
  if (cellalive(col, row)) {
    if ((n < 2) || (n > 3)) flipcell(col, row);       // cell is alive. If it has <2 or >3 neighbors, it dies
  } else {
    if (n == 3) flipcell(col, row);                   // cell is dead, but has exactly 3 neighbors. Spawn, baby!
  }
}


int countneighbors(int col, int row) {
  unsigned char count = 0;
  for (int r = row - 1; r <= row + 1; r++) {
    if ((r == -1) || (r == 8)) continue;              // for cells living on the edge!
    for (int c = col - 1; c <= col + 1; c++) {
      if ((c == -1) || (c == 8)) continue;            // dito
      if ((r != row) || (c != col)) {
        if (cellalive(c, r)) {
          count++;
        }
      }
    }
  }
  return count;
}


void drawmatrix(byte matrix[], unsigned char d_num) {
  for (int i = 0; i < 8; i++)
  {
    lc.setColumn(d_num, i, matrix[7 - i]);
  }
}


void setup()
{
  lc.shutdown(display_num, false);                    // Wake up displays
  lc.setIntensity(display_num, 0);                    // Set intensity levels
  lc.clearDisplay(display_num);                       // Clear Displays

  pinMode(RANDOMIZE_BUTTON, INPUT);                   // Set up buttons
  pinMode(START_BUTTON, INPUT);
  resetworld();
}

void loop()
{
  rbtn = digitalRead(RANDOMIZE_BUTTON);
  sbtn = digitalRead(START_BUTTON);

  if (rbtn) {                                         // CAST LIGHTNING INTO THAT PRIMORDIAL SOUP!!!
    resetworld();
    delay(50);
  
  } else if (sbtn) {                                  // starts or stops the iterations
    started = !started;
    delay(50);
  
  } else if (started) {
    drawmatrix(world, display_num);                   // update display with current generation
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        updatecell(j, i);                             // does what it says
      }
    }
    memcpy(world, new_world, sizeof(world));          // set up new generation to be displayed in next iteration
    delay(delayTime);
  }
}
