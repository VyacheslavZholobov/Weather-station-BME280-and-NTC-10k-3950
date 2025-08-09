/*
Author: Vyacheslav Zholobov (slaventiis)
23.12.2019
*/

#include <Arduino.h>
#include "VarDef.h"
#include "Adafruit_BME280.h"
#include "7seg74HC595.h"
#include "ID_scaner_i2c.h"
#include "Interrupt_user.h"
#include "NTC_new_Fast.h"

//#define SERIAL_MONITOR_ON // Comment out if you don't need a serial monitor.

Adafruit_BME280 bme; // use I2C interface
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_Sensor *bme_pressure = bme.getPressureSensor();
Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();

//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
void setup() {
  Wire.begin();
  bme.begin();
  #ifdef SERIAL_MONITOR_ON
  Serial.begin(9600);
  Serial.println(F("BME280 Sensor event test"));

  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
    //while (1) delay(10);
  }

  bme_temp->printSensorDetails();
  bme_pressure->printSensorDetails();
  bme_humidity->printSensorDetails();
  #endif

  Indicator_init();
  Interrupt_init();

  ADC_NTC_average = read_ADC_X10(NTC_PIN); // For quick access to the current temperature
}

void loop() {

  if (Count_read_Temperature < 500) // output 2 to the temperature indicator
  {
    IND_Output_f(2,0,3,3); // deduce 2
  } 
  else if (Count_read_Temperature < 2500 )  // Output data from BME280
  {
    if(Flag.Read_BME == true)
    {
      if (bme.begin())
      {
        Flag.Read_BME = false;
        sensors_event_t temp_event, pressure_event, humidity_event;
        bme_temp->getEvent(&temp_event);
        bme_pressure->getEvent(&pressure_event);
        bme_humidity->getEvent(&humidity_event);
    
        #ifdef SERIAL_MONITOR_ON
        Serial.print(F("Humidity = "));
        Serial.print(humidity_event.relative_humidity);
        Serial.println(" %"); 

        Serial.print(F("Temperature = "));
        Serial.print(temp_event.temperature);
        Serial.println(" *C");

        Serial.print(F("Pressure = "));
        Serial.print(pressure_event.pressure);
        Serial.print(" hPa = "); // hectopascal hPa
        Serial.print(pressure_event.pressure*0.75006375541921);
        Serial.println(" mm Hg"); // Millimeters of mercury column
        Serial.println();
        #endif // SERIAL_MONITOR_ON

        Hum = humidity_event.relative_humidity * 10;
        if (Hum %10 > 4) Hum += 10; // Rounded up
        Temp = temp_event.temperature *10;
        if (Temp %10 > 4) Temp += 10; // Rounded up
        Press = pressure_event.pressure*7.5006375541921;// Pressure value mmHg. *10
        if (Press %10 > 4) Press += 10; // Rounded up

        IND_Output_f(Hum/10,0,6,2);
        IND_Output_f(Temp,1,3,3);
        IND_Output_f(Press/10+1,0,0,3);
      }
      else
      {
        for(uint8_t i = 0; i < IND_AMOUNT_NUM; i++)
        {
          IND_Output_f(88,0,6,2);
          IND_Output_f(888,1,3,3);
          IND_Output_f(888,0,0,3);
        }
      }
    }
  } 
  else if (Count_read_Temperature < 3000) // output 1 to the temperature indicator
  {
    IND_Output_f(1,0,3,3); // output 1
  } 
  else if (Count_read_Temperature < 5000) // output the NTC temperature to the temperature indicator
  {
    if(Flag.Display_NTC == true)
    {
      IND_Output_f(Temp_NTC,1,3,3);
      Flag.Display_NTC = false;
    }
  } 
  else  // We'll reset the meters and raise the flags.
  {
    Count_read_Temperature = 0;
    Flag.Read_BME = true;
    Flag.Display_NTC = true;
  }

  ADC_NTC_current = 0;
  ADC_NTC_current = read_ADC_X10(NTC_PIN);
  ADC_NTC_average += (ADC_NTC_current - ADC_NTC_average) / 16;
  Temp_NTC = calc_temperature_Fast(ADC_NTC_average);

  _delay_ms(1);
}