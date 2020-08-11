#include <Keypad.h>
#include <VirtualWire.h>

const byte ROWS = 4;
const byte COLS = 4;

const int MAX_CODE_LENGTH = 4;
const int SECONDS_UNTIL_ALARM = 5;

//--------------------------
//Pin assignment
const int button = 2;
const int buzzer = 11;
const int transmitter = 12;

const int green_led = A1;
const int red_led = A2;
//--------------------------

enum StatusCodes {
  Armed = 100,
  Disarmed = 200,
  Triggered = 300
  //Deactivated = 400
};

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {10, 9, 8, 7};
byte colPins[COLS] = {6, 5, 4, 3};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

char disarmCode[MAX_CODE_LENGTH] = {'0', '0', '0', '0'}; //TODO: Temporary - Look into defining on the device
bool activated = false;

char currentCode[MAX_CODE_LENGTH] = {};
int currentCodeIdx = 0;

unsigned long delayStart = 0;
bool delayRunning = false;
//-------------------------------------------------------------------------------------------------------------------------

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  pinMode(button, INPUT);

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(green_led, HIGH);
  digitalWrite(red_led, LOW);

  vw_set_tx_pin(transmitter);
  vw_setup(2000);

  sendStatusCode(Armed);

  Serial.begin(9600);
  Serial.println("Status: Armed");
}

void loop() {
  if (digitalRead(button) == HIGH) {
    if (activated != true) {
      setInitialTrigger();
    }
    
    activated = true;
  }

  sendStatusCode(Armed); //TODO: Does this need to be constantly broadcasting, or just send once for each status change?

  if (activated) {
    handleCodeEntry();
    enableAlarm();
  } else {
    disableAlarm(false);
  }
}

void setInitialTrigger() {
  Serial.println("Status: Triggered");
  tone(buzzer, 950, 100);
  
  delayStart = millis();
  delayRunning = true;
}
void enableAlarm() {
  digitalWrite(green_led, LOW);
  digitalWrite(red_led, HIGH);
  
  if (delayRunning && ((millis() - delayStart) >= (SECONDS_UNTIL_ALARM * 1000))) {
    sendStatusCode(Triggered);
    
    tone(buzzer, 650, 100);
    tone(buzzer, 950, 100);
  }
}
void disableAlarm(bool setDisable) {
  sendStatusCode(Disarmed);
  
  digitalWrite(green_led, HIGH);
  digitalWrite(red_led, LOW);

  if (setDisable) {
    Serial.println("Status: Disarmed");
    tone(buzzer, 700, 100);
    tone(buzzer, 800, 100);
    tone(buzzer, 900, 100);
  }

  activated = false;
}

void handleCodeEntry() {
  char enteredKey = customKeypad.getKey();
  bool hasDisarmed = false;
  
  if (enteredKey) {   
    currentCode[currentCodeIdx] = enteredKey;
    currentCodeIdx++;
    
    tone(buzzer, 1000, 50);
  }

  if (currentCodeIdx > MAX_CODE_LENGTH-1) {
    if (checkDisarmed()) {
      disableAlarm(true);
    } else {
      Serial.println("Status: Unauthorised - Incorrect Code");
      tone(buzzer, 500, 500);
      tone(buzzer, 300, 200);
    }
  }
}

bool checkDisarmed() {
   bool hasDisarmed = true;
    
  for (int i=0; i < currentCodeIdx; i++) {
    if (currentCode[i] != disarmCode[i]) {
      hasDisarmed = false;
      break;
    }
  }

  currentCodeIdx = 0;

  return hasDisarmed;
}

void sendStatusCode(StatusCodes code) {
  vw_send((uint8_t *)code, 3);
  vw_wait_tx();
}
