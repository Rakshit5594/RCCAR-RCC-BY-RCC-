#include <RF24.h>
#include <SPI.h>

RF24 radio(9, 10);                 // CE, CSN
const byte address[6] = "00001";   // Same as transmitter

char data[10] = "";                // Received command
unsigned long lastSignalTime = 0;  // For failsafe

// Motor control pins (L298N)
const int IN1 = 2;
const int IN2 = 3;
const int IN3 = 4;
const int IN4 = 5;
const int ENA = 6;
const int ENB = 7;

void setup() {
  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Initial motor speed (0–255)
  analogWrite(ENA, 200);
  analogWrite(ENB, 200);

  // NRF24 setup
  radio.begin();
  radio.printDetails();
  radio.openReadingPipe(0, address);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();  // set as receiver
}

void moveForward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void moveBackward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void turnLeft() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}

void turnRight() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(data));
    lastSignalTime = millis();  // update time of last signal
    Serial.print("Received: ");
    Serial.println(data);
if (millis() - lastSignalTime > 3000) {
  radio.stopListening();
  delay(50);
  radio.startListening();
  lastSignalTime = millis();
}
    if (strcmp(data, "forward") == 0) moveForward();
    else if (strcmp(data, "backward") == 0) moveBackward();
    else if (strcmp(data, "left") == 0) turnLeft();
    else if (strcmp(data, "right") == 0) turnRight();
    else if (strcmp(data, "stop") == 0) stopMotors();
  }

  // Failsafe: stop if no command received for 1 second
  if (millis() - lastSignalTime > 1000) {
    stopMotors();
  }
}
