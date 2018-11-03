#define BLYNK_PRINT Serial
#define BUTTON_MAX 1

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "50eddf6bec494445b6eb04fba34eadcf";
char id[] = "PHI_LHP";
char pass[] = "phi@@@@@";

byte buttonPin[BUTTON_MAX] = {4}; // D2
byte ledPin[2] = {14, 0}; // D5 - D3
byte ledState[BUTTON_MAX] = {LOW};

int buttonState[BUTTON_MAX]      = {LOW};
int lastButtonState[BUTTON_MAX]  = {HIGH};
unsigned long lastDebounceTime[BUTTON_MAX] = {0};
unsigned long debounceDelay    = 10;

BLYNK_CONNECTED() {
  Blynk.syncAll();
}

BLYNK_WRITE(V1)
{
  byte index = 0;
  int pinData = param.asInt();
  Serial.printf("V1 -> %d", pinData);
  Serial.println();
  changeState(index, pinData);
}

void changeState(byte index, byte newState) {
  ledState[index] = newState;
  digitalWrite(ledPin[0], newState);
  digitalWrite(ledPin[1], newState);
}

void initPin() {
  pinMode(buttonPin[0], INPUT);
  pinMode(ledPin[0], OUTPUT);
  pinMode(ledPin[1], OUTPUT);
}

void setup() {
  initPin();
  Serial.begin(115200); 
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
    digitalWrite(ledPin[0], ledState[i]);
    digitalWrite(ledPin[1], ledState[i]);
    switch (i) {
      case 0:
        Blynk.virtualWrite(V1, ledState[i]);
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
        return (true);
      }
    }
  }
  lastButtonState[i] = reading;
  return false;
}
