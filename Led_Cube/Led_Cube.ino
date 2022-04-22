#include <SPI.h>
#include <MsTimer2.h>

unsigned long Timer = 0;

#include "LedCubeData.h"
#include "ButtonDebounce.h"

void NextDisplay()
{
  // Send next pattern to Shift Registers.
  PORTC |= 0x0f;// set bottom bit low, turning off display.
  SPI.transfer16( ~LedCube_NextPlane() );
  // Turn on the current plane.
  PORTC &= ~(1 << LedCube_CurrentPlane );

} // End of NextDisplay

// As a test, each plane is given a location (row,column)
// and that plane is moved through all its locations.
int ZeroRow  = 0, ZeroColumn  = 0;
void MoveZero()
{
  ZeroColumn++;
  if ( ZeroColumn >= 4 )
  {
    ZeroColumn = 0;
    ZeroRow++;
    if ( ZeroRow >= 4 )
      ZeroRow = 0;
  }
} // End of MoveZero

int OneRow   = 1, OneColumn   = 1;
void MoveOne()
{
  OneColumn++;
  if ( OneColumn >= 4 )
  {
    OneColumn = 0;
    OneRow++;
    if ( OneRow >= 4 )
      OneRow = 0;
  }
} // End of MoveOne

int TwoRow   = 2, TwoColumn   = 2;
void MoveTwo()
{
  TwoColumn++;
  if ( TwoColumn >= 4 )
  {
    TwoColumn = 0;
    TwoRow++;
    if ( TwoRow >= 4 )
      TwoRow = 0;
  }
} // End of MoveTwo

int ThreeRow = 3, ThreeColumn = 3;
void MoveThree()
{
  ThreeColumn++;
  if ( ThreeColumn >= 4 )
  {
    ThreeColumn = 0;
    ThreeRow++;
    if ( ThreeRow >= 4 )
      ThreeRow = 0;
  }
}  // End of MoveThree

void Mode0()
{
  LedCube_ClearData();
  MoveZero();
  LedCube_SetLed( ZeroRow,  ZeroColumn,  0 );
  MoveOne();
  LedCube_SetLed( OneRow,   OneColumn,   1 );
  MoveTwo();
  LedCube_SetLed( TwoRow,   TwoColumn,   2 );
  MoveThree();
  LedCube_SetLed( ThreeRow, ThreeColumn, 3 );
}

// Makes LEDs chase each other around the cube.
int ChaseRow = 0, ChaseCol = 0, ChaseLayer = 0;
int ChasePos = 0, ChaseIncrement = 1;
void MoveChase() {
  // The row is incremented back and forth.
  // The function below causes ChaseRow to follow the repeating sequence of values 0, 1, 2, 3, 3, 2, 1, 0
  ChaseRow = min(ChasePos % 8, (63 - ChasePos) % 8);
  // ChaseCol is incremented at 1/4 the speed of ChaseRow, so that each time
  // ChaseRow reaches an edge of the cube, ChaseCol is advanced 1 column in the sequence.
  ChaseCol = min((ChasePos / 4) % 8, (63 - (ChasePos / 4)) % 8);
  // ChaseCol is incremented at 1/4 the speed of ChaseCol (and therefore 1/16 the speed of ChaseRow),
  // so that each time ChaseCol reaches the edge of the cube ChaseLayer is advanced 1 layer in the sequence.
  ChaseLayer = min((ChasePos / 16) % 8, (63 - (ChasePos / 16)) % 8);

  // Switch the direction of the path when ChasePos reaches its min and max.
  if(ChasePos == 0) {
    ChaseIncrement = 1;
  } else if(ChasePos == 63) {
    ChaseIncrement = -1;
  }

  ChasePos += ChaseIncrement;
}

void Chase() {
  LedCube_ClearData();
  LedCube_SetLed(ChaseRow, ChaseCol, ChaseLayer);
  MoveChase();
}

// Shared move code for Layers and Vertical patterns
int layer = 0;
int increment = 1;
void MoveLayer() {
  if (layer == 0) {
    increment = 1;
  } else if (layer == 3) {
    increment = -1;
  }

  layer += increment;
}

// Shift through all the horizontal layers
void Layers() {
  LedCube_ClearData();

  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      LedCube_SetLed(i, j, layer);
    }
  }

  MoveLayer();
}

// Shift through all the vertical layers
void Vertical() {
  LedCube_ClearData();

  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      LedCube_SetLed(i, layer, j);
    }
  }

  MoveLayer();
}

// setup code, run once:
void setup()
{
  MsTimer2::set(4, NextDisplay ); // 4ms period
  MsTimer2::start();

  // A3-A0 to outputs.
  DDRC |= 0x0f;
  // Set up display data.
  LedCube_ClearData();
  // Start up the SPI
  SPI.begin();
  // Set the parameters for the transfers.
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  ButtonInitialize();

  // Timer for moving the ON led's
  Timer = millis();

} // End of setup

int Mode = 0;

// main code, run repeatedly:
void loop()
{
  // 2000 millisecond timer to update display
  if ( millis() - Timer >= 200 )
  {
    // Extra modes could be done here.
    switch (Mode) {
      case 0:
        Mode0();
        break;
      case 1:
        Chase();
        break;
      case 2:
        Layers();
        break;
      case 3:
        Vertical();
        break;
    }

    Timer += 200; // Update timer

  } // End of timer if.

  if ( ButtonNextState(digitalRead(4)) == 1 )
  {
    Mode = (Mode + 1) % 4;
  }
} // End of loop.
