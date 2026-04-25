#include <Servo.h>

#define SERVO_PIN 12

String inputString = "";
int prikaz = 0;
Servo servo;

int position = 180;
void setup() {
  Serial.begin(115200);
  servo.attach(SERVO_PIN,500,2500);
  servo.write(position);
}

void loop() {

   while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') { // Enter
      position = inputString.toInt();
      if (position >= 0 && position <= 180) {
        servo.write(position); // nastavi servo na uhol
        Serial.print("Prikaz cislo: ");
        Serial.print(prikaz);
        Serial.print(" Servo je nastavny na uhli: ");
        Serial.println(servo.read());
        prikaz++;
      } else {
        Serial.println("Neplatna pozicia");
      }
      inputString = "";
    } else {
      inputString += c;
    }
  }

  delay(50);
}