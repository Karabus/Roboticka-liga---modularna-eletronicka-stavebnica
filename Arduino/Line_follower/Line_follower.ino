#include <Servo.h>
int pins[4] = {26,27,28,29}; 

Servo leftServo;
Servo rightServo;

const int NumOfSensors = 4;
const int BuzzerPin = 0;
const int LeftServoPin = 12;
const int RightServoPin = 13;
int sensorMin[NumOfSensors];
int sensorMax[NumOfSensors];
int sensorValues[NumOfSensors];

void initCalibration() {
  for (int i = 0; i < NumOfSensors; i++) {
    sensorMin[i] = 4095;
    sensorMax[i] = 0;
  }
  
}

void calibrate() {
  for (int i = 0; i < NumOfSensors; i++) {
    int val = analogRead(pins[i]);

    if (val < sensorMin[i]) sensorMin[i] = val;
    
    if (val > sensorMax[i]) sensorMax[i] = val;
  }
}

int readLine() {
  long weightedSum = 0;
  long sum = 0;

  for (int i = 0; i < NumOfSensors; i++) {
    int value = readCalibrated(i);
    sensorValues[i] = value;

    // pozície: 0, 1000, 2000, 3000
    weightedSum += (long)value * (i * 1000);
    sum += value;
  }

  if (sum == 0) return 0; // nič nevidí → rovno

  int position = weightedSum / sum;

  // stred je medzi 1. a 2. senzorom → 1500
  int error = position - 1500;

  return error;
}

int readCalibrated(int i) {
  int val = analogRead(pins[i]);

  // ochrana proti deleniu nulou
  if (sensorMax[i] == sensorMin[i]) return 0;

  int norm = (val - sensorMin[i]) * 1000 / (sensorMax[i] - sensorMin[i]);

  // clamp
  if (norm < 0) norm = 0;
  if (norm > 1000) norm = 1000;

  // invert: čierna = 1000
  return 1000 - norm;
}

void setup() {
  Serial.begin(115200);

  initCalibration();
  
  leftServo.attach(LeftServoPin);
  rightServo.attach(RightServoPin);

  Serial.println("Kalibrujem");
  // kalibrácia ~3 sekundy
  for (int i = 0; i < 300; i++) {
    calibrate();
    delay(20);
  }

  Serial.println("Kalibracia hotova");
}
void loop() {
  int error = readLine();

  int baseSpeed = 90;   // neutrál (pre continuous rotation servo ~90)
  int maxCorrection = 40;

  // zosilnenie chyby
  int correction = error / 25;  
  correction = constrain(correction, -maxCorrection, maxCorrection);

  int leftSpeed  = baseSpeed + correction;
  int rightSpeed = baseSpeed - correction;

  // pre continuous servá:
  leftServo.write(leftSpeed);
  rightServo.write(rightSpeed);

  Serial.print("Error: ");
  Serial.print(error);
  Serial.print("\tL: ");
  Serial.print(leftSpeed);
  Serial.print("\tR: ");
  Serial.println(rightSpeed);

  delay(20);
}