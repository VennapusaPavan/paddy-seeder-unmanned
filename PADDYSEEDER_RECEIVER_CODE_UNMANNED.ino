#include "ACTUATE_HW.h"
void setup()
{
  Serial.begin(9600);
  Serial3.begin(9600);
}

int value = 0;
String inString = "";
String dataReceived = "";
int starIndex = 0;
const int max_str_len = 25; //max 25 characters per line

bool data_update_flag = false;
bool data_fail_flag = false;

String data_obtained = "";

struct dataPacket
{
  int x_direction_value;
  int y_direction_value;
  int propelling_speed;
  int meteringmechanism_status;
  int clutches_disengagement_status;
  int steering_control;
}data_packet;

struct sensorStatus
{
  int steering_value;
  int dogclutch_right;
  int dogclutch_left;
}sensor_status;

// initialize time parameters
unsigned long toc, tic;

//initiate the hardware pins
int prop_mtr_dir_pin = 29;
int prop_mtr_pwm_pin = 10;

// define the L2938 pins for dog clutch actuation
int dog_clutch_in1 = 2, dog_clutch_in2 = 3, dog_clutch_in3 = 4, dog_clutch_in4 = 5;
int limit_sw_pin_right = 8;
int limit_sw_pin_left = 7;

//define steering pin
int steering_sensor_pin = 0;
int steering_dir_pin = 31;
int steering_pwm_pin = 11;

//define pump pin
int meteringmechanism_control_pin = 9;

ACTUATE_HW actuate_HW(prop_mtr_dir_pin, prop_mtr_pwm_pin, dog_clutch_in1, dog_clutch_in2, dog_clutch_in3, dog_clutch_in4, limit_sw_pin_right, limit_sw_pin_left, steering_sensor_pin, steering_dir_pin, steering_pwm_pin, meteringmechanism_control_pin);

//-------------------------------------------------------------------------------------------
void loop()
{
  data_obtained = get_data_packet();
  update_data_packet(data_obtained);

    if(data_update_flag)
    {
      data_update_flag = false;
      print_data_received();
  
      if(data_packet.clutches_disengagement_status == 0)
      {
        actuate_HW.update_dog_clutch_actuation(data_packet.x_direction_value, data_packet.y_direction_value, data_packet.steering_control);
        actuate_HW.update_steering_actuation(data_packet.x_direction_value, data_packet.y_direction_value, data_packet.steering_control);
        actuate_HW.update_prop_mtr_actuation(data_packet.propelling_speed, data_packet.y_direction_value, data_packet.x_direction_value, data_packet.steering_control);
        actuate_HW.update_pump_status(data_packet.meteringmechanism_status);
      }
      else
      {
        bool left_dog_clutch_status = actuate_HW.get_left_dog_clutch_status();
        bool right_dog_clutch_status = actuate_HW.get_right_dog_clutch_status();

        actuate_HW.update_steering_actuation(data_packet.x_direction_value, data_packet.y_direction_value, data_packet.steering_control);
  
        if( (left_dog_clutch_status == true) || (right_dog_clutch_status == true))
        {
          actuate_HW.unload_dog_clutches();
        }
        if(left_dog_clutch_status == true) //true: Dog clutch is engaged, false: Dog clutch is disengaged
        {
          actuate_HW.disengage_left_dog_clutch();
        }
  
        if(right_dog_clutch_status == true) //true: Dog clutch is engaged, false: Dog clutch is disengaged
        {
          actuate_HW.disengage_right_dog_clutch();
        }
      }
      
    }

  if(data_fail_flag)
  {
    Serial.println("DATA_TRANSMISSION_FAILED");
    actuate_HW.update_prop_mtr_actuation(0, 0, 0, 0);
    data_fail_flag = false;
  }
  
}
//-----------------------------------------------------------------------------------------
String get_data_packet()
{
  if(Serial3.available())
  {
    // Initialization
    inString = "";
    dataReceived = "";
    starIndex = 0;
    char readData[max_str_len] = "*************************"; //25 stars
    
    value = Serial3.readBytesUntil('\n', readData, max_str_len);
    inString = readData;
    starIndex = inString.indexOf('*');

    for(int i=0; i<starIndex; i++)
    {
      dataReceived += inString[i];
    }
    
    //Serial.println(dataReceived);
  }
  return dataReceived;
}
//---------------------------------------------------------------------------------------------

void update_data_packet(String _data_obtained)
{
  // Update X_DIRECTION_VALUE
  if(_data_obtained == "X_DIRECTION_VALUE")
  {
    toc = millis();
    while(true)
    {
      _data_obtained = get_data_packet();
      if((_data_obtained != "X_DIRECTION_VALUE") && (_data_obtained != ""))
      {
        data_packet.x_direction_value = _data_obtained.toInt();
        //Serial.print("[dataPacket.x_direction_value:]->");
        //Serial.println(data_packet.x_direction_value);
        break;
      }

      tic = millis();
      if( (tic - toc) > 500)
      {
        data_fail_flag = true;
        Serial.println("DATA_FAILED");
        break;
      }
    }
  }
  // Updated X_DIRECTION_VALUE

  // Update Y_DIRECTION_VALUE
  if(_data_obtained == "Y_DIRECTION_VALUE")
  {
    toc = millis();
    while(true)
    {
      _data_obtained = get_data_packet();
      if((_data_obtained != "Y_DIRECTION_VALUE") && (_data_obtained != ""))
      {
        data_packet.y_direction_value = _data_obtained.toInt();
        //Serial.print("[dataPacket.y_direction_value:]->");
        //Serial.println(data_packet.y_direction_value);
        break;
      }
      
      tic = millis();
      if( (tic - toc) > 500)
      {
        data_fail_flag = true;
        Serial.println("DATA_FAILED");
        break;
      }
    }
  }
  // Updated Y_DIRECTION_VALUE

  // Update PROPELLING_SPEED
  if(_data_obtained == "PROPELLING_SPEED")
  {
    toc = millis();
    while(true)
    {
      _data_obtained = get_data_packet();
      if((_data_obtained != "PROPELLING_SPEED") && (_data_obtained != ""))
      {
        data_packet.propelling_speed = _data_obtained.toInt();
        //Serial.print("[dataPacket.propelling_speed:]->");
        //Serial.println(data_packet.propelling_speed);
        break;
      }

      tic = millis();
      if( (tic - toc) > 500)
      {
        data_fail_flag = true;
        Serial.println("DATA_FAILED");
        break;
      }
    }
  }
  // Updated PROPELLING_SPEED 

  // Update PUMP_STATUS
  if(_data_obtained == "meteringmechanism_STATUS")
  {
    toc = millis();
    while(true)
    {
      _data_obtained = get_data_packet();
      if((_data_obtained != "meteringmechanism_STATUS") && (_data_obtained != ""))
      {
        data_packet.meteringmechanism_status = _data_obtained.toInt();
        //Serial.print("[dataPacket.pump_status:]->");
        //Serial.println(data_packet.pump_status);
        break;
      }

      tic = millis();
      if( (tic - toc) > 500)
      {
        data_fail_flag = true;
        Serial.println("DATA_FAILED");
        break;
      }
    }
  }
  // Updated PUMP_Status

   // Update CLUTCHES_DISENGAGEMENT_STATUS
  if(_data_obtained == "CLUTCHES_STATUS")
  {
    toc = millis();
    while(true)
    {
      _data_obtained = get_data_packet();
      if((_data_obtained != "CLUTCHES_STATUS") && (_data_obtained != ""))
      {
        data_packet.clutches_disengagement_status = _data_obtained.toInt();
        //Serial.print("[dataPacket.clutches_disengagement_status:]->");
        //Serial.println(data_packet.clutches_disengagement_status);
        break;
      }

      tic = millis();
      if( (tic - toc) > 500)
      {
        data_fail_flag = true;
        Serial.println("DATA_FAILED");
        break;
      }
    }
  }
  // Updated Clutches_disengagement_STATUS


  // Update STEERING_CONTROL_VALUE
  if(_data_obtained == "STEERING_CONTROL")
  {
    toc = millis();
    while(true)
    {
      _data_obtained = get_data_packet();
      if((_data_obtained != "STEERING_CONTROL") && (_data_obtained != ""))
      {
        data_packet.steering_control = _data_obtained.toInt();
        //Serial.print("[dataPacket.steering_control:]->");
        //Serial.println(data_packet.steering_control);
        
        data_update_flag = true;
        break;
      }

      tic = millis();
      if( (tic - toc) > 500)
      {
        data_fail_flag = true;
        Serial.println("DATA_FAILED");
        break;
      }
    }
  }
}
//------------------------------------------------------------------------------------------

void print_data_received()
{
    Serial.print("[x_dir_val:]->");
    Serial.print(data_packet.x_direction_value);
    Serial.print(" [y_dir_val:]->");
    Serial.print(data_packet.y_direction_value);
    Serial.print(" [prop_speed:]->");
    Serial.print(data_packet.propelling_speed);
    Serial.print(" [meteringmechanism_status:]->");
    Serial.print(data_packet.pump_status);
    Serial.print(" [clutches_disengage_status:]->");
    Serial.print(data_packet.clutches_disengagement_status);
    Serial.print(" [steeringCtrl:]->");
    Serial.println(data_packet.steering_control);
}
