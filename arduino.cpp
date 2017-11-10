// UTFT_Demo_320x240
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// This program is a demo of how to use most of the functions
// of the library with a supported display modules.
//
// This demo was made for modules with a screen resolution
// of 320x240 pixels.
//
// This program requires the UTFT library.
//

#include <UTFT.h>
#include <NeoPixelBus.h>

#define RGB_LED_1_R_PIN 8
#define RGB_LED_1_G_PIN 9
#define RGB_LED_1_B_PIN 10
#define RGB_LED_2_R_PIN 11
#define RGB_LED_2_G_PIN 12
#define RGB_LED_2_B_PIN 13

#define PREDEFINED_COLOR_RED 'r'
#define PREDEFINED_COLOR_GREEN 'g'
#define PREDEFINED_COLOR_BLUE 'b'
#define PREDEFINED_COLOR_YELLOW 'y'
#define PREDEFINED_COLOR_ORANGE 'o'
#define PREDEFINED_COLOR_CYAN 'c'


// Declare which fonts we will be using
extern uint8_t SmallFont[];

// Set the pins to the correct ones for your development shield
// ------------------------------------------------------------
// Arduino Uno / 2009:
// -------------------
// Standard Arduino Uno/2009 shield            : <display model>,A5,A4,A3,A2
// DisplayModule Arduino Uno TFT shield        : <display model>,A5,A4,A3,A2
//
// Arduino Mega:
// -------------------
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41
// CTE TFT LCD/SD Shield for Arduino Mega      : <display model>,38,39,40,41
//
// Remember to change the model parameter to suit your display module!
UTFT myGLCD(ILI9341_16,38,39,40,41);

String serialData;
char* files[]={"Cld136.raw", "mail136.raw"}; // 320x240
int picsize_x, picsize_y;
unsigned long unixTime;
char* weather, statusIcon;
int controlsDimLevel;
bool rgbLEDModeCustom[2];
char rgbLEDColors[2]; //
byte rgbLEDs[7]; //0-5 are RGB1 - RGB2, 6 is the overall LED brightness for single colour LEDs
byte orange[8][3] = {{255,60,0}, {210,48,0}, {112,31,0}, {58,19,0}, {28,8,0}, {20,5,0}, {10,2,0}, {8,2,0}};
byte yellow[8][3] = {{255,215,0}, {200,160,0}, {98,80,0}, {60,50,0}, {20,20,0}, {10,10,0}, {5,5,0}, {2,2,0}};
byte cyan[8][3] = {{0,250,210}, {0,195,155}, {0,96,77}, {0,58,47}, {0,20,20}, {0,10,10}, {0,5,5}, {0,2,2}};
byte red[8][3] = {{255,0,0}, {150,0,0}, {100,0,0}, {63,0,0}, {20,0,0}, {8,0,0}, {4,0,0}, {2,0,0}};
byte green[8][3] = {{0,255,0}, {0,150,0}, {0,100,0}, {0,63,0}, {0,20,0}, {0,8,0}, {0,4,0}, {0,2,0}};
byte blue[8][3] = {{0,0,255}, {0,0,150}, {0,0,100}, {0,0,63}, {0,0,20}, {0,0,8}, {0,0,4}, {0,0,2}};

void resetLCD() {
  myGLCD.setBackColor(0,0,0);
  myGLCD.clrScr();
}


void setup()
{
  randomSeed(analogRead(0));
  // Setup the LCD
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);
  resetLCD();
  // Serial.println(file.initFAT());
  picsize_x=myGLCD.getDisplayXSize();
  picsize_y=myGLCD.getDisplayYSize();
  Serial3.begin(115200);
  Serial.begin(115200);
}

void setRGBLEDPredefColorByString(String commandString, int rgbLED_ID) {

}

void setRGBLEDPredefColor(byte rgbValues[], int rgbLED_ID) {
  if(rgbLED_ID == 0) {
    digitalWrite(rgbValues[0], RGB_LED_1_R_PIN);
    digitalWrite(rgbValues[1], RGB_LED_1_G_PIN);
    digitalWrite(rgbValues[2], RGB_LED_1_B_PIN);
  }
  else if(rgbLED_ID == 1) {
    digitalWrite(rgbValues[0], RGB_LED_2_R_PIN);
    digitalWrite(rgbValues[1], RGB_LED_2_G_PIN);
    digitalWrite(rgbValues[2], RGB_LED_2_B_PIN);
  }
}

void applyControlsBrightness() {
  for(int i = 0; i < sizeof(rgbLEDs)/sizeof(rgbLEDs[0]); i++) {
    if(rgbLEDModeCustom[i]) //for later use, LEDs could also be set to custom colors, dimming has to be calculated {
    {}
    else {
      switch (rgbLEDColors[i]) {
        case PREDEFINED_COLOR_RED:
          setRGBLEDPredefColor(red[controlsDimLevel], i);
        case PREDEFINED_COLOR_GREEN:
          setRGBLEDPredefColor(green[controlsDimLevel], i);
        case PREDEFINED_COLOR_BLUE:
          setRGBLEDPredefColor(blue[controlsDimLevel], i);
        case PREDEFINED_COLOR_YELLOW:
          setRGBLEDPredefColor(yellow[controlsDimLevel], i);
        case PREDEFINED_COLOR_ORANGE:
          setRGBLEDPredefColor(orange[controlsDimLevel], i);
        case PREDEFINED_COLOR_CYAN:
          setRGBLEDPredefColor(cyan[controlsDimLevel], i);
        break;
      }
    }
  }
}

void readSwSer() {
  serialData = "";
  Serial.println(Serial3.available());
  while (Serial3.available() > 0) {
    serialData += Serial3.read();
  }
}

/*RgbColor getRgbColor(byte[] rgbValues) {
  if(sizeof(rgbValues) > 0 && sizeof(rgbValues)) < 3) {

  }
}
*/

void processCommand(String commandString) {
  if(commandString.startsWith("t")) { //unixtime
    unixTime = commandString.substring(1).toInt();
  }
  else if(commandString.startsWith("w")) { // weather icon filename
    weather = commandString.substring(1).c_str();
  }
  else if(commandString.startsWith("s")) { //status icon filename
    statusIcon = commandString.substring(1).c_str();
  }
  else if(commandString.startsWith("b+")) { //controls (LCD, RGB LED Buttons) brightness +
    if(controlsDimLevel > 0) {
      controlsDimLevel--;
    }
    else {
      controlsDimLevel = 0;
    }
    applyControlsBrightness();
  }
  else if(commandString.startsWith("b-")) { //controls (LCD, RGB LED Buttons) brightness -
    if(controlsDimLevel < 8) {
      controlsDimLevel++;
    }
    else {
      controlsDimLevel = 8;
    }
    applyControlsBrightness();
  }
  else if(commandString.startsWith("RGB1PredefColor")) { //RGB LED1 Color
    setRGBLEDPredefColorByString(commandString, 0);
  }
  else if(commandString.startsWith("RGB2PredefColor")) { //RGB LED1 Color
    rgbLEDs[0] = commandString.substring(4,7).toInt();
    rgbLEDs[1] = commandString.substring(8,11).toInt();
    rgbLEDs[2] = commandString.substring(12,15).toInt();
  }
  else if(commandString.startsWith("RGB1")) { //RGB LED1 Color
    rgbLEDs[0] = commandString.substring(4,7).toInt();
    rgbLEDs[1] = commandString.substring(8,11).toInt();
    rgbLEDs[2] = commandString.substring(12,15).toInt();
  }
  else if(commandString.startsWith("RGB1")) { //RGB LED1 Color
    rgbLEDs[0] = commandString.substring(4,7).toInt();
    rgbLEDs[1] = commandString.substring(8,11).toInt();
    rgbLEDs[2] = commandString.substring(12,15).toInt();
  }
  else if(commandString.startsWith("RGB2")) { //RGB LED2 Color
    rgbLEDs[3] = commandString.substring(4,7).toInt();
    rgbLEDs[4] = commandString.substring(8,11).toInt();
    rgbLEDs[5] = commandString.substring(12,15).toInt();
  }
}

void loop()
{
  readSwSer();
  int buf[318];
  int x, x2;
  int y, y2;
  int r;
  int initialMillis = millis();

  Serial.println(Serial3.available());
  while (Serial3.available() > 0) {
    processCommand(Serial3.readString());
  }

  // Clear the screen and draw the frame
  myGLCD.clrScr();

  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, 319, 13);
  myGLCD.setColor(64, 64, 64);
  myGLCD.fillRect(0, 226, 319, 239);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.print("* Universal Color TFT Display Library *", CENTER, 1);
  myGLCD.setBackColor(64, 64, 64);
  myGLCD.setColor(255,255,0);
  myGLCD.print("<http://www.RinkyDinkElectronics.com/>", CENTER, 227);

  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRect(0, 14, 319, 225);

// Draw crosshairs
  myGLCD.setColor(0, 0, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.drawLine(159, 15, 159, 224);
  myGLCD.drawLine(1, 119, 318, 119);
  for (int i=9; i<310; i+=10) {
    myGLCD.drawLine(i, 117, i, 121);
  }
  for (int i=19; i<220; i+=10) {
    myGLCD.drawLine(157, i, 161, i);
  }
// Draw sin-, cos- and tan-lines
  myGLCD.setColor(0,255,255);
  myGLCD.print("Sin", 5, 15);
  for (int i=1; i<318; i++)
  {
    myGLCD.drawPixel(i,119+(sin(((i*1.13)*3.14)/180)*95));
  }
  myGLCD.setColor(255,0,0);
  myGLCD.print("Cos", 5, 27);
  for (int i=1; i<318; i++)
  {
    myGLCD.drawPixel(i,119+(cos(((i*1.13)*3.14)/180)*95));
  }

  myGLCD.setColor(255,255,0);
  myGLCD.print("Tan", 5, 39);
  for (int i=1; i<318; i++)
  {
    myGLCD.drawPixel(i,119+(tan(((i*1.13)*3.14)/180)));
  }


  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,318,224);
  myGLCD.setColor(0, 0, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.drawLine(159, 15, 159, 224);
  myGLCD.drawLine(1, 119, 318, 119);

// Draw a moving sinewave
  x=1;
  for (int i=1; i<(318*20); i++)
  {
    x++;
    if (x==319) {
      x=1;
 }
    if (i>319) {
      if ((x==159)||(buf[x-1]==119)) {
        myGLCD.setColor(0,0,255);
    }
      else {
        myGLCD.setColor(0,0,0);
      }
      myGLCD.drawPixel(x,buf[x-1]);
    }
    myGLCD.setColor(0,255,255);
    y=119+(sin(((i*1.1)*3.14)/180)*(90-(i / 100)));
    myGLCD.drawPixel(x,y);
    buf[x-1]=y;
  }


  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,318,224);

// Draw some filled rectangles
  for (int i=1; i<6; i++)
  {
    switch (i)
    {
      case 1:
        myGLCD.setColor(255,0,255);
        break;
      case 2:
        myGLCD.setColor(255,0,0);
        break;
      case 3:
        myGLCD.setColor(0,255,0);
        break;
      case 4:
        myGLCD.setColor(0,0,255);
        break;
      case 5:
        myGLCD.setColor(255,255,0);
        break;
    }
    myGLCD.fillRect(70+(i*20), 30+(i*20), 130+(i*20), 90+(i*20));
  }


  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,318,224);

// Draw some filled, rounded rectangles
  for (int i=1; i<6; i++)
  {
    switch (i)
    {
      case 1:
        myGLCD.setColor(255,0,255);
        break;
      case 2:
        myGLCD.setColor(255,0,0);
        break;
      case 3:
        myGLCD.setColor(0,255,0);
        break;
      case 4:
        myGLCD.setColor(0,0,255);
        break;
      case 5:
        myGLCD.setColor(255,255,0);
        break;
    }
    myGLCD.fillRoundRect(190-(i*20), 30+(i*20), 250-(i*20), 90+(i*20));
  }

  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,318,224);

// Draw some filled circles
  for (int i=1; i<6; i++)
  {
    switch (i)
    {
      case 1:
        myGLCD.setColor(255,0,255);
        break;
      case 2:
        myGLCD.setColor(255,0,0);
        break;
      case 3:
        myGLCD.setColor(0,255,0);
        break;
      case 4:
        myGLCD.setColor(0,0,255);
        break;
      case 5:
        myGLCD.setColor(255,255,0);
        break;
    }
    myGLCD.fillCircle(100+(i*20),60+(i*20), 30);
  }

  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,318,224);

// Draw some lines in a pattern
  myGLCD.setColor (255,0,0);
  for (int i=15; i<224; i+=5)
  {
    myGLCD.drawLine(1, i, (i*1.44)-10, 224);
  }
  myGLCD.setColor (255,0,0);
  for (int i=224; i>15; i-=5)
  {
    myGLCD.drawLine(318, i, (i*1.44)-11, 15);
  }
  myGLCD.setColor (0,255,255);
  for (int i=224; i>15; i-=5)
  {
    myGLCD.drawLine(1, i, 331-(i*1.44), 15);
  }
  myGLCD.setColor (0,255,255);
  for (int i=15; i<224; i+=5)
  {
    myGLCD.drawLine(318, i, 330-(i*1.44), 224);
  }

  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,318,224);

// Draw some random circles
  for (int i=0; i<100; i++)
  {
    myGLCD.setColor(random(255), random(255), random(255));
    x=32+random(256);
    y=45+random(146);
    r=random(30);
    myGLCD.drawCircle(x, y, r);
  }


  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,318,224);

// Draw some random rectangles
  for (int i=0; i<100; i++)
  {
    myGLCD.setColor(random(255), random(255), random(255));
    x=2+random(316);
    y=16+random(207);
    x2=2+random(316);
    y2=16+random(207);
    myGLCD.drawRect(x, y, x2, y2);
  }


  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,318,224);

// Draw some random rounded rectangles
  for (int i=0; i<100; i++)
  {
    myGLCD.setColor(random(255), random(255), random(255));
    x=2+random(316);
    y=16+random(207);
    x2=2+random(316);
    y2=16+random(207);
    myGLCD.drawRoundRect(x, y, x2, y2);
  }


  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,318,224);

  for (int i=0; i<100; i++)
  {
    myGLCD.setColor(random(255), random(255), random(255));
    x=2+random(316);
    y=16+random(209);
    x2=2+random(316);
    y2=16+random(209);
    myGLCD.drawLine(x, y, x2, y2);
  }


  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,318,224);
  // draw stars

  for (int i=0; i<10000; i++)
  {
    myGLCD.setColor(random(255), random(255), random(255));
    myGLCD.drawPixel(2+random(316), 16+random(209));
  }


  myGLCD.fillScr(0, 0, 255);
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRoundRect(80, 70, 239, 169);

  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.print("That's it!", CENTER, 93);
  myGLCD.print("Restarting in a", CENTER, 119);
  myGLCD.print("few seconds...", CENTER, 132);

  myGLCD.setColor(0, 255, 0);
  myGLCD.setBackColor(0, 0, 255);
  myGLCD.print("Runtime: (msecs)", CENTER, 210);
  myGLCD.printNumI(millis()-initialMillis, CENTER, 225);

  delay(1000);
}
