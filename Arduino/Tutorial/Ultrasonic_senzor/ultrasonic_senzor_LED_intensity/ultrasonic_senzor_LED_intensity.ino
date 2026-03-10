#define TRIG 3
#define ECHO 2
#define LED_PIN 0   // PWM pin pre LED

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // spustenie merania
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);
  float distance = duration * 0.0343 / 2; // cm

  distance = constrain(distance, 5, 100); // obmedzíme

  // mapovanie vzdialenosti na PWM (0–255)
  int brightness = map(distance, 5, 100, 255, 0); // bližšie = jasnejšie

  analogWrite(LED_PIN, brightness);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm  LED brightness: ");
  Serial.println(brightness);

  delay(50);
}