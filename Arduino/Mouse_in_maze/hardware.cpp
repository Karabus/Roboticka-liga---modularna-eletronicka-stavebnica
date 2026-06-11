#include "hardware.h"

VL53L0X         sensors[3];
Adafruit_BNO055 bno(55, 0x28, &Wire1);
Servo           servoLeft;
Servo           servoRight;
bool            gyroActive = false;

const int     xshutPins[3]   = {0, 1, 2};
const uint8_t sensorAddrs[3] = {0x30, 0x31, 0x32};

// ============================================================
// SETUP I2C (Wire — hlavná zbernica pre VL53L0X)
// ============================================================
void setupI2C() {
    Wire.setSDA(I2C_SDA);
    Wire.setSCL(I2C_SCL);
    Wire.begin();
    delay(100);
}

// ============================================================
// SETUP GYROSKOP (Wire1 — druhá zbernica pre BNO055)
// ============================================================
bool setupGyroscope() {
    Wire1.setSDA(BNO_SDA);
    Wire1.setSCL(BNO_SCL);
    Wire1.begin();
    delay(100);

    Serial.println("Hladam BNO055...");

    int foundAddr = -1;
    for (byte addr = 1; addr < 127; addr++) {
        Wire1.beginTransmission(addr);
        if (Wire1.endTransmission() == 0) {
            Serial.print("Najdene na adrese: 0x");
            Serial.println(addr, HEX);
            foundAddr = addr;
            break;
        }
    }

    if (foundAddr < 0) {
        Serial.println("BNO055: nenajdene!");
        return false;
    }

    bno = Adafruit_BNO055(55, foundAddr, &Wire1);

    if (!bno.begin()) {
        Serial.println("BNO055 begin() zlyhalo!");
        return false;
    }

    gyroActive = true;
    Serial.println("BNO055 OK");
    return true;
}

// ============================================================
// SETUP VL53L0X (používa Wire inicializovaný cez setupI2C)
// ============================================================
bool setupLasers() {
    // Wire už inicializovaný v setupI2C() — nevoláme znovu begin()
    for (int i = 0; i < 3; i++) {
        pinMode(xshutPins[i], OUTPUT);
        digitalWrite(xshutPins[i], LOW);
    }
    delay(10);

    for (int i = 0; i < 3; i++) {
        digitalWrite(xshutPins[i], HIGH);
        delay(10);

        sensors[i].setBus(&Wire);

        if (!sensors[i].init()) {
            Serial.print("VL53L0X #");
            Serial.print(i);
            Serial.println(" nenajdeny!");
            return false;
        }

        sensors[i].setAddress(sensorAddrs[i]);

        Serial.print("VL53L0X #");
        Serial.print(i);
        Serial.print(" OK → 0x");
        Serial.println(sensorAddrs[i], HEX);
    }

    for (int i = 0; i < 3; i++) sensors[i].startContinuous(50);

    Serial.println("Vsetky senzory OK");
    return true;
}

// ============================================================
// SETUP SERVO
// ============================================================
void setupServos() {
    servoLeft.attach(SERVO_LEFT_PIN);
    servoRight.attach(SERVO_RIGHT_PIN);
    servoLeft.write(STOP);  
    servoRight.write(STOP);
}

// ============================================================
// ČÍTANIE SENZOROV
// ============================================================
SensorData readSensors() {
    SensorData data;

    data.left  = sensors[0].readRangeContinuousMillimeters();
    data.front = sensors[1].readRangeContinuousMillimeters();
    data.right = sensors[2].readRangeContinuousMillimeters();

    if (gyroActive) {
        sensors_event_t event;
        bno.getEvent(&event);
        data.heading = event.orientation.x;
        data.dir     = (int)(data.heading / 90.0f) % 4;
    } else {
        data.heading = -1.0f;
        data.dir     = 0;
    }

    return data;
}