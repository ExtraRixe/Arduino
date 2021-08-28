/* 
 *  Forked from jacksonspry/STACKER 
 *  Fixed Overhang issue
 *  Added Win and Lose message
 *  Added audio
 *  Added Serial Debug
 *  Easter egg if pressed for 3 seconds, and plays a tune
*/

#include <LedControl.h>
#include "binary.h"

LedControl lc = LedControl(7, 6, 5, 4); //DIN, CLK, CS
int i = 0, led = 2, SW_pin = 3, a = 0, x = 7, y = 6, butpress = 1, dir = 0,
    sto_x = 0, leh = 4, d = 0, sta = 1, rst = 4, hold = 0, speaker = A5;
void setup() {
  while (i < 4) { //Loop to initiate and clear all displays
    lc.shutdown(i, false);
    lc.setIntensity(i, 6);
    lc.clearDisplay(i);
    i++;
  }
  i = 0;
  pinMode(SW_pin, INPUT); //button pin
  digitalWrite(rst, HIGH);
  digitalWrite(led, HIGH);
  delay(200);
  pinMode(rst, OUTPUT);
  Serial.begin(9600);
}
void checkfall() {
  if (x != sto_x) { //If x is not equal to the previous x value

    for (i = 0; i <= sto_x - leh; i++) {//erase overhang from left
      lc.setLed(a, i, y + 2, 0);
      lc.setLed(a, i, y + 3, 0);
    }

    for (i = sto_x + 1; i <= 7; i++) {//erase overhang from right
      lc.setLed(a, i, y + 2, 0);
      lc.setLed(a, i, y + 3, 0);
    }

    leh = leh - abs(x - sto_x); //subtract the difference in x from the length
    if (x > sto_x) { //if x overhangs to the left
      x = x - abs(x - sto_x); //shift x to be on the tower
      sto_x = x; //set previous x value
    }
  }
  Serial.print("Sto_x:");
  Serial.print(sto_x);
  Serial.print(", x:");
  Serial.print(x);
  Serial.print(", leh:");
  Serial.print(leh);
  Serial.print(", Y:");
  Serial.print(y);
  Serial.print(", A:");
  Serial.print(a);
  Serial.println();
  sto_x = x; //set previous x value



  if (leh <= 0) { //Lose
    delay(150);
    lcd_lose();
    delay(1500);
    digitalWrite(rst, LOW);
  }

  if (leh > 0 && y == -2 && a == 3) {
    delay(150);
    lcd_win();
    delay(3000);
    digitalWrite(rst, LOW);
  }
}
void loop() {
  while (digitalRead(SW_pin) == HIGH) { //while the button is not pressed
    delay(96 - 2 * ((a * 8) + (8 - y))); //variable delay to make it speed up the higher you go
    butpress = 1; //button lock variable on
    if (x < 0 || x > 6 + leh) { //if the platform is off the screen
      dir = abs(dir - 1); //switch the direction
      tone(speaker, 500, 25);
    }
    if (dir == 0) {
      x--; //incriment the platform right if the dir is 0
      lc.setLed(a, x + 1, y, 0); //erase the platform trail
      lc.setLed(a, x + 1, y + 1, 0);
    }
    else {
      x++; //incriment the platform left if the dir is 1
      lc.setLed(a, x - leh, y, 0); //erase the platform trail
      lc.setLed(a, x - leh, y + 1, 0);
    }
    while (d < leh) { //loop to draw the platform based off the length
      lc.setLed(a, x - d, y, 1); //top pixel
      lc.setLed(a, x - d, y + 1, 1); //bottom pixel
      d++;
    }
    d = 0;
  }

  if (key_read() == 2 && sta == 1) {
    //Serial.println("Long");
    lcd_0728();
    b2g();
    lcd_clear();
  } else if (key_read() == 1) {
    //Serial.println("Short");
    if (sta == 1) { //if its the first press of the game)
      sta = 0;
      sto_x = x;
    }
    y = y - 2; //move y up two pixels
    butpress = 0;
    tone(speaker, 450, 25);
    delay(50);
    tone(speaker, 500, 25);
    checkfall(); //check for misalignment
  }

  if (y < 0) { //if y goes off the screen, warp to next sceen
    a++; //incriment screen address
    y = 6; //set y to bottom value
  }
}

int key_read()  {
  hold = 0;
  while (digitalRead(SW_pin) == LOW) {
    hold++;
    Serial.println(hold);//increment count if key is continuously pressed
  }
  if (hold > 200
      && hold < 500) {
    return 2;
  } else if (hold > 500) {
    digitalWrite(rst, LOW);
  }
  else {
    return 1;
  }
}

void lcd_0728() {
  const byte NUM[][8] = {
    {
      B00000000,
      B00000000,
      B00111100,
      B01000010,
      B01000010,
      B00111100,
      B00000000,
      B00000000
    }, {
      B00000000,
      B00000000,
      B01100000,
      B01010000,
      B01001110,
      B01000000,
      B00000000,
      B00000000
    }, {
      B00000000,
      B00000000,
      B00110010,
      B01001010,
      B01001010,
      B00100110,
      B00000000,
      B00000000
    }, {
      B00000000,
      B00000000,
      B00101100,
      B01010010,
      B01010010,
      B00101100,
      B00000000,
      B00000000
    }
  };
  for (int i = 3; i >= 0; i--) {
    for (int j = 0; j <= 7; j++) {
      lc.setRow(i, j, NUM[3 - i][j]);
      delay(50);
    }
  }
}
void lcd_lose() {
  const byte IMAGES[][8] = {
    {
      B00000000,
      B00000000,
      B00000010,
      B00000010,
      B00000010,
      B01111110,
      B00000000,
      B00000000
    }, {
      B00000000,
      B00000000,
      B00111100,
      B01000010,
      B01000010,
      B00111100,
      B00000000,
      B00000000
    }, {
      B00000000,
      B00000000,
      B00100100,
      B01001010,
      B01010010,
      B00100100,
      B00000000,
      B00000000
    }, {
      B00000000,
      B00000000,
      B01000010,
      B01010010,
      B01010010,
      B01111110,
      B00000000,
      B00000000
    }
  };

  for (int i = 3; i >= 0; i--) {
    for (int j = 0; j <= 7; j++) {
      lc.setRow(i, j, IMAGES[3 - i][j]);
      delay(25);
    }
  }
}

void lcd_win() {
  const byte IMAGES[][8] = {
    {
      B00000000,
      B00000000,
      B01111110,
      B00000100,
      B00001000,
      B00000100,
      B01111110,
      B00000000
    }, {
      B00000000,
      B00000000,
      B00000000,
      B01000010,
      B01111110,
      B01000010,
      B00000000,
      B00000000
    }, {
      B00000000,
      B00000000,
      B01111100,
      B00001000,
      B00010000,
      B00100000,
      B01111100,
      B00000000
    }, {
      B00000000,
      B00001000,
      B01100100,
      B00000100,
      B00000100,
      B01100100,
      B00001000,
      B00000000
    }
  };

  for (int i = 3; i >= 0; i--) {
    for (int j = 0; j <= 7; j++) {
      lc.setRow(i, j, IMAGES[3 - i][j]);
      delay(25);
    }
  }
}

void lcd_clear() {
  for (int i = 3; i >= 0; i--) {
    for (int j = 0; j <= 7; j++) {
      lc.setRow(i, j, B00000000);
      delay(50);
    }
  }
}

void b2g() {
  tone(speaker, 293, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 261, 30.6817875);
  delay(34.090875);
  delay(272.727);
  tone(speaker, 391, 30.6817875);
  delay(34.090875);
  delay(443.181375);
  tone(speaker, 293, 30.6817875);
  delay(34.090875);
  delay(170.454375);
  tone(speaker, 246, 30.6817875);
  delay(34.090875);
  delay(306.817875);
  tone(speaker, 329, 30.6817875);
  delay(34.090875);
  delay(545.454);
  tone(speaker, 329, 30.6817875);
  delay(34.090875);
  delay(170.454375);
  tone(speaker, 246, 30.6817875);
  delay(34.090875);
  delay(340.90875);
  tone(speaker, 391, 30.6817875);
  delay(34.090875);
  delay(477.27225);
  tone(speaker, 391, 30.6817875);
  delay(34.090875);
  delay(170.454375);
  tone(speaker, 293, 30.6817875);
  delay(34.090875);
  delay(340.90875);
  tone(speaker, 440, 30.6817875);
  delay(34.090875);
  delay(511.363125);
  tone(speaker, 440, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 329, 30.6817875);
  delay(34.090875);
  delay(340.90875);
  tone(speaker, 391, 30.6817875);
  delay(34.090875);
  delay(511.363125);
  tone(speaker, 391, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 293, 30.6817875);
  delay(34.090875);
  delay(1056.817125);
  tone(speaker, 523, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 523, 30.6817875);
  delay(34.090875);
  delay(170.454375);
  tone(speaker, 523, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 523, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 440, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 440, 30.6817875);
  delay(34.090875);
  tone(speaker, 440, 30.4089375);
  delay(170.454375);
  tone(speaker, 440, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 391, 30.6817875);
  delay(34.090875);
  delay(170.454375);
  tone(speaker, 391, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 349, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 349, 30.6817875);
  delay(34.090875);
  delay(170.454375);
  tone(speaker, 293, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 349, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 293, 30.6817875);
  delay(34.090875);
  delay(170.454375);
  tone(speaker, 391, 30.6817875);
  delay(34.090875);
  delay(306.817875);
  tone(speaker, 349, 30.6817875);
  delay(34.090875);
  delay(170.454375);
  tone(speaker, 349, 30.6817875);
  delay(34.090875);
  delay(306.817875);
  tone(speaker, 349, 7.670446875);
  delay(8.52271875);
  tone(speaker, 349, 23.011340625);
  delay(25.56815625);
  delay(204.54525);
  tone(speaker, 293, 30.6817875);
  delay(34.090875);
  delay(272.727);
  tone(speaker, 349, 30.6817875);
  delay(34.090875);
  delay(170.454375);
  tone(speaker, 466, 30.6817875);
  delay(34.090875);
  delay(306.817875);
  tone(speaker, 466, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 349, 30.6817875);
  delay(34.090875);
  delay(340.90875);
  tone(speaker, 293, 30.6817875);
  delay(34.090875);
  delay(170.454375);
  tone(speaker, 440, 30.6817875);
  delay(34.090875);
  delay(272.727);
  tone(speaker, 440, 30.6817875);
  delay(34.090875);
  delay(170.454375);
  tone(speaker, 391, 30.6817875);
  delay(34.090875);
  delay(306.817875);
  tone(speaker, 349, 30.6817875);
  delay(34.090875);
  delay(204.54525);
  tone(speaker, 466, 30.6817875);
  delay(34.090875);
  delay(272.727);
  tone(speaker, 466, 30.6817875);
  delay(34.090875);
  delay(170.454375);
  tone(speaker, 391, 30.6817875);
  delay(34.090875);
  delay(272.727);
  tone(speaker, 329, 30.6817875);
  delay(34.090875);
  delay(204.54525);
  tone(speaker, 523, 30.6817875);
  delay(34.090875);
  delay(306.817875);
  tone(speaker, 523, 30.6817875);
  delay(34.090875);
  delay(749.99925);
  tone(speaker, 391, 30.6817875);
  delay(34.090875);
  tone(speaker, 174, 30.9544125);
  delay(147.727125);
  delay(363.636);
  tone(speaker, 349, 30.6817875);
  delay(34.090875);
  delay(647.726625);
  tone(speaker, 261, 30.6817875);
  delay(34.090875);
  delay(102.272625);
  tone(speaker, 293, 30.6817875);
  delay(34.090875);
  delay(102.272625);
  tone(speaker, 261, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 293, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 293, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 261, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 391, 30.6817875);
  delay(34.090875);
  delay(340.90875);
  tone(speaker, 261, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 349, 30.6817875);
  delay(34.090875);
  delay(511.363125);
  tone(speaker, 349, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 233, 30.6817875);
  delay(34.090875);
  delay(170.454375);
  tone(speaker, 220, 30.6817875);
  delay(34.090875);
  delay(170.454375);
  tone(speaker, 293, 30.6817875);
  delay(34.090875);
  delay(272.727);
  tone(speaker, 349, 30.6817875);
  delay(34.090875);
  delay(204.54525);
  tone(speaker, 466, 30.6817875);
  delay(34.090875);
  delay(306.817875);
  tone(speaker, 293, 30.6817875);
  delay(34.090875);
  delay(170.454375);
  tone(speaker, 261, 30.6817875);
  delay(34.090875);
  delay(272.727);
  tone(speaker, 220, 30.6817875);
  delay(34.090875);
  delay(204.54525);
  tone(speaker, 440, 30.6817875);
  delay(34.090875);
  delay(272.727);
  tone(speaker, 349, 30.6817875);
  delay(34.090875);
  delay(204.54525);
  tone(speaker, 391, 30.6817875);
  delay(34.090875);
  delay(272.727);
  tone(speaker, 391, 30.6817875);
  delay(34.090875);
  delay(204.54525);
  tone(speaker, 466, 30.6817875);
  delay(34.090875);
  delay(306.817875);
  tone(speaker, 293, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 391, 30.6817875);
  delay(34.090875);
  delay(340.90875);
  tone(speaker, 349, 30.6817875);
  delay(34.090875);
  delay(136.3635);
  tone(speaker, 523, 30.6817875);
  delay(34.090875);
  delay(306.817875);
  tone(speaker, 523, 30.6817875);
  delay(34.090875);
}
