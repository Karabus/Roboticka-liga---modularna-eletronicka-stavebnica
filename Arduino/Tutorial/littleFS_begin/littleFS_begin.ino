#include <Arduino.h>
#include <LittleFS.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  // Mount
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount zlyhalo, formatujem...");
    if (!LittleFS.format()) { Serial.println("Format zlyhalo!"); return; }
    if (!LittleFS.begin())  { Serial.println("Mount po formate zlyhalo!"); return; }
  }
  Serial.println("LittleFS OK");

  // Zapis s kontrolou
  File f = LittleFS.open("/hello.txt", "w");
  if (!f) { Serial.println("CHYBA: open na zapis zlyhalo!"); return; }
  f.println("Hello, LittleFS!");
  f.flush();
  f.close();
  Serial.println("Zapis OK");

  // Over ze subor existuje
  if (!LittleFS.exists("/hello.txt")) {
    Serial.println("CHYBA: subor neexistuje po zapise!");
    return;
  }
  Serial.println("Subor existuje OK");

  // Citanie
  File r = LittleFS.open("/hello.txt", "r");
  if (!r) { Serial.println("CHYBA: open na citanie zlyhalo!"); return; }
  Serial.println("Obsah:");
  while (r.available()) Serial.write(r.read());
  r.close();
}

void loop() {}