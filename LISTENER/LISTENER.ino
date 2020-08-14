#include <RH_ASK.h>
#include <SPI.h>

//--------------------------
//Pin assignment
const int buzzer = 2;
const int listener = A1;
//--------------------------

RH_ASK driver;

enum StatusCodes {
  Armed = 100,
  Disarmed = 200,
  Triggered = 300
  //Deactivated = 400
};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);
  tone(buzzer, 450, 200);
  tone(buzzer, 750, 200);

  if (!driver.init()) {
    Serial.println("Error: Failed to Initialise Radio");
  }
  
  Serial.begin(9600);
  Serial.println("Status: Listening");
}

void loop() {
  uint8_t buf[3];
  uint8_t buflen = sizeof(buf);

  // Non-blocking
  if (driver.recv(buf, &buflen)) {
    int i;

    digitalWrite(LED_BUILTIN, HIGH);
    // Message with a good checksum received, dump it.
    Serial.print("Got: ");
    Serial.println((char*)buf);

    digitalWrite(LED_BUILTIN, LOW);
  }
}
