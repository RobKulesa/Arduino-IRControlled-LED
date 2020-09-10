#include <IRremote.h>

#define GREENPIN 5
#define REDPIN 6
#define BLUEPIN 9
#define WHITEPIN 10
#define RECV_PIN 12

int colMAX = 256; int fadeSpeed = 5;
int red = 0; int green = 0; int blue = 0; int white = 0;

IRrecv irrecv(RECV_PIN);
decode_results results;
double lightCurve[100];

void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  pinMode(WHITEPIN, OUTPUT);
  pinMode(RECV_PIN, INPUT);
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);
  for (int i = 0; i < 100; ++i) lightCurve[i] = min(pow((double) (i + 15) / 100.0, 2), 0.99);

}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    switch (results.value) {
      case 0xA10C840B: //play
        fade();
        break;
      case 0xA10C640B: //circle
        fade2();
        break;
      case 0xA10C220D: //ratio
        if (red + 5 > 256) setRed(0);
        else setRed(red + 5);
        break;
      case 0xA10CA20D: //exit
        if (green + 5 > 256) setGreen(0);
        else setGreen(green + 5);
        break;
      case 0xA10C620D: //list
        if (blue + 5 > 256) setBlue(0);
        else setBlue(blue + 5);
        break;
      case 0xA10C1807: //red
        Serial.println("white");
        if (white + 5 > 256) setWhite(0);
        else setWhite(white + 5);
        break;
      case 0xA10C4C03: //clear
        setColor(0, 0, 0, 0);
        break;
      case 0xA10C8807: //live tv
        setRed(inputIntens());
        break;
      case 0xA10CC807: //info
        setGreen(inputIntens());
        break;
      case 0xA10C6C03: //guide
        setBlue(inputIntens());
        break;
      case 0xA10C5807: //green
        setWhite(inputIntens());
        break;
      case 0xE0E0F00F: //mute
        colMAX = inputIntens();
        break;
      case 0xA10C040B: //record
        fadeSpeed = inputIntens();
        break;
      default:
        break;
    }
    irrecv.resume();
  }
}

void setColor(int r, int g, int b, int w) {
  setRed(r);
  setGreen(g);
  setBlue(b);
  setWhite(w);
}

void setRed(int r) {
  red = r;
  analogWrite(REDPIN, r);
}

void setGreen(int g) {
  green = g;
  analogWrite(GREENPIN, g);
}

void setBlue(int b) {
  blue = b;
  analogWrite(BLUEPIN, b);
}

void setWhite(int w) {
  white = w;
  analogWrite(WHITEPIN, w);
}

int inputIntens() {
  int val = 0;
  int multiplier = 100;
  while (multiplier != 0) {
    irrecv.resume();
    while (!irrecv.decode(&results)) {}
    Serial.println(results.value, HEX);
    switch (results.value) {
      case 0xA10C8C03: //0
        val += 0 * multiplier;
        break;
      case 0xA10C140B: //1
        val += 1 * multiplier;
        break;
      case 0xA10C940B: //2
        val += 2 * multiplier;
        break;
      case 0xA10C540B: //3
        val += 3 * multiplier;
        break;
      case 0xA10CD40B: //4
        val += 4 * multiplier;
        break;
      case 0xA10C340B: //5
        val += 5 * multiplier;
        break;
      case 0xA10CB40B: //6
        val += 6 * multiplier;
        break;
      case 0xA10C740B: //7
        val += 7 * multiplier;
        break;
      case 0xA10CF40B: //8
        val += 8 * multiplier;
        break;
      case 0xA10C0C03: //9
        val += 9 * multiplier;
        break;
      default:
        multiplier *= 10;
        break;
    }
    multiplier /= 10;
  }
  irrecv.resume();
  return val;
}

bool checkInput() {
  bool breakout = false;
  if (irrecv.decode(&results)) {
    switch (results.value) {
      case 0xA10CC40B: //pause
        breakout = true;
        break;
      case 0xA10C240B: //ff
        fadeSpeed /= 1.3;
        break;
      case 0xA10C440B: //rewind
        fadeSpeed = max(fadeSpeed * 1.3, 4);
        break;
      case 0xE0E0E01F: //vol up
        if (colMAX + 5 > 256) colMAX = 256;
        else colMAX += 5;
        break;
      case 0xE0E0D02F: //vol dwn
        if (colMAX - 5 < 0) colMAX = 0;
        else colMAX -= 5;
        break;
      case 0xE0E0F00F: //mute
        colMAX = inputIntens();
        break;
      case 0xA10C040B: //record
        fadeSpeed = inputIntens();
        break;
      case 0xA10C4C03: //clear
        breakout = true;
        setColor(0, 0, 0, 0);
        break;
    }
    irrecv.resume();
  }
  return breakout;
}

void fade() {
  setWhite(0);
  setBlue(255);
  bool breakout = false;
  while (!breakout) {
    for (red = 0; red < colMAX; red++) {
      analogWrite(REDPIN, red);
      breakout = checkInput();
      if (breakout) break;
      delay(fadeSpeed);
    }
    if (breakout) break;
    for (blue = colMAX - 1; blue > 0; blue--) {
      analogWrite(BLUEPIN, blue);
      breakout = checkInput();
      if (breakout) break;
      delay(fadeSpeed);
    }
    if (breakout) break;
    for (green = 0; green < colMAX; green++) {
      analogWrite(GREENPIN, green);
      breakout = checkInput();
      if (breakout) break;
      delay(fadeSpeed);
    }
    if (breakout) break;
    for (red = colMAX - 1; red > 0; red--) {
      analogWrite(REDPIN, red);
      breakout = checkInput();
      if (breakout) break;
      delay(fadeSpeed);
    }
    if (breakout) break;
    for (blue = 0; blue < colMAX; blue++) {
      analogWrite(BLUEPIN, blue);
      breakout = checkInput();
      if (breakout) break;
      delay(fadeSpeed);
    }
    if (breakout) break;
    for (green = colMAX - 1; green > 0; green--) {
      analogWrite(GREENPIN, green);
      breakout = checkInput();
      if (breakout) break;
      delay(fadeSpeed);
    }
    if (breakout) break;
  }
}

void fade2() {
  int oR = red; int oG = green; int oB = blue; int oW = white;
  bool breakout = false;
  while (!breakout) {
    for (int i = 99; i >= 0; --i) {
      breakout = checkInput();
      if (breakout) break;
      setColor(lightCurve[i]* oR, lightCurve[i] * oG, lightCurve[i] * oB, lightCurve[i] * oW);
      Serial.println(red);
      delay(fadeSpeed);
    }
    if (breakout) break;
    for (int i = 0; i < 100; ++i) {
      breakout = checkInput();
      if (breakout) break;
      setColor(lightCurve[i]* oR, lightCurve[i] * oG, lightCurve[i] * oB, lightCurve[i] * oW);
      Serial.println(red);
      delay(fadeSpeed);
    }
  }
}
