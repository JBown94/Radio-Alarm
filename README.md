# Radio Alarm
---

This simple wireless radio intruder alarm is designed so that it can be triggered either with a motion detector, or a simple panic button.
Then can only be disabled by entering the correct code via the keypad.

````
NOTE: Will be looking at using RFID keycards/fobs as an alternative disarm mechanism.
````

## Hardware
---

* Arduino Nano (x2)
* Small Breadboard (x2)

### 1st Board
---
* 4x4 Matrix Keypad
* Push button
* Piezo Active Buzzer
* Radio Transceiver

### 2nd Board
---
* Ultrasonic sensor
* Piezo Active Buzzer
* Radio Transceiver

## Software
---

The software is going to be split up into 2 separate sketches;
* One for triggering the alarm manually, via the panic button, handling the actual alarm & finally disarming the device.
* One for determining if the alarm has been triggered via movement/detection.