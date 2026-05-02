#include <Servo.h>

// ✅ SPRÁVNY PIN MAPPING - A26, A27, A28, A29 (NIE 26, 27, 28, 29!)
int pins[4] = {A0, A1, A2, A3};  // ← KLÚČOVÁ ZMENA!
const int NumOfSensors = 4;

// Servo motory
Servo leftServo;
Servo rightServo;
const int LeftServoPin = 12;
const int RightServoPin = 13;

// Ultrazvukový senzor
const int TRIG = 0;
const int ECHO = 1;

// PID premenné
int D = 0, P = 0, I = 0, previousError = 0, error = 0, PIDvalue = 0;
int lsp, rsp;

// PID koeficienty - FIXNÉ!
const int lfSpeed = 95;
const float Kp = 0.006;
const float Ki = 0.0001;
const float Kd = 0.008;
const int I_MAX = 5000;
const int I_MIN = -5000;

// Kalibrečné polia
int sensorMin[NumOfSensors];
int sensorMax[NumOfSensors];
int threshold[NumOfSensors];
int sensorValues[NumOfSensors];

// Počítač pre diagnostiku
int noLineCounter = 0;

// ============= INVERZIA MOTORA =============
// Pravý motor je opačne orientovaný
int invertServo(int value) {
  return 180 - value;
}

// ============= KALIBRÁCIA =============
void calibrate() {
  Serial.println("\n╔══════════════════════════════════════╗");
  Serial.println("║     KALIBRÁCIA ČIDIEL - ŠTART         ║");
  Serial.println("║  Pohybuj robotom nad čiaru a bielu!   ║");
  Serial.println("║  Piny: A26, A27, A28, A29 ✅          ║");
  Serial.println("╚══════════════════════════════════════╝\n");
  
  for (int i = 0; i < NumOfSensors; i++) {
    sensorMin[i] = 4095;
    sensorMax[i] = 0;
  }
  
  // Správna rotácia: ľavý dopredu, pravý dozadu (s inverziou)
  int zmena = 30;
  for (int k = 0; k < 6; k++) {
    Serial.print("Kalibrácia ");
    Serial.print(k + 1);
    Serial.println("/6...");
    
    int leftValue = 90 + zmena;
    int rightValue = invertServo(90 - zmena);  // S inverziou!
    
    leftServo.write(leftValue);
    rightServo.write(rightValue);
    
    for (int j = 0; j < 500; j++) {
      for (int i = 0; i < NumOfSensors; i++) {
        int val = analogRead(pins[i]);
        if (val < sensorMin[i]) sensorMin[i] = val;
        if (val > sensorMax[i]) sensorMax[i] = val;
      }
    }
    delay(500);
    zmena *= -1;
  }
  
  // Zastavenie
  leftServo.write(90);
  rightServo.write(90);
  delay(300);
  
  // ============= DIAGNOSTIKA KALIBRÁCIÍ =============
  Serial.println("\n╔══════════════════════════════════════╗");
  Serial.println("║     VÝSLEDKY KALIBRÁCIÍ               ║");
  Serial.println("╚══════════════════════════════════════╝\n");
  
  boolean calOK = true;
  
  for (int i = 0; i < NumOfSensors; i++) {
    threshold[i] = (sensorMin[i] + sensorMax[i]) / 2;
    int rozsah = sensorMax[i] - sensorMin[i];
    
    Serial.print("SENZOR ");
    Serial.print(i);
    Serial.print(" (A");
    Serial.print(25 + i);
    Serial.print(") | MIN: ");
    Serial.print(sensorMin[i]);
    Serial.print(" | MAX: ");
    Serial.print(sensorMax[i]);
    Serial.print(" | PRAH: ");
    Serial.print(threshold[i]);
    Serial.print(" | ROZSAH: ");
    Serial.println(rozsah);
    
    // DIAGNOSTIKA
    if (rozsah < 50) {
      Serial.println(" ❌ KRITICKÉ: Rozsah príliš malý!");
      calOK = false;
    }
    else if (rozsah < 200) {
      Serial.println(" ⚠️  VAROVANIE: Malý rozsah");
      calOK = false;
    }
    else if (rozsah >= 500) {
      Serial.println(" ✅ DOBRE");
    }
    else {
      Serial.println(" ✓ OK");
    }
  }
  
  Serial.println();
  if (calOK) {
    Serial.println("✅ KALIBRÁCIA ÚSPEŠNÁ!");
  } else {
    Serial.println("⚠️  Problémy s kalibráciou!");
  }
}

// ============= ČÍTANIE SENZOROV =============
int readCalibrated(int i) {
  int val = analogRead(pins[i]);
  
  if (sensorMax[i] == sensorMin[i]) return 0;
  
  int norm = (val - sensorMin[i]) * 1000 / (sensorMax[i] - sensorMin[i]);
  
  if (norm < 0) norm = 0;
  if (norm > 1000) norm = 1000;
  
  return 1000 - norm;  // Čierna = 1000, biela = 0
}
void avoidBrick(){
  rightServo.write(90);
  leftServo.write(150);
  //otoci sa do prava
  rightServo.write(150);
  delay(500);
  //ide rovno
  rightServo.write(0);
  leftServo.write(180);
  delay(1500);
  //ide do lava
  rightServo.write(30);
  leftServo.write(30);
  delay(500);

  //ide rovno
  rightServo.write(0);
  leftServo.write(180);
  delay(2000);

  //otoci sa do lava
  rightServo.write(30);
  leftServo.write(30);
  delay(500);
  //ide rovno
  rightServo.write(0);
  leftServo.write(180);
  delay(1500);
  //otoci sa doprava
  leftServo.write(150);
  rightServo.write(150);
  delay(500);
  return;
}
int readLine() {
  long weightedSum = 0;
  long sum = 0;
  
  for (int i = 0; i < NumOfSensors; i++) {
    int value = readCalibrated(i);
    sensorValues[i] = value;
    
    weightedSum += (long)value * (i * 1000);
    sum += value;
  }
  
  if (sum == 0) return -9999;  // Žiadna čiara
  
  int position = weightedSum / sum;
  int error = position - 1500;  // 1500 = stred
  
  return error;
}

// ============= ULTRAZVUK =============
float scanDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  
  long duration = pulseIn(ECHO, HIGH, 30000);
  
  if (duration == 0) {
    return 500;
  }
  
  float distance = duration * 0.0343 / 2;
  
  if (distance < 0) distance = 500;
  if (distance > 400) distance = 400;
  
  return distance;
}

// ============= PID REGULÁCIA =============
void linefollow(int error) {
  P = error;
  I = I + error;
  
  // Anti-windup
  if (I > I_MAX) I = I_MAX;
  if (I < I_MIN) I = I_MIN;
  
  D = error - previousError;
  previousError = error;
  
  // Fixné koeficienty
  PIDvalue = (Kp * P) + (Ki * I) + (Kd * D);
  
  lsp = lfSpeed - PIDvalue;
  rsp = lfSpeed + PIDvalue;
  
  // Bezpečný rozsah servo
  if (lsp > 120) lsp = 120;
  if (lsp < 70) lsp = 70;
  if (rsp > 120) rsp = 120;
  if (rsp < 70) rsp = 70;
  
  // Zabránenie malým otrasům
  if (lsp > 88 && lsp < 92) lsp = 90;
  if (rsp > 88 && rsp < 92) rsp = 90;
  
  leftServo.write(lsp);
  rightServo.write(invertServo(rsp));  // S inverziou!
}

// ============= EMERGENCY STOP =============
void emergencyStop() {
  Serial.println("🛑 EMERGENCY STOP!");
  leftServo.write(90);
  rightServo.write(90);
  I = 0;
  noLineCounter = 0;
}

// ============= SETUP =============
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  leftServo.attach(LeftServoPin);
  rightServo.attach(RightServoPin);
  
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  
  previousError = 0;
  I = 0;
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║   LINE FOLLOWER - KONEČNÁ VERZIA       ║");
  Serial.println("║   Piny: A26, A27, A28, A29 ✅          ║");
  Serial.println("║   Motory invertované ✅                ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  calibrate();
  
  delay(1000);
  Serial.println("\n>>> SYSTÉM PRIPRAVENÝ - ŠTARTOVANIE <<<\n");
}

// ============= HLAVNÝ LOOP =============
void loop() {
  float distance = scanDistance();
  int error = readLine();
  
  // DEBUG výstup
  Serial.print("DIST:");
  Serial.print(distance);
  Serial.print("cm | SENZORY:[");
  for (int i = 0; i < 4; i++) {
    Serial.print(sensorValues[i]);
    if (i < 3) Serial.print(",");
  }
  Serial.print("] | ");
  
  // ============= BEZPEČNOSŤ =============
  if (distance <= 20) {
    emergencyStop();
    avoidBrick();
    Serial.println("STOP");
  }
  // ============= ČIARA NÁJDENÁ =============
  else if (error != -9999) {
    noLineCounter = 0;
    
    // Detekcia pokyn na stranách
    if (sensorValues[0] > threshold[0] && sensorValues[3] < threshold[3]) {
      // Čiara vľavo → skrč doprava
      lsp = 80;
      rsp = 110;
      leftServo.write(lsp);
      rightServo.write(invertServo(rsp));
      Serial.println("SKRČ DOPRAVA");
    }
    else if (sensorValues[3] > threshold[3] && sensorValues[0] < threshold[0]) {
      // Čiara vpravo → skrč doľava
      lsp = 110;
      rsp = 80;
      leftServo.write(lsp);
      rightServo.write(invertServo(rsp));
      Serial.println("SKRČ DOĽAVA");
    }
    else {
      // Čiara v strede → PID sledovanie
      Serial.print("FOLLOW | ");
      linefollow(error);
    }
  }
  // ============= ČIARA STRATENÁ =============
  else {
    noLineCounter++;
    
    if (noLineCounter < 30) {
      // Pomaly sa otáčaj doprava
      lsp = 92;
      rsp = 88;
      leftServo.write(lsp);
      rightServo.write(invertServo(rsp));
      Serial.println("HĽADÁM ČIARU (doprava)");
    }
    else if (noLineCounter < 60) {
      // Pomaly sa otáčaj doľava
      lsp = 88;
      rsp = 92;
      leftServo.write(lsp);
      rightServo.write(invertServo(rsp));
      Serial.println("HĽADÁM ČIARU (doľava)");
    }
    else {
      // Príliš dlho bez čiary
      emergencyStop();
      Serial.println("CHYBA: ČIARA STRATENÁ");
    }
    Serial.println();
  }
  
  delay(50);
}