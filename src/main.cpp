#include <Arduino.h>
#include <WEMOS_Matrix_LED.h>

MLED mled(5); //set intensity=5

uint8 state1[8] = {0x00, 0x00, 0x1c, 0x10, 0x08, 0x00, 0x0e, 0x00};
uint8 state2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x04, 0x02};
uint8 state3[8] = {0x00, 0x00, 0x00, 0x00, 0xe2, 0x03, 0x40, 0x00};

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
  int row = map[(i+8)%8];
  int col = (j+8)%8;
  return row & (1<<col);
}

// set the state of map[i,j] to s
void set(uint8 *map, int i, int j, int s) {
  int row = map[i%8];
  int col = j%8;
  int mask = (1<<col); // the bit we are going to set
  if (s) {
    map[i%8] = row | mask; // set the position to true
  } else {
    map[i%8] = row & !mask; // clear the position
  }
}

void setup() {
  Serial.begin(115200);
  // delay(4000);
  Serial.println("Starting");

  loadState(current, state3);
  // for(int i=1; i<8; i++)  {
  //   mled.disBuffer[i]=current[i];  //full screen
  // }

  // set(current,0,4,1);
  // Serial.printf("A: [0,4]=%d\n",get(current,0,4));
  // Serial.printf("A: [8,4]=%d\n",get(current,8,4));
  // set(current,0,4,0);
  // Serial.printf("B: [0,4]=%d\n",get(current,0,4));
  // set(current,0,4,1);
  // Serial.printf("C: [0,4]=%d\n",get(current,0,4));

  // mled.clear();
}

uint8 count = 1;

void loop() {

  // use bottom row for progress
  count++;
  Serial.printf("count=%d\n",count);
  // mled.disBuffer[0]=count;

  // if (count > 2) {
  //   exit(0);
  // }

  // load current into the display
  for(int i=0; i<8; i++)  {
    mled.disBuffer[i]=current[i];  //full screen
  }
  mled.display();

  delay(500);

  // copy current into previous
  loadState(previous,current);

  for(int i=0;i<8;i++){
     for(int j=0;j<8;j++) {

      int p = get(previous, i,j);
      int alive = 0;

      for (int k=-1; k<=1; k++) {
        for (int l=-1; l<=1; l++) {

          if (k==0 && l==0) {
            continue; // don't count ourselves
          }

          if (get(previous,i+k,j+l)) {
            alive++; // count alive neighbors
          }
        }
      }

      // Serial.printf("[%d,%d] is %s and has %d alive neighbors\n",i,j,(p?"alive":"dead"),alive);

      if (p) { // the current cell is alive
        switch (alive) {
          case 0:
          case 1:
            set(current,i,j,0); // underpopulation
            Serial.printf("[%d,%d] dies from underpopulation\n",i,j);
            break;
          case 2:
          case 3:
            set(current,i,j,1); // just right, live on
            Serial.printf("[%d,%d] keeps on living\n", i,j);
            break;
          default:
            set(current,i,j,0); // overpopulation
            Serial.printf("[%d,%d] dies from overpopulation\n",i,j);
        }
      } else { // the current cell is dead
        if (alive==3) {
          set(current, i,j,1); // reproduction creates new cell
          Serial.printf("[%d,%d] created\n",i,j);
        }
      }

    }
  }
}

