#include <VirtualWire.h>

//--------------------------
//Pin assignment
const int buzzer = 2;
const int listener = A1;
//--------------------------

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

  vw_set_rx_pin(listener);
  vw_setup(2000);
  vw_rx_start();

  Serial.begin(9600);
  Serial.println("Status: Listening");
}

void loop() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  
  
  // Non-blocking
  if (vw_get_message(buf, &buflen)) {
    int i;

    digitalWrite(LED_BUILTIN, HIGH); // Flash a light to show received good message
    // Message with a good checksum received, dump it.
    Serial.print("Got: ");
  
    for (i = 0; i < buflen; i++) {
      Serial.print(buf[i], HEX);
      Serial.print(' ');
    }
    
    Serial.println();
    digitalWrite(LED_BUILTIN, LOW);
  }
}
