#define BLYNK_PRINT Serial
#define BUTTON_MAX 2

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "50eddf6bec494445b6eb04fba34eadcf";
char id[] = "VP.LEHONGPHONG";
char pass[] = "87654321";

byte buttonPin[BUTTON_MAX] = {D5, D6};
byte ledPin[BUTTON_MAX] = {D2, D1};
byte ledState[BUTTON_MAX] = {LOW, LOW};

int buttonState[BUTTON_MAX]      = {LOW, LOW};
int lastButtonState[BUTTON_MAX]  = {HIGH, HIGH};
unsigned long lastDebounceTime[BUTTON_MAX] = {0, 0};
unsigned long debounceDelay    = 10;  

BLYNK_CONNECTED() {
  Blynk.syncAll();
}

BLYNK_WRITE(V1) {
  byte index = 0;
  int pinData = param.asInt();
  Serial.printf("V1 -> %d", pinData);
  Serial.println();
  changeState(index, pinData);
}

BLYNK_WRITE(V2) {
  byte index = 1;
  int pinData = param.asInt();
  Serial.printf("V2 -> %d", pinData);
  Serial.println();
  changeState(index, pinData);
}

void changeState(byte index, byte newState) {
  ledState[index] = newState;
  digitalWrite(ledPin[index], newState);
}

void smartConfigWifi() {
  int connect_count = 0;
  Serial.print("Waiting for config..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("..");
    if (connect_count >= 10) {
      WiFi.beginSmartConfig();
      while (1) {
        delay(500);
        if (WiFi.smartConfigDone()) {
          Serial.println();
          Serial.println("SmartConfig: Success");
          Serial.println("Connecting to Wifi..");
          break;
        }
        Serial.print("..");
      }
      connect_count++;
    }
  }
  WiFi.printDiag(Serial); // hiển thị thông tin Wifi
}

void initPin() {
  pinMode(buttonPin[0], INPUT);
  pinMode(buttonPin[1], INPUT);
  pinMode(ledPin[0], OUTPUT);
  pinMode(ledPin[1], OUTPUT);
}

void setup() {
  initPin();
  Serial.begin(115200);
  delay(5);
  WiFi.mode(WIFI_STA);
  delay(5);
  Blynk.begin(auth, id, pass);
}

void loop() {
  Blynk.run();
  byte indexButtonPushed = hasButtonPushed();
  if (indexButtonPushed != 255) {
    byte i = indexButtonPushed;
    Serial.printf("indexButtonPushed = %d, buttonState = %d", i, ledState[i]);
    Serial.println();
    ledState[i] = ledState[i] ? LOW : HIGH;
    digitalWrite(ledPin[i], ledState[i]);
    switch (i) {
    case 0:
      Blynk.virtualWrite(V1, ledState[i]);
      break;
    case 1:
      Blynk.virtualWrite(V2, ledState[i]);
      break;
    }

  }
}


byte hasButtonPushed() {
  for (int i = 0; i < BUTTON_MAX; ++i) {
    if (buttonPushed(i))
      return i;
  }
  return 255;
}
bool buttonPushed(byte i) {
  int reading = digitalRead(buttonPin[i]);
  if (reading != lastButtonState[i]) {
    lastDebounceTime[i] = millis();
  }
  if ((millis() - lastDebounceTime[i]) > debounceDelay) {
    if (reading != buttonState[i]) {
      buttonState[i] = reading;
      if (buttonState[i] == HIGH) {
        return(true);
      }
    }
  }
  lastButtonState[i] = reading;
  return false;
}
