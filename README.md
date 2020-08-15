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
* Portable Power Supplies/Power Packs (x2)

### 1st Board [Transmitter]
---
* 4x4 Matrix Keypad
* Push button
* Piezo Active Buzzer
* Radio Transmitter
* Red & Green LEDs

### 2nd Board [Listener]
---
* Power module
* Ultrasonic sensor
* Piezo Active Buzzer
* Radio Transmitter

## Software
---

The software is going to be split up into 2 separate sketches;
* One for triggering the alarm manually, via the panic button, & disarming the device.
* One for determining if the alarm has been triggered via movement/detection & handling the actual alarm notification.

### Dependencies
---

* **RH_ASK** - On both sketches, for the radio communication
	* **SPI**	- A dependancy of the RH_ASK library
* **KeyPad** - On the Transmitter sketch, for the handling on the 4x4 matrix keypad