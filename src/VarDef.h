

volatile uint16_t Count_read_Temperature;
uint16_t Temp, Press, Hum;
int Temp_NTC;
int ADC_NTC_current;
int ADC_NTC_average;

struct FLAGS
{
   bool Read_BME;
   bool Display_NTC;
}Flag;