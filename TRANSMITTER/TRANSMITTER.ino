#include <Keypad.h>
#include <VirtualWire.h>

const byte ROWS = 4;
const byte COLS = 4;

const int MAX_CODE_LENGTH = 4;

//--------------------------
//Pin assignment
const int button = 2;
const int buzzer = 11;
const int transmitter = 12;
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

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(button, INPUT);

  digitalWrite(LED_BUILTIN, LOW);

  vw_set_tx_pin(transmitter);
  vw_setup(2000);

  sendStatusCode(Armed);
  
  Serial.begin(9600);
  Serial.println("Status: Armed");
}

void loop() {
  if (digitalRead(button) == HIGH) {
    if (activated != true) {
      Serial.println("Status: Triggered");
    }
    
    activated = true;
  }

  sendStatusCode(Armed);

  if (activated) {
    handleCodeEntry();
    enableAlarm();
  } else {
    disableAlarm(false);
  }
}

void enableAlarm() {
  sendStatusCode(Triggered);
}
void disableAlarm(bool setDisable) {
  //TODO: Send a radio signal to the other board that the alarm has been disarmed
  activated = false;
  //noTone(buzzer);

  if (setDisable) {
    Serial.println("Status: Disarmed");
    sendStatusCode(Disarmed);
    tone(buzzer, 700, 100);
    tone(buzzer, 800, 100);
    tone(buzzer, 900, 100);
  }
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