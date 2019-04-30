/*
 *  Radmesser Sensor Arduino Code:
 *
 *  Author: Michael Gegg
 *  EMail:  michael.gegg@tagesspiegel.de
 *
 *  Description:
 *  This is the code for the Arduino in the Radmesser-Sensor setup. The Arduino
 *  used in the Radmesser experiment was the Arduino Nano ATmega168. This
 *  produces memory limitations. If the ATmega328 is used the code can be
 *  written in a more human friendly manner.
 *
 */

// library for the BLE module communication
// this fixes the pins for the BLE module to D9 -> RX and D8 -> TX
#include <AltSoftSerial.h>
AltSoftSerial BTserial;

// pins for the ultrasonic sensors:
// sensor 1 measures to the left/back, sensor 2 measures to the left/front,
// sensor 3 measures to the right
const short TRIGGER_PIN3 = 4;
const short ECHO_PIN3    = 5;

const short TRIGGER_PIN2 = 6;
const short ECHO_PIN2    = 7;

const short TRIGGER_PIN1 = 10;
const short ECHO_PIN1    = 11;

// left threshold (in units of 10 microseconds)
// If a left facing sensor measures something closer than this, the measurement 
// is discarded. This prevents anything closer than the edge of the handlebar
// to trigger an event.
short left_threshold = 0;

// ultrasonic response time in microseconds
short duration = 0;

// time since the start of the measurement (in units of 200 milliseconds)
unsigned long tmeasure = 0;

// storage for a single char that comes form the BLE module, which
// was sent by the smartphone
char swap  = ' ';

// buffer for input received from the BLE module
String input = "";

// `ret` encodes the measurements, and is sent via the BLE Module:
//
// The AT-09 sends a string of 20 characters, where the last two characters
// are reserved for the new line character. This leaves 18 characters for data
// encoding. The AT-09 can send roughly every 300 milliseconds.
// If the strings are transferred to the AT-09 faster than that for a short
// while, the data just arrives a bit delayed. If the transfer rate is too fast
// for an extended period, data will be lost.
//
// 18 characters every 300 milliseconds is not much. We still used this
// BLE-Module since it was the most stable and reliable one that we tested, 
// and since it is quite cheap.
// Due to this restriction in data transfer rate we encoded all data as 8bit
// characters.
//
// The first character is used for the arduino time, which is a bit off, since
// we are using noInterrupts(). The unit is 200 milliseconds, so that the time
// overflows after 256*200 milliseconds, which can be corrected on the app side.
//
// The remaining 17 characters are reserved for the distance measurement. 8
// measurements each for the two left sensors and one measurement for the right
// sensor. The sensors time out after 24 milliseconds. If all sensors time out
// this means that the string is filled in >400 milliseconds. If objects like
// cars come close, this rate will increase, since the sound travel time is
// less. If there is an object within one meter left and right, this time drops
// to ~100 milliseconds. Overtaking events are usually short, so this does not
// produce problems in most cases. There is the possibility to apply a distance
// dependent delay between measurements, but this has the drawback that
// sometimes short and close overtaking events cannot be recorded properly.

String ret = "000000000000000000";

// tstart is the arduino time (millis()) of the start of the measurement
unsigned int tstart = 0;

// index for the ret string
unsigned short i = 0;

// if true the measurement is enabled, if false the measurement is disabled
bool flag = false;

// write the time to the first character of the ret string
// units are 200 milliseconds since measurement start
void writeTime() {
  tmeasure = millis() - tstart;
  tmeasure /= 200;
  swap = tmeasure % 256;
  ret.setCharAt(0,swap);
}

// Convert the measured time of the HC-SR04 to a single character:
// Takes the time for a ONEWAY travel between object and sensor in units
// of 10 microseconds. Due to the timeout, this number is < 1200,
// (which corresponds to roughly 4m distance to the object at around 4°C.)
// We divide this number by 4.7 (1200 / 256 ~ 4.69) to encode the distance
// with 47 microsecond (~1.5 cm) resolution, which is more than enough.
char convertChar(const short &deltat) {
  return deltat / 4.7;
}

// Measurement function:
// returns the time that the sound wave took ONEWAY between the object and the
// sensor, otherwise returns the timout constant.
// The units are 10 microseconds.
// The noInterrupts() is very important for accuracy, without this the measured
// distances are off by many centimeters.
// Resets the HC-SR04 if it timeouts. This step reduces the notorious noise of
// the cheap HC-SR04 sensors.
void measure(const short &trigger,const short &echo) {
  delayMicroseconds(5);

  // trigger
  noInterrupts();
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  // reads the echoPin, returns the sound wave travel time in microseconds
  // timeout in microseconds corresponds to roughly 4m distance to the object at
  // roughly 4°C, if its warmer the distance gets bigger
  duration = pulseIn(echo, HIGH, 24000);
  interrupts();

  // pulseIn will only return 0 if it timed out. 
  // therefore we reset the HC-SR04 to increase measurment accuracy
  if (duration == 0) { // if the measurment timed out
    pinMode(echo, OUTPUT); // then we set echo pin to output mode
    digitalWrite(echo, LOW); // we send a LOW pulse to the echo pin
    delayMicroseconds(200);
    pinMode(echo, INPUT); // and finally we come back to input mode
  }

  if (duration) duration /= 20;
  else          duration = 1200;
}

// initialize serial for monitoring
void setupSerial() {
  Serial.begin(9600);
  Serial.print("Sketch:   "); Serial.println(__FILE__);
  Serial.print("Uploaded: "); Serial.println(__DATE__);
}

// initialize the HC-SR04
void setupUltrasonic(short trigger,short echo, short no) {
  // initialize pins for HC-SR04
  pinMode(trigger, OUTPUT); 
  pinMode(echo, INPUT);
  // reset the trigger pin and wait a half a second
  digitalWrite(trigger, LOW);
  delayMicroseconds(500);
  // initial test measurement
  measure(trigger,echo);
  Serial.print("Initial test measurement Sensor ");
  Serial.print(no);
  Serial.print(": ");
  Serial.print(duration);
}

// check response of the BLE module
void checkResponse() {
  delay(600);
  while (BTserial.available()) {
    swap = BTserial.read();
    Serial.print(swap);
  }
  Serial.println("");
}

// setup the BLE module:
// This step needs to be changed if a different BLE module is used.
void setupBLE() {
  // intitialize serial connection to AT-09
  BTserial.begin(9600);
  Serial.println("BTserial started at 9600");
  delay(500);
  BTserial.println("AT+DEFAULT");
  checkResponse();
  BTserial.println("AT+ROLE0");
  checkResponse();
  BTserial.println("AT+TYPE0");
  checkResponse();
  BTserial.println("AT+NAMEradmesser");
  checkResponse();
  BTserial.println("AT+RESET");
  checkResponse();
}


// setup functon
void setup() {
  setupSerial();
  setupUltrasonic(TRIGGER_PIN1,ECHO_PIN1,1);
  setupUltrasonic(TRIGGER_PIN2,ECHO_PIN2,2);
  setupUltrasonic(TRIGGER_PIN3,ECHO_PIN3,3);
  setupBLE();
}


// Main function
void loop() {
  // Check for notifications from the connected device, e.g. disable and enable
  // measurment commands 's' and 'm'
  while(BTserial.available()) {
    swap = BTserial.read();
    input.concat(swap);
    delay(10);
  }

  if( input != "" || swap != ' ' ) {
    Serial.print("data received");
    Serial.println(input);

    // Decide what to do:
    // 'dxy': The measurement is started if the sensor receives a string
    //        starting with d and the time xy that the sound travels
    //        from the sensor to the outer edge of the handlebar, in units of
    //        10 microseconds, encoded as a number with usually two digits
    //        e.g. d47 for 470 microseconds ~ 15cm
    // 'm':   The measurement can also be started by sending the 'm' character
    // 's':   The measurement can be stopped by sending the 's' character,
    //        this step sometimes needs to be done multiple times, if the
    //        character is sent during the measurement
    // 'r':   The arduino can be reset by sending the 'r' character
    //
    // Anything else also stops the measurement
    if (input.charAt(0) == 'd') {

      Serial.println("inner distance:");
      input.remove(0,1);
      left_threshold  = input.toInt();

      // set the measurement flag to true
      flag = true;

      // set the start time of the measurement
      tstart = millis();

      Serial.print(left_threshold);
    } else if (swap == 'm') { // enable the measurement
      Serial.println("enabled");
      flag = true;
    } else if (swap == 's') { // disable the measurement
      Serial.println("disabled");
      flag = false;
    } else if (swap == 'r') { // restart the arduino
      Serial.println("received r");
      asm volatile ("  jmp 0");
    } else {
      // set the measurement flag to false if something weird happened
      flag = false;
    }
    input = "";
    swap = ' ';
  }

  if (flag) {
    // This is the measurement cycle:
    // First the time is written, then four pairs of measurments to the left are
    // recorded, then a single measurement to the right and then another four
    // measurements to the left. Then the date is sent via the BLE Module, which
    // needs a small delay of 20 milliseconds to properly work.
    //
    // The measurement to the right is placed in the middle so that the
    // measurement to the left has two breaks of ~20-25 milliseconds instead of
    // one break of ~40-50 milliseconds. This setup performed better, produced
    // less errors due to left side measurement interrupts.
    //
    writeTime(); // write current time to first char in ret
    i = 0;       // index in ret

    while(i<4) {  // write four pairs of measurements to the string

      // back sensor
      measure(TRIGGER_PIN1,ECHO_PIN1);
      if (duration > left_threshold) {  // only if it was 'outside' the bicycle
        Serial.print("Sensor 1: ");
        Serial.println(duration);
        ret.setCharAt(1+2*i,convertChar(duration));

        //front sensor
        measure(TRIGGER_PIN2,ECHO_PIN2);
        if (duration > left_threshold) {// only if it was 'outside' the bicycle
          Serial.print("Sensor 2: ");
          Serial.println(duration);
          ret.setCharAt(1+2*i+1,convertChar(duration));
          i++;
        }
      }
    }

    // one measurement to the right
    measure(TRIGGER_PIN3,ECHO_PIN3);
    ret.setCharAt(17,convertChar(duration));

    while (i<8) { // write another four pairs of measurements to the string

      // back sensor
      measure(TRIGGER_PIN1,ECHO_PIN1);
      if (duration > left_threshold) { // only if it was 'outside' the bicycle
        Serial.print("Sensor 1: ");
        Serial.println(duration);
        ret.setCharAt(1+2*i,convertChar(duration));

        // front sensor
        measure(TRIGGER_PIN2,ECHO_PIN2);
        if (duration > left_threshold) { // only if it was 'outside' the bicycle
          Serial.print("Sensor 2: ");
          Serial.println(duration);
          ret.setCharAt(1+2*i+1,convertChar(duration));
          i++;
        }
      }
    }

    // send the information
    BTserial.print(ret);
    Serial.println(ret);
    delay(20); // delay neccessary for BLE Module
    swap = ' ';
  }
}
