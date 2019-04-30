# Radmesser-Schaltplan und Sensorcode

Dies ist der Schaltplan und der Code für den Radmesser-Sensor. Der Radmesser-Sensor wird an einem Fahrrad angebracht und kann messen, ob und in welchem Abstand das Fahrrad von einem Auto oder einem anderen Fahrzeug überholt wird.

Dafür misst der Radmesser-Sensor mit zwei Ultraschallsensoren nach links. Die zwei Sensoren sind jeweils geneigt - einer nach vorne und einer nach hinten. Je nachdem, welcher Sensor das Objekt zuerst sieht, kann unterschieden werden, wer wen überholt hat. Zusätzlich misst ein weiterer Sensor den Abstand nach rechts, um zu wissen, wie viel Platz zu parkenden Autos gehalten wurde. Um die Messergebnisse zu bereinigen, muss der Abstand vom Sensor zur Außenkante des Lenkers/Fahrrads ausgemessen werden.

Für eine genauere Messung empfiehlt es sich, die aktuelle Außentemperatur zu erheben, um die Temperaturabhängigkeit der Schallgeschwindigkeit zu berücksichtigen. Für eine verlässliche Messung ist zudem noch ein Foto von dem überholenden Objekt nötig. Dies kann mithilfe einer Smartphone-App aufgenommen werden.

## Elektronische Bauteile
- 1x Arduino Nano (ATmega168)
- 3x Ultraschallsensoren HC-SR04
- 1x BLE-Modul AT-09 bzw. MLT-BT05
- 1x Powerbank 2000 mAh
- 1x USB A Male Pin
- 1x Kippschalter
- 1x LED
- 1x Vorwiderstand
- genügend Kabel

## Verpackung
- 1x Strapubox 2004
- 3x 12-20cm Kabelklettband zur Befestigung
- Schrauben und Muttern zum befestigen der Klettbänder

## Erklärung

Wir haben den kleinsten Arduino verwendet, hauptsächlich aus Kostengründen. Deswegen ist der Arduino-Code auf wenig Speicherverbrauch hin optimiert.

Wir benutzen drei Mal die Standard-Ultraschallsensoren HC-SR04 mit Reset-Routine um die Messgenauigkeit zu erhöhen. Ausserdem schalten wir während der Messung die Arduino interrupts aus, was die Messgenauigkeit ebenfalls erhöht. ==== die Resetroutine versteht man hier nicht, würde eher generell sagen dass die dinger mit ein paar Tricks stabil genug bekommt und auf den code verweisen

Die verwendeten BLE-Module waren AT-09/MLT-BT05. Gekauft haben wir sie unter dem Namen AT-09, aber der voreingestellte Name war MLT-BT05. Diese Module hatten von allen getesteten Modulen die höchste Zuverlässigkeit: Es haben alle funktioniert, die wir bestellt haben. Das Modul kann ca. alle 300 ms eine String von 18 chars senden. Um dennoch schnell genug die Daten zum Smartphone senden zu können, werden die Daten in einem auf die Anwendung hin optimierten Verfahren codiert.

Die 2000 mAh Powerbank ist ausreichend für > 5 Stunden Messzeit.

In das Plastikgehäuse müssen Öffnungen für die Ultraschallsensoren gebohrt/gefräst werden, damit die Ultraschallsensoren herausschauen können. Die Klettbänder wurden an dem Gehäuse verschraubt. Mithilfe von Stücken von alten Fahrradschläuchen haben wir dafür gesorgt, dass der Fahrradlack nicht von den Schraubenköpfen zerkratzt wird und der Sensor weniger hin und her schwenkt bzw. stabiler am Fahrradrohr sitzt.

Die Sensoren nach links sind jeweils um ca. 10° nach vorne und nach hinten geneigt. Dieser Wert hat sich in den Tests bewährt.

Um zu testen ob der Sensor funktioniert, kann eine BLE-App auf dem Smartphone installiert werden, wie z.B. die App BLE Scanner. Eine sehr detaillierte Beschreibung, wie dies funktionieren kann findet sich z.B. auf der Seite von Martyn Curry: http://www.martyncurrey.com/hm-10-bluetooth-4ble-modules/

Weitere Erklärungen zur Methode und zur Datenauswertung finden sich hier: https://www.tagesspiegel.de/gesellschaft/medien/radmesser-die-genaue-methode-wie-wurden-die-ueberholabstaende-gemessen/23710682.html



# Radmesser Circuit Diagram and Arduino Code

This is the circuit diagram and Arduino code for the Radmesser sensor. The Radmesser sensor is supposed to be attached to a bicycle and can measure whether the bicycle is being overtaken by a car or another vehicle and at which distance.

To do this, the Radmesser sensor measures to the left with two ultrasonic sensors. The two sensors are each inclined - one to the front and one to the rear. Depending on which sensor sees the object first, a distinction can be made as to who overtook whom. In addition, another sensor measures the distance to the right in order to know how much space has been held towards parked cars. To correct the measurement results, the distance from the sensor to the outer edge of the handlebar/bicycle must be measured.

For a more accurate measurement, it is recommended to record the current outside temperature to take into account the temperature dependence of the speed of sound. For a reliable measurement, a photo of the overtaking object is also required. This can be done using a smartphone app.

## Electronic Parts
- 1x Arduino Nano (ATmega168)
- 3x ultrasonic sensors HC-SR04
- 1x BLE module AT-09 bzw. MLT-BT05
- 1x power bank 2000 mAh
- 1x USB A Male Pin
- 1x toggle switch
- 1x LED
- 1x dropping resistor
- enough wiring

## Casing
- 1x Strapubox 2004
- 3x 12-20cm Velcro straps for mounting the sensor
- Screws and nuts for fastening the Velcro straps


## Explanation

We used the smallest Arduino, mainly for cost reasons. That's why the Arduino code is optimized for low memory usage.

We use three times the standard ultrasonic sensors HC-SR04 with reset routine to increase the measurement accuracy. We also switch off the Arduino interrupts during the measurement, which also increases the measurement accuracy.

The BLE modules used were AT-09/MLT-BT05. We bought them under the name AT-09 from AliExpress, but the factory default name was MLT-BT05. These modules had the highest reliability of all the modules tested: all the modules we ordered worked. The module can send a string of 18 chars every 300 ms. In order to send the data to the smartphone fast enough, the data is encoded in a procedure optimized for the application.

The 2000 mAh power bank is sufficient for > 5 hours measuring time.

We drilled holes in the plastic housing so that the ultrasonic sensors can look out. The Velcro tapes were screwed to the housing. With the help of pieces of old bicycle tubes, we made sure that the bicycle paint is not scratched by the screw heads and that the sensor swivels less back and forth or rather sits more stable on the bicycle tube.

The sensors to the left are each tilted by approx. 10° forwards and backwards. This value has proven itself in the tests.

To test whether the sensor works, a BLE app can be installed on the smartphone, such as the BLE Scanner app. A very detailed description of how this works can be found on the Martyn Curry website: http://www.martyncurrey.com/hm-10-bluetooth-4ble-modules/

Further explanations of the method and data evaluation can be found here (German language): https://www.tagesspiegel.de/gesellschaft/medien/radmesser-die-genaue-methode-wie-wurden-die-ueberholabstaende-gemessen/23710682.html
