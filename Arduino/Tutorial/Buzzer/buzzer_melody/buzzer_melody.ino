#define BUZZER_PIN 0

// definícia melódie: frekvencie v Hz
int melody[] = {262, 294, 330, 349, 392, 440, 494, 523}; // C D E F G A B C

// dĺžka not v ms
int noteDurations[] = {500, 500, 500, 500, 500, 500, 500, 500};

void setup() {
  Serial.begin(115200);
}

void loop() {
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = noteDurations[thisNote];
    tone(BUZZER_PIN, melody[thisNote], noteDuration);

    delay(noteDuration * 1.3); // malá medzera medzi notami
  }

  delay(2000); // pauza pred opakovaním melódie
}