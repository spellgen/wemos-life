#include <Arduino.h>
#include <WEMOS_Matrix_LED.h>

MLED mled(1); //set intensity=5

#define DELAY 200 // frame delay in ms

uint8 state1[8] = {0x00, 0x00, 0x1c, 0x10, 0x08, 0x00, 0x0e, 0x00}; // rather random
uint8 state2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x04, 0x02}; // glider
uint8 state3[8] = {0x00, 0x00, 0x00, 0x00, 0xe2, 0x03, 0x40, 0x00}; // metusaleh
uint8 state4[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x20, 0x10}; // centered glider
uint8 state5[8] = {0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00}; // a line

uint8 *current = state4;
uint8 previous[8];

uint8 count = 0;

int loadState(uint8 *to, uint8 *from) {
    for(int i=0;i<8; i++) {
        to[i]=from[i];
    }
    return 0;
}

// return the state of map[i,j]
int get(uint8 *map, int i, int j) {
  int row = map[(i+8)%8];
  int col = (j+8)%8;
  return row & (1<<col);
}

// set the state of map[i,j] to s - i and j in the range [0,7]
void set(uint8 *map, int i, int j, int s) {
  int mask = (1<<j); // the bit we are going to set
  if (s) {
    map[i] |= mask; // set the position to true
  } else {
    map[i] &= (mask ^ 0xff); // clear the position
  }
}

void print(int idx, uint8 *map) {
  Serial.printf("=== iteration %d ===\n",idx);
  for (int i=0; i<8; i++) {
    for (int j=0; j<8; j++) {
      Serial.printf("%s",(get(map,7-i,j)? "x": "."));
    }
    Serial.printf("\n");
  }
  Serial.printf("\n");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");

  // loadState(current, state1);

  // initial load current into the display
  for(int i=0; i<8; i++)  {
    mled.disBuffer[i]=current[i];  //full screen
  }

  mled.display();
  delay(2000); // display the initial state
  mled.intensity = 5;
  print(count, current);
}

void loop() {

  // use bottom row for progress
  count++;
  Serial.printf("count=%d\n",count);
  // mled.disBuffer[0]=count;

  // if (count > 2) {
  //   exit(0);
  // }

  // copy current into previous
  loadState(previous,current);

  for(int i=0; i<8; i++){
     for(int j=0; j<8; j++) {

      int p = get(previous, i, j);
      int alive = 0;

      for (int k=-1; k<=1; k++) {
        for (int l=-1; l<=1; l++) {

          if (k==0 && l==0) {
            continue; // don't count ourselves
          }

          if (get(previous, i+k, j+l)) {
            alive++; // count alive neighbors
          }
        }
      }

      // Serial.printf("[%d,%d] is %s and has %d alive neighbors\n",i,j,(p?"alive":"dead"),alive);

      if (p) { // the current cell is alive
        switch (alive) {
          case 0:
          case 1:
            set(current, i, j, 0); // underpopulation
            Serial.printf("[%d,%d] dies from underpopulation (%d)\n",i,j, alive);
            break;
          case 2:
          case 3:
            set(current, i, j, 1); // just right, live on
            Serial.printf("[%d,%d] keeps on living (%d)\n", i,j,alive);
            break;
          default:
            set(current, i, j, 0); // overpopulation
            Serial.printf("[%d,%d] dies from overpopulation (%d)\n",i,j, alive);
        }
      } else { // the current cell is dead
        if (alive==3) {
          set(current, i, j, 1); // reproduction creates new cell
          Serial.printf("[%d,%d] created (%d)\n",i,j, alive);
        }
      }

    }
  }

  print(count,current);

  // load current into the display
  for(int i=0; i<8; i++)  {
    mled.disBuffer[i]=current[i];  //full screen
  }
  mled.display();
  delay(DELAY);
}

