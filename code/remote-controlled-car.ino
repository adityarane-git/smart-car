/**
 * Filename: remote-controlled-car.ino
 * IR-Controlled Differential Drive Robot Firmware
 * Author: Aditya Rane
 * Platform: Arduino Uno R4
 */

#include <Arduino.h>
#include <IRremote.hpp>

// Pin Definitions
constexpr uint8_t IR_RECEIVE_PIN = 12;
constexpr uint8_t STATUS_LED_PIN = 13;

// L9110 Motor Driver Control Pins
constexpr uint8_t MOTOR_LEFT_IN1  = 5;   // PWM capable
constexpr uint8_t MOTOR_LEFT_IN2  = 6;   // PWM capable
constexpr uint8_t MOTOR_RIGHT_IN1 = 9;   // PWM capable
constexpr uint8_t MOTOR_RIGHT_IN2 = 10;  // PWM capable

// Key Command Definitions (Standard NEC IR Remote Mapping)
constexpr uint8_t CMD_FORWARD  = 0x18;
constexpr uint8_t CMD_BACKWARD = 0x52;
constexpr uint8_t CMD_LEFT     = 0x08;
constexpr uint8_t CMD_RIGHT    = 0x5A;
constexpr uint8_t CMD_STOP     = 0x1C;
constexpr uint8_t CMD_SPEED_UP = 0x15; // Plus button
constexpr uint8_t CMD_SPEED_DN = 0x07; // Minus button

// Speed Configuration Parameters
uint8_t currentSpeed = 180; // Default PWM duty cycle (0-255)
constexpr uint8_t SPEED_STEP = 25;
constexpr uint8_t MIN_SPEED  = 100;
constexpr uint8_t MAX_SPEED  = 255;

// Function Prototypes
void setMotorSpeeds(int leftSpeed, int rightSpeed);
void processCommand(uint8_t command);
void blinkFeedback();

void setup() {
    Serial.begin(9600);
    
    // Configure Motor Pins
    pinMode(MOTOR_LEFT_IN1, OUTPUT);
    pinMode(MOTOR_LEFT_IN2, OUTPUT);
    pinMode(MOTOR_RIGHT_IN1, OUTPUT);
    pinMode(MOTOR_RIGHT_IN2, OUTPUT);
    
    // Configure Status LED
    pinMode(STATUS_LED_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, LOW);
    
    // Initialize Motors to Stopped State
    setMotorSpeeds(0, 0);

    // Initialize IR Receiver
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    Serial.println(F("System Initialized. Awaiting IR Commands..."));
}

void loop() {
    if (IrReceiver.decode()) {
        blinkFeedback();
        
        uint8_t command = IrReceiver.decodedIRData.command;
        Serial.print(F("Received Command: 0x"));
        Serial.println(command, HEX);
        
        processCommand(command);
        
        IrReceiver.resume(); // Clear buffer and receive next signal
    }
}

/**
 * @brief Sets speed and direction for left and right motors using PWM.
 * @param leftSpeed Motor speed (-255 to 255)
 * @param rightSpeed Motor speed (-255 to 255)
 */
void setMotorSpeeds(int leftSpeed, int rightSpeed) {
    // Left Motor Control
    if (leftSpeed > 0) {
        analogWrite(MOTOR_LEFT_IN1, constrain(leftSpeed, 0, 255));
        digitalWrite(MOTOR_LEFT_IN2, LOW);
    } else if (leftSpeed < 0) {
        digitalWrite(MOTOR_LEFT_IN1, LOW);
        analogWrite(MOTOR_LEFT_IN2, constrain(-leftSpeed, 0, 255));
    } else {
        digitalWrite(MOTOR_LEFT_IN1, LOW);
        digitalWrite(MOTOR_LEFT_IN2, LOW);
    }

    // Right Motor Control
    if (rightSpeed > 0) {
        analogWrite(MOTOR_RIGHT_IN1, constrain(rightSpeed, 0, 255));
        digitalWrite(MOTOR_RIGHT_IN2, LOW);
    } else if (rightSpeed < 0) {
        digitalWrite(MOTOR_RIGHT_IN1, LOW);
        analogWrite(MOTOR_RIGHT_IN2, constrain(-rightSpeed, 0, 255));
    } else {
        digitalWrite(MOTOR_RIGHT_IN1, LOW);
        digitalWrite(MOTOR_RIGHT_IN2, LOW);
    }
}

/**
 * @brief Decodes byte command and updates motor states or parameters.
 */
void processCommand(uint8_t command) {
    switch (command) {
        case CMD_FORWARD:
            Serial.println(F("Action: FORWARD"));
            setMotorSpeeds(currentSpeed, currentSpeed);
            break;

        case CMD_BACKWARD:
            Serial.println(F("Action: BACKWARD"));
            setMotorSpeeds(-currentSpeed, -currentSpeed);
            break;

        case CMD_LEFT:
            Serial.println(F("Action: PIVOT LEFT"));
            setMotorSpeeds(-currentSpeed, currentSpeed);
            break;

        case CMD_RIGHT:
            Serial.println(F("Action: PIVOT RIGHT"));
            setMotorSpeeds(currentSpeed, -currentSpeed);
            break;

        case CMD_STOP:
            Serial.println(F("Action: STOP"));
            setMotorSpeeds(0, 0);
            break;

        case CMD_SPEED_UP:
            if (currentSpeed + SPEED_STEP <= MAX_SPEED) {
                currentSpeed += SPEED_STEP;
            } else {
                currentSpeed = MAX_SPEED;
            }
            Serial.print(F("Speed Increased: "));
            Serial.println(currentSpeed);
            break;

        case CMD_SPEED_DN:
            if (currentSpeed - SPEED_STEP >= MIN_SPEED) {
                currentSpeed -= SPEED_STEP;
            } else {
                currentSpeed = MIN_SPEED;
            }
            Serial.print(F("Speed Decreased: "));
            Serial.println(currentSpeed);
            break;

        default:
            Serial.println(F("Unrecognized Command"));
            break;
    }
}

/**
 * @brief Flashes status LED to confirm command reception.
 */
void blinkFeedback() {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(40);
    digitalWrite(STATUS_LED_PIN, LOW);
}