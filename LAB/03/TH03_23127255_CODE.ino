#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int potPin = A0;
const int lightPin = A1;
const int soilPin = A2;
const int btnPin = 2;
const int ledPin = 3;
const int servoPin = 10;

Servo fan;

int menuIndex = 0;
int lastMenuIndex = -1;
unsigned long lastBtn = 0;
bool inSubmenu = false;
int _lightVal = -1, _soilVal = -1;
int _brightness = -1;
int _angle = -1;

byte iconLight[8] = {
  B00100, B10101, B01110, B11111,
  B01110, B10101, B00100, B00000
};

byte iconSoil[8] = {
  B00100, B00100, B01010, B01010,
  B10001, B10001, B01110, B00000
};

byte iconLED[8] = {
  B00100, B01010, B01110, B11111,
  B01110, B01010, B00100, B00000
};

byte iconFan[8] = {
  B00100, B01110, B11111, B11111,
  B11111, B01110, B01110, B00000
};

byte iconBar[8] = {
  B11111, B11111, B11111, B11111,
  B11111, B11111, B11111, B11111
};

void setup() {
  _lightVal = _soilVal = _angle = _brightness = -1;
  // Serial.begin(9600);
  pinMode(btnPin, INPUT);
  pinMode(ledPin, OUTPUT);

  fan.attach(servoPin);

  lcd.init();
  lcd.backlight();

  lcd.createChar(0, iconLight);
  lcd.createChar(1, iconSoil);
  lcd.createChar(2, iconLED);
  lcd.createChar(3, iconFan);
  lcd.createChar(4, iconBar);

  lcd.clear();
}

void showMenu() {
  if (menuIndex < 2) {
    if (lastMenuIndex >= 2 || lastMenuIndex == -1) {
      lcd.clear();
      lcd.setCursor(1, 0); lcd.print("TT Ca nhan");
      lcd.setCursor(1, 1); lcd.print("TT Cam bien");
    }
  } else if (lastMenuIndex < 2) {
    lcd.clear();
    lcd.setCursor(1, 0); lcd.print("DK Den");
    lcd.setCursor(1, 1); lcd.print("DK Quat");
  }
  lcd.setCursor(0, 0);
  lcd.print(menuIndex & 1 ? " " : ">");
  lcd.setCursor(0, 1);
  lcd.print(menuIndex & 1 ? ">" : " ");
}

void showInfo() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("NGUYEN THO TAI");
  lcd.setCursor(4, 1);
  lcd.print("23127255");
}

void printLCDPadded(int v) {
  if (v < 1000) lcd.print(" ");
  if (v < 100)  lcd.print(" ");
  if (v < 10)   lcd.print(" ");
  lcd.print(v);
}

void showSensors() {
  int lightVal = analogRead(lightPin);
  int soilVal = analogRead(soilPin);

  if (_lightVal == -1) {
    lcd.setCursor(0, 0);
    lcd.write(byte(0));
    lcd.print(" Anh sang: ");
  }
  if (_soilVal == -1) {
    lcd.setCursor(0, 1);
    lcd.write(byte(1));
    lcd.print(" Do am   : ");
  }

  if (_lightVal != lightVal || _soilVal != soilVal) {
    _soilVal = soilVal;
    _lightVal = lightVal;
    lcd.setCursor(12, 0);
    printLCDPadded(lightVal);
    lcd.setCursor(12, 1);
    printLCDPadded(soilVal);
  }
}

void controlLED() {
  if (_brightness == -1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(byte(2));
    lcd.print(" DO SANG LED");
  }
  int brightness = map(analogRead(potPin), 0, 1023, 0, 255);
  //Serial.print("B="); Serial.println(brightness);
  if (_brightness != brightness) {
    analogWrite(ledPin, brightness);
    int barLevel = map(brightness, 0, 255, 0, 16);
    lcd.setCursor(0, 1);
    for (int i = 0; i < 16; ++i)
      lcd.write(i < barLevel ? byte(4) : ' ');
    _brightness = brightness;
  }
}

void controlFan() {
  if (_angle == -1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(byte(3));
    lcd.print(" Angle:");
  }
  int angle = map(analogRead(potPin), 0, 1023, 0, 180);
  fan.write(angle);
  if (_angle != angle) {
    lcd.setCursor(8, 0);
    printLCDPadded(angle);
    _angle = angle;
  }
}

bool lastBtnState = LOW;

bool btnPressed() {
  bool current = digitalRead(btnPin);
  unsigned long now = millis();
  if (current == LOW && lastBtnState == HIGH && now - lastBtn > 100) {
    lastBtn = now;
    lastBtnState = LOW;
    return true;
  }
  if (current == HIGH) lastBtnState = HIGH;
  return false;
}

void loop() {
  if (!inSubmenu) {
    menuIndex = map(analogRead(potPin), 0, 1023, 0, 3);
    if (menuIndex != lastMenuIndex) {
      showMenu();
      lastMenuIndex = menuIndex;
    }

    if (btnPressed()) {
      inSubmenu = true;
      lastMenuIndex = -1;
      lcd.clear();
    }
  } else {
    if (btnPressed()) {
      inSubmenu = false;
      lastMenuIndex = -1;
      _lightVal = -1; _soilVal = -1;
      _angle = -1; _brightness = -1;
      lcd.clear();
      return;
    }
    if (menuIndex == 0 && lastMenuIndex == -1) {
      lastMenuIndex = 0;
      showInfo();
      return;
    }

    switch (menuIndex) {
      case 1: showSensors(); break;
      case 2: controlLED(); break;
      case 3: controlFan(); break;
    }
  }
  delay(50);
}
