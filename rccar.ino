#include <RF24.h>
#include <SPI.h>

RF24 radio(9, 8);                 // CE, CSN pins for NRF24L01
const byte address[6] = "00001";  // Communication channel

char command[10] = "";            // Stores the direction string

// Joystick pins
int vrx = A0;
int vry = A1;
int sw = 2;

void setup() {
  Serial.begin(9600);
  pinMode(vrx, INPUT);
  pinMode(vry, INPUT);
  pinMode(sw, INPUT_PULLUP);  // internal pull-up for joystick button

  // NRF24 setup
  radio.begin();
  radio.openWritingPipe(address);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();  // set as transmitter
}

void loop() {
  int vrxval = analogRead(vrx);  // X-axis (Forward/Backward)
  int vryval = analogRead(vry);  // Y-axis (Left/Right)
  int swval = digitalRead(sw);   // Button (optional)

  // Dead zone range for stability
  const int centerMin = 470;
  const int centerMax = 550;

  // Default stop
  strcpy(command, "stop");

  // Determine direction
  if (vrxval > centerMax && vrxval <= 1023)
    strcpy(command, "forward");
  else if (vrxval < centerMin && vrxval >= 0)
    strcpy(command, "backward");
  else if (vryval < centerMin && vryval >= 0)
    strcpy(command, "left");
  else if (vryval > centerMax && vryval <= 1023)
    strcpy(command, "right");

  // Send command
  radio.write(&command, sizeof(command));

  // Debugging
  Serial.print("VRX: "); Serial.print(vrxval);
  Serial.print("  VRY: "); Serial.print(vryval);
  Serial.print("  -> Command: "); Serial.println(command);

  delay(150);  // Small delay to prevent flooding
}
