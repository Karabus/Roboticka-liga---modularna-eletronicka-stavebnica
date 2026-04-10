#include <Servo.h>

// RP2040-Zero pinout
const int PIN_SERVO_LEFT_WHEEL = 4;
const int PIN_SERVO_RIGHT_WHEEL = 5;
const int PIN_SERVO_SCANER = 6;
const int TRIG = 7;
const int ECHO = 8;

Servo servoLeftWheel;
Servo servoRightWheel;
Servo servoScaner;

const float MIN_SAFE_DISTANCE = 20.0; // cm
const long PULSIN_TIMEOUT = 30000;
static bool blocking = LOW;
float scanDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  
  long duration = pulseIn(ECHO, HIGH, PULSIN_TIMEOUT);
  
  if (duration == 0) {
    Serial.println("Senzor timeout!");
    return -1;
  }
  
  float distance = duration * 0.0343 / 2;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  return distance;
}

void stopRobot() {
  servoLeftWheel.write(90);
  servoRightWheel.write(90);
  Serial.println("=== Stop");
}


void moveForward() {
  servoLeftWheel.write(180);   
  servoRightWheel.write(0);    
  Serial.println(">>> Dopredu");
}

void moveBackward() {
  servoLeftWheel.write(0);     
  servoRightWheel.write(180);  
  Serial.println("<<< Dozadu");
}

void rotateLeft(int durationMs) {
  servoLeftWheel.write(180);     
  servoRightWheel.write(180);    
  Serial.println("↻ Otočenie vľavo");
  delay(durationMs);
}

void rotateRight(int durationMs) {
  servoLeftWheel.write(0);   
  servoRightWheel.write(0);  
  Serial.println("↺ Otočenie vpravo");
  delay(durationMs);
}
void scanLeft() {
  servoScaner.write(0);
  Serial.println("📡 Skenujem VĽAVO");
  delay(300);
}

void scanRight() {
  servoScaner.write(180);
  Serial.println("📡 Skenujem VPRAVO");
  delay(300);
}

void scanCenter() {
  servoScaner.write(90);
  delay(200);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  delay(1000);
  Serial.println("\n=== SETUP START ===");
  
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  
  servoLeftWheel.attach(PIN_SERVO_LEFT_WHEEL, 1000, 2000);
  servoRightWheel.attach(PIN_SERVO_RIGHT_WHEEL, 1000, 2000);
  servoScaner.attach(PIN_SERVO_SCANER, 1000, 2000);
  
  delay(500);
  
  stopRobot();
  scanCenter();
  
  Serial.println("=== SETUP DONE ===\n");
}

void loop() {
  // KROK 1: Pohyb dopredu
  moveForward();
  delay(200);
  
  // KROK 2: Skontroluj predný senzor
  float distFront = scanDistance();
  blocking = distFront > 0 && distFront <= MIN_SAFE_DISTANCE;
  while (blocking) {
    // Prekážka vpredu!
    Serial.println("⚠️  Prekážka vpredu!\n");
    stopRobot();
    delay(200);
    
    // KROK 3: Pozri sa VĽAVO
    scanLeft();
    float distLeft = scanDistance();
    blocking = distLeft > 0 && distLeft <= (MIN_SAFE_DISTANCE*sqrt(2));
    if (!blocking) {
      // VĽAVO je voľne -> otoč sa tam a pokračuj
      Serial.println("✓ Vľavo je voľne! Otáčam sa vľavo...\n");
      scanCenter();
      rotateLeft(300); // ~45° otočenie
      return; // Idi na ďalší loop - pohyb dopredu
    }
    
    // KROK 4: Vľavo blokuje -> Pozri sa VPRAVO
    Serial.println("✗ Vľavo BLOKUJE! Skúšam vpravo...\n");
    scanRight();
    float distRight = scanDistance();
    blocking =  distRight > 0 && distRight <= (MIN_SAFE_DISTANCE*sqrt(2));
    if (!blocking) {
      // VPRAVO je voľne -> otoč sa tam a pokračuj
      Serial.println("✓ Vpravo je voľne! Otáčam sa vpravo...\n");
      scanCenter();
      rotateRight(300); // ~45° otočenie
      return; // Idi na ďalší loop
    }
    
    // KROK 5: Vľavo aj vpravo blokuje -> Ide DOZADU 1 sekundu
    Serial.println("✗ Vľavo aj vpravo BLOKUJE! Idem dozadu!\n");
    scanCenter();
    moveBackward();
    delay(1000);
    stopRobot();
    delay(200);
    
    // Teraz sa otoč VĽAVO (ak tam bolo voľnejšie) alebo VPRAVO
    
  }
  
  Serial.println("✓ Cesta voľná, pokračujem dopredu\n");
  delay(300);
  
}