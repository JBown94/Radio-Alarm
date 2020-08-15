#include <RH_ASK.h>
#include <SPI.h>

const int SECONDS_UNTIL_ALARM = 5;

int buzzer = 2;

unsigned long delayStart = 0;
bool delayRunning = false;

char currentStatus = "0";

RH_ASK driver;

void setup() {
  pinMode(buzzer, OUTPUT);
  tone(buzzer, 850, 150);

  Serial.begin(9600);
  
  if (!driver.init()) {
    Serial.println("Error: Failed to Initialise Radio");
  } else {
     Serial.println("Listening");
  }
}

void loop() {
  uint8_t buf[3];
  uint8_t buflen = sizeof(buf);

  if (driver.recv(buf, &buflen)) {
    char *msg = (char *)buf;
    char result = msg[0];

    if (result == '3') {
      if (result != currentStatus) {
        currentStatus = result;
        
        delayStart = millis();
        delayRunning = true;

        tone(buzzer, 650, 100);
      }
      
      activateAlarm();
    }
  }
}

void activateAlarm() {
  if (delayRunning && ((millis() - delayStart) >= (SECONDS_UNTIL_ALARM * 1000))) {
    tone(buzzer, 650, 100);
    tone(buzzer, 750, 100);
  }
}
