int joyX = A0;
int joyY = A1;
int button = 15;

void setup() {
  Serial.begin(115200);

  pinMode(button, INPUT_PULLUP);
}

void loop() {

  int x = analogRead(joyX);
  int y = analogRead(joyY);
  int sw = digitalRead(button);

  Serial.print("X: ");
  Serial.print(x);

  Serial.print("  Y: ");
  Serial.print(y);

  Serial.print("  SW: ");
  Serial.println(sw);

  delay(100);
}