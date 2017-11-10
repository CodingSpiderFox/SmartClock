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

extern uint8_t SmallFont[];
UTFT myGLCD(ILI9341_16,38,39,40,41);

String serialData;
char* files[]={"Cld136.raw", "mail136.raw"}; // 320x240
int picsize_x, picsize_y;
unsigned long unixTime;
char* weather, statusIcon;
int controlsDimLevel, sleepModeDimLevel;
int controlsDimLevelBeforeStateChange; //dim level before sleep mode or light was turned on
bool rgbLEDModeCustom[2];
char rgbLEDColors[2]; //
bool sleepModeEnabled;
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

void toggleSleepMode() {
  if(!sleepModeEnabled) { //enable sleep mode
    controlsDimLevelBeforeStateChange = controlsDimLevel;
    controlsDimLevel = sleepModeDimLevel;
    applyControlsBrightness();
  }
  else  {
    controlsDimLevel = controlsDimLevelBeforeStateChange;
    applyControlsBrightness();
  }
}

void processCommand(String commandString) {
  if(commandString.startsWith("t")) { //unixtime
    unixTime = commandString.substring(2).toInt();
  }
  else if(commandString.startsWith("w")) { // weather icon filename
    weather = commandString.substring(2).c_str();
  }
  else if(commandString.startsWith("i")) { //status icon filename
    statusIcon = commandString.substring(2).c_str();
  }
  else if(commandString.startsWith("sb")) { //(sb=sleepmodeBrightness)controls brightness while sleep mode
    sleepModeDimLevel = commandString.substring(2).toInt();
  }
  else if(commandString.startsWith("st")) { //(st=sleepModeToggle)enable/disable sleep mode
    toggleSleepMode();
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

  myGLCD.clrScr();


  delay(1000);
}
