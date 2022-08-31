#include <SoftwareSerial.h>
SoftwareSerial TRANSCEIVER(10, 11); // HC-12 TX Pin, HC-12 RX Pin
int TRANSCEIVER_set_pin = 9;
int y_direction_pin = 1;
int x_direction_pin = 2;
int propelling_speed_pin = 0;
int pump_status_pin = 7;
int clutches_disengagement_status_pin = 6;
int steering_control_pin = 3;
String send_value = "";

int transmit_val = 0;

struct dataPacket
{
  int x_direction_value = 0;
  int y_direction_value = 0;
  int propelling_speed_value = 0;
  int pump_status_value = 0;
  int clutches_disengagement_status_value = 0;
  int steering_control_value = 0;
}data_packet;

void setup() 
{
  Serial.begin(9600);             // Serial port to computer
  TRANSCEIVER.begin(9600);        // Serial port to HC12
  pinMode(TRANSCEIVER_set_pin, OUTPUT);
  digitalWrite(TRANSCEIVER_set_pin, HIGH);

  pinMode(pump_status_pin, INPUT_PULLUP);
  pinMode(clutches_disengagement_status_pin, INPUT_PULLUP);
}
void loop() 
{
  while (TRANSCEIVER.available()) 
  {        // If HC-12 has data
    Serial.write(TRANSCEIVER.read());      // Send the data to Serial monitor
  }
  while (Serial.available()) 
  {      // If Serial monitor has data
    TRANSCEIVER.write(Serial.read());      // Send that data to HC-12
  }

  // upadate the dataPacket and transmit the data
  Serial.println("[UPDATING dataPacket...]");
  update_dataPacket();
  transmit_dataPacket();
  Serial.println("[TRANSMITTED dataPacket]");
  delay(200);
}

int analogReadAverage(int sensor_pin)
{
  int raw_sensor_value = 0;
  int no_samples = 10;
  for(int i=0; i<no_samples; i++)
  {
    raw_sensor_value += analogRead(sensor_pin);
  }
  return raw_sensor_value = raw_sensor_value/no_samples;
}

void update_dataPacket()
{
  data_packet.x_direction_value = analogReadAverage(x_direction_pin);
  data_packet.y_direction_value = analogReadAverage(y_direction_pin);
  data_packet.propelling_speed_value = analogReadAverage(propelling_speed_pin);
  data_packet.pump_status_value = digitalRead(meteringmechanism_status_pin);
  data_packet.clutches_disengagement_status_value = digitalRead(clutches_disengagement_status_pin);
  data_packet.steering_control_value = analogReadAverage(steering_control_pin);
}

void transmit_dataPacket()
{
  TRANSCEIVER.write("X_DIRECTION_VALUE\n");
  transmit_val = map(data_packet.x_direction_value, 0, 1024, -45, 46);
  TRANSCEIVER.println(String(transmit_val));
  Serial.print("[X] ");
  Serial.print(transmit_val);
  
  TRANSCEIVER.write("Y_DIRECTION_VALUE\n");
  transmit_val = map(data_packet.y_direction_value, 3, 1024, -10, 11);
  TRANSCEIVER.println(String(transmit_val));
  Serial.print(" [Y] ");
  Serial.print(transmit_val);

  TRANSCEIVER.write("PROPELLING_SPEED\n");
  transmit_val = map(data_packet.propelling_speed_value, 0, 1024, 0, 255);
  TRANSCEIVER.println(String(transmit_val));
  Serial.print(" [Prop Spd] ");
  Serial.print(transmit_val);

  TRANSCEIVER.write("meteringmechanism_status\n");
  transmit_val = data_packet.meteringmechanism_status_value;
  TRANSCEIVER.println(String(transmit_val));
  Serial.print(" [meteringmechanism Status] ");
  Serial.print(transmit_val);

  TRANSCEIVER.write("CLUTCHES_STATUS\n");
  transmit_val = data_packet.clutches_disengagement_status_value;
  TRANSCEIVER.println(String(transmit_val));
  Serial.print(" [Clutches Status] ");
  Serial.print(transmit_val);

  TRANSCEIVER.write("STEERING_CONTROL\n");
  transmit_val = map(data_packet.steering_control_value, 20, 980, -46, 46);
  TRANSCEIVER.println(String(transmit_val));
  Serial.print(" [Steering Value] ");
  Serial.println(transmit_val);
}
