#include <Keypad.h>
#include <RH_ASK.h>
#include <SPI.h>

const byte ROWS = 4;
const byte COLS = 4;

const int MAX_CODE_LENGTH = 4;

//--------------------------
//Pin assignment
const int button = 2;
const int buzzer = 11;
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
RH_ASK driver;

char disarmCode[MAX_CODE_LENGTH] = {'0', '0', '0', '0'}; //TODO: Temporary - Look into defining on the device
bool activated = false;

char currentCode[MAX_CODE_LENGTH] = {};
int currentCodeIdx = 0;
//-------------------------------------------------------------------------------------------------------------------------

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  pinMode(button, INPUT);

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(green_led, HIGH);
  digitalWrite(red_led, LOW);

  if (!driver.init()) {
    Serial.println("Error: Failed to Initialise Radio");
  }

  tone(buzzer, 950);
  delay(50);
  noTone(buzzer);
  delay(50);
  tone(buzzer, 950);
  delay(50);
  noTone(buzzer);
  
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

  if (activated) {
    handleCodeEntry();
    enableAlarm();
  } else {
    sendStatusCode(Armed); //TODO: Does this need to be constantly broadcasting, or just send once for each status change?
    disableAlarm(false);
  }
}

void setInitialTrigger() {
  Serial.println("Status: Triggered");
  tone(buzzer, 950, 100);
}
void enableAlarm() {
  digitalWrite(green_led, LOW);
  digitalWrite(red_led, HIGH);

  sendStatusCode(Triggered);
}
void disableAlarm(bool setDisable) {
  digitalWrite(green_led, HIGH);
  digitalWrite(red_led, LOW);

  if (setDisable) {
    Serial.println("Status: Disarmed");
    sendStatusCode(Disarmed);
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
  char *msg = "";

  //TODO: See if able to convert the values directly
  switch (code) {
    case Armed: msg = "100"; break;
    case Disarmed: msg = "200"; break;
    case Triggered: msg = "300"; break;
    //case Deactivated: msg = 400; break;
    default: msg = "000";
  }

  driver.send((uint8_t *)msg, strlen(msg));
  driver.waitPacketSent();
}
