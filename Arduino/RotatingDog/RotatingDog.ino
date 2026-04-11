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
static bool located = LOW;
static float CurDistance = 0.0f;
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
  servoLeftWheel.write(110);   
  servoRightWheel.write(70);    
  Serial.println(">>> Dopredu");
}

void moveBackward() {
  servoLeftWheel.write(0);     
  servoRightWheel.write(180);  
  Serial.println("<<< Dozadu");
}

void rotateLeft(int angle) {
  // nastav kolieska pre otacanie vlavo
  servoLeftWheel.write(110);   // dozadu
  servoRightWheel.write(110);   // dopredu
  Serial.print("↻ Otočenie vľavo o ");
  Serial.print(angle);
  Serial.println(" stupnov");

  // vypocet casu otacania (ms) - doladit experimentom
  int durationMs = angle * 14; // 14 ms na 1 stupeň, kalibrovat
  delay(durationMs);

  stopRobot(); // zastavenie po otacani
}

void rotateRight(int angle) {
  // nastav kolieska pre otacanie doprava
  servoLeftWheel.write(70);    // dopredu
  servoRightWheel.write(70);  // dozadu
  Serial.print("↻ Otočenie vpravo o ");
  Serial.print(angle);
  Serial.println(" stupnov");

  // vypocet casu otacania (ms) - doladit experimentom
  int durationMs = angle * 14; // 14 ms na 1 stupeň, kalibrovat
  delay(durationMs);

  stopRobot(); // zastavenie po otacani
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  delay(1000);
  Serial.println("\n=== SETUP START ===");
  
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  
  servoLeftWheel.attach(PIN_SERVO_LEFT_WHEEL, 500, 2500);
  servoRightWheel.attach(PIN_SERVO_RIGHT_WHEEL, 500, 2500);
  servoScaner.attach(PIN_SERVO_SCANER, 500, 2500);
  
  delay(500);
  servoScaner.write(90);

  
  if (servoScaner.read() == 90) {
    Serial.println("=== SETUP DONE ===\n");
  }
  else {
    Serial.println("=== ERROR IN SCANER ===\n");
    while(true){};
  }
  servoLeftWheel.write(90);
  servoRightWheel.write(90);
  
  /*float curMin = 50000.0f;
  int curMinIndex = -1;
  
  for (int i = 0; i < 18; i++){
    servoScaner.write(i*10+5);
    delay(200);
    CurDistance = scanDistance();
    if (CurDistance != -1.0f && curMin < CurDistance){
      curMin = CurDistance;
      curMinIndex = i;
    }
  }
  servoScaner.write(curMinIndex*10+5);*/
  
}

void loop() {
  // krátka pauza medzi skenmi
  delay(400);

  // zmeraj aktuálnu vzdialenosť pred robotom
  CurDistance = scanDistance();
  located = CurDistance != -1 && CurDistance < MIN_SAFE_DISTANCE;
  Serial.print("Located: ");
  Serial.println(located);

  if (!located) {
    // 🔍 Skenovanie od 5° do 175° (scanner servo)
    int startAngle = -1;
    int endAngle = -1;
    bool inObstacle = false;

    for (int angle = 5; angle <= 175; angle++) {
      servoScaner.write(angle);
      delay(20);  // nech servu stihne dojst na poziciu

      CurDistance = scanDistance();

      if (CurDistance != -1 && CurDistance < 40) {
        // sme na prekážke
        if (!inObstacle) {
          startAngle = angle;
          inObstacle = true;
        }
        endAngle = angle;
      } else {
        // mimo prekážky
        if (inObstacle) {
          break;  // našli sme celý objekt
        }
      }
    }

    // Ak sa nenašla prekážka
    if (startAngle == -1 || endAngle == -1) {
      Serial.println("Ziadna prekazka nenajdena");
      return;
    }

    // vypocet stredu prekážky
    int targetAngle = (startAngle + endAngle) / 2;
    servoScaner.write(targetAngle);
    Serial.print("Start: "); Serial.println(startAngle);
    Serial.print("End: "); Serial.println(endAngle);
    Serial.print("Center: "); Serial.println(targetAngle);

    // 🎯 otočenie robota na stred prekážky
    int diff = targetAngle - 90;
    if (diff > 0) {
      rotateRight(diff);
    } 
    else if (diff < 0){
      rotateLeft(-diff);
    }

    // zastav robot
    stopRobot();
    servoScaner.write(90);
    // 🎯 vráť scanner dopredu
    delay(300);

    // 🚗 pohyb k prekážke
    while (true) {
      CurDistance = scanDistance();
      if (CurDistance == -1) continue;

      if (CurDistance <= 10.0) {
        stopRobot();
        Serial.println("Dosiahnuty ciel");
        break;
      }

      moveForward();
      delay(100);
    }
  }
}
