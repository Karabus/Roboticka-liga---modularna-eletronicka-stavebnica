#include <Servo.h>

#define SERVO_PIN 6

String inputString = "";
int prikaz = 0;
Servo servo;

int position = 90;
void setup() {
  Serial.begin(115200);
  servo.attach(SERVO_PIN);
  servo.write(position);
}

void loop() {

   while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') { // Enter
      position = inputString.toInt();
      if (position >= 500 && position <= 3000) {
        servo.writeMicroseconds(position); // nastavi servo na uhol
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