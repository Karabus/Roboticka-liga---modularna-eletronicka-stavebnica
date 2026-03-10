#define BUZZER_PIN 0  // pin pre buzzer

String inputString = ""; // uloží číslo zo Serialu

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.println("Zadaj frekvenciu pre buzzer (Hz):");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') { // Enter
      int freq = inputString.toInt();
      if (freq > 0) {
        tone(BUZZER_PIN, freq); // spusti tón
        Serial.print("Buzzer hra frekvenciu: ");
        Serial.println(freq);
      } else {
        noTone(BUZZER_PIN); // stop
        Serial.println("Neplatna frekvencia, vypinam buzzer");
      }
      inputString = "";
    } else {
      inputString += c;
    }
  }
}