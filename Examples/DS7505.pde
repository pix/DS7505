/*
* DS7505 Library
* Kerry D. Wong
* http://www.kerrywong.com
* 3/2011
*/
#include <Wire.h>
#include <DS7505.h>

DS7505 ds7505;

void setup()
{
    Serial.begin(9600);

    Wire.begin();

    //initializes temp sensor with 12 bit resolution
    //the I2C address is 0 0 0 in this case
    //(pin 5, 6, 7 are tied to ground)
    ds7505.init(0,0,0,12);

    //set the thermostat at 32.45 (Celsius) with a hysteresis of 30.14 degree
    ds7505.setThermostatC(32.45f, 30.14f, DS7505::FT_6);

    //the temperatures read back will be slightly off depending on the resolution
    //settings
    Serial.println(ds7505.getTempC(DS7505::P_TOS));
    Serial.println(ds7505.getTempC(DS7505::P_THYST));

    //make the settings permanent (write to NV memory)
    ds7505.sendCommand(DS7505::CMD_COPY_DATA);
}


void loop()
{
  delay(500);

  //print the current temperature in Fahrenheit
  Serial.println(ds7505.getTempF());
}
