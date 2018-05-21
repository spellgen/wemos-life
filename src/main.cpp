#include <Arduino.h>
#include <WEMOS_Matrix_LED.h>

MLED mled(5); //set intensity=5

uint8 state1[8] = {0xe7, 0xff, 0xff, 0x81, 0x81, 0x81, 0x81, 0x71};

uint8 current[8];
uint8 previous[8];

int loadState(uint8 *to, uint8 *from) {
    for(int i=0;i<8; i++) {
        to[i]=from[i];
    }
    return 0;
}

// return the state of map[i,j]
int get(uint8 *map, int i, int j) {
  int row = map[i%8];
  int col = j%8;
  return row & (1<<col);
}

// set the state of map[i,j] to s
void set(uint8 *map, int i, int j, int s) {
  int row = map[i%8];
  int col = j%8;
  int mask = (1<<col); // the bit we are going to set
  if (s) {
    row |= mask; // set the position to true
  } else {
    row &= !mask; // clear the position
  }
}

void setup() {
  Serial.begin(115200);
  // Serial.println("Starting");

  loadState(current, state1);
  // for(int i=1; i<8; i++)  {
  //   mled.disBuffer[i]=current[i];  //full screen
  // }

  mled.clear();
  delay(5000);
}

uint8 count = 1;

void loop() {

  // use bottom row for progress
  count = count<<1;
  mled.disBuffer[0]=count;

  // load current into the display
  for(int i=1; i<8; i++)  {
    mled.disBuffer[i]=current[i];  //full screen
  }
  mled.display();

  // copy current into previous
  loadState(previous,current);

  for(int i=0;i<8;i++){
     for(int j=0;i<8;j++) {

      int p = get(previous, i,j);
      int alive = 0;

      for (int k=-1; k<2; k++) {
        for (int l=-1; l<2; l++) {

          if (k==0 && l==0) {
            continue;
          }
          if (get(previous,i+k,j+l)) {
            alive++; // count alive neighbors
          }
          if (p) { // the current cell is alive
            switch (alive) {
              case 0:
              case 1:
                set(current,i,j,0); // underpopulation
                break;
              case 2:
              case 3:
                set(current,i,j,1); // just right, live on
                break;
              default:
                set(current,i,j,0); // overpopulation
            }
          } else { // the current cell is dead
            if (alive==3) {
              set(current, i,j,1); // reproduction creates new cell
            }
          }
        }
      }
    }
  }

  delay(1000);
}

