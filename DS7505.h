#ifndef DS7505_H
#define DS7505_H

#include <inttypes.h>
#include <WProgram.h>

/*! \mainpage DS7505 Library
 *
 * For use with MAXIM's DS7505: http://www.maxim-ic.com/datasheet/index.mvp/id/5730
 * Originaly written by Kerry D. Wong: http://www.kerrywong.com 3/2011
 *
 * \code
 *
 *  DS7505 ds7505;
 *
 *  Wire.begin();
 *  // Initializes temp sensor with 12 bit resolution
 *  // the I2C address is 0 0 0 in this case
 *  // (pin 5, 6, 7 are tied to ground)
 *  ds7505.init(0, 0, 0, DS7505::RES_12);
 *
 *  // Set the thermostat at 32.45 (Celsius) with a hysteresis of 30.14 degree
 *  ds7505.setThermostatC(32.45f, 30.14f, DS7505::FT_6);
 *
 *  // The temperatures read back will be slightly off depending on the resolution
 *  // settings
 *  Serial.println(ds7505.getTempC(DS7505::P_TOS));
 *  Serial.println(ds7505.getTempC(DS7505::P_THYST));
 *
 *  // Make the settings permanent (write to NV memory)
 *  ds7505.sendCommand(DS7505::CMD_COPY_DATA);
 *
 *  Serial.println(ds7505.getTempF());
 *
 * \endcode
 */

class DS7505
{

public:

  //! Resolution configuration
  enum Resolution {
    RES_09 = 0x0, /*!<  9 bit res */
    RES_10 = 0x1, /*!< 10 bit res */
    RES_11 = 0x2, /*!< 11 bit res */
    RES_12 = 0x3, /*!< 12 bit res */
  };

  //! Fault Tolerance Configuration
  /*!
   * In both operating modes, the user can program the
   * thermostat-fault tolerance, which sets how many con-
   * secutive temperature readings (1, 2, 4, or 6) must fall
   * outside the thermostat limits before the thermostat out-
   * put is triggered. The fault tolerance is set by the F1 and
   * F0 bits in the configuration register. The default factory
   * power-up setting for fault tolerance is 1 (F1 = 0, F0 = 0).
   */
  enum FaultTolerance {
    FT_1 = 0x0, /*!< fault tolerance consecutive out of limits 1 */
    FT_2 = 0x1, /*!< fault tolerance consecutive out of limits 2 */
    FT_4 = 0x2, /*!< fault tolerance consecutive out of limits 4 */
    FT_6 = 0x3, /*!< fault tolerance consecutive out of limits 6 */
  };

  //! Registers Pointer Definition
  enum Register {
    P_TEMP = 0x0, // temperature
    P_CONF = 0x1, // configuration
    P_THYST = 0x2, // Thyst
    P_TOS = 0x3, // Tos
  };

  //! Comand
  enum Command {
    // command set
    CMD_RECALL_DATA = 0xB8,
    CMD_COPY_DATA = 0x48,
    CMD_POR = 0x54,
  };

  //! Default constructor.
  DS7505() {};

  //! Get the current temperature in Celsius. */
  float getTempC() { return getTemp(P_TEMP); }

  //! Get the current temperature in Fahrenheit */
  float getTempF() { return 9.0/5.0 * getTemp(P_TEMP) + 32.0; }

  //! Get the temperature specified by \ref refPdef in Celsius */
  /*!
   * \param regPdef
   *   P_TEMP: get temperature in Celsius
   *   P_THYST: get hysteresis temperature
   *   P_OS: get trip temperature
   */
  float getTempC(Register regPdef) { return getTemp(regPdef); }

  //! Get the temperature specified by \ref regPdef in Fahrenheit */
  /*!
   * \param regPdef
   *   P_TEMP: get temperature in Celsius
   *   P_THYST: get hysteresis temperature
   *   P_OS: get trip temperature
   */
  float getTempF(Register regPdef) { return 9.0/5.0 * getTemp(regPdef) + 32.0;}

  //! Sets the thermostat trip temperature in Celsius */
  /*!
   * Hysteresis is set to be 5 degree below trip temperature
   * \param t The temperature
   */
  void setThermostatC(float t) { setThermostatC(t, t - 5.0); }

  //! Sets the thermostat temperature in Fahrenheit
  /*!
   * Hysteresis is set to be 5 degree below trip temperature
   * \param t The temperature
   */
  void setThermostatF(float t) { setThermostatF(t, t - 5.0); }

  //! Sets the thermostat temperature in Celsius
  /*!
   * \param tos trip temperature
   * \param thyst hysteresis temperature
   */
  void setThermostatC(float tos, float thyst) { setThermostatC(tos, thyst, FT_1); }

  //! Sets the thermostat temperature in Fahrenheit
  /*!
   * \param tos trip temperature
   * \param thyst hysteresis temperature
   */
  void setThermostatF(float tos, float thyst) { setThermostatF(tos, thyst, FT_1); }

  //! Sets the thermostat temperature in Celsius
  /*!
   * \param tos trip temperature
   * \param thyst hysteresis temperature
   * \param ft fault tolerance (consecutive out-of-limits conversions before tripping)
   */
  void setThermostatC(float tos, float thyst, FaultTolerance ft) { setThermostat(tos, thyst, ft); }

  //! Sets the thermostat temperature in Fahrenheit
  /*!
   * \param tos trip temperature
   * \param thyst hysteresis temperature
   * \param ft fault tolerance (consecutive out-of-limits conversions before tripping)
   */
  void setThermostatF(float tos, float thyst, FaultTolerance ft) { setThermostat((tos - 32.0) * 5.0 / 9.0, (thyst - 32.0) * 5.0 / 9.0, ft); }

  //! Sets the configuration register
  /*!
   * \param configByte
   *   NVB: NV Memory Status
   *   R1: Conversion Resolution Bit 1
   *   R2: Conversion Resolution Bit 2
   *   F1: Thermostat Fault Tolerance Bit 1
   *   F2: Thermostat Fault Tolerance Bit 2
   *   POL: Thermostat Output (O.S.) Polarity
   *   TM: Thermostat Operating Mode
   *   SD: Shutdown
   *   [ NVB R1 R0 F1 F0 POL TM SD] (see DS7505 data-sheet)
   */
  void setConfigRegister(uint8_t configByte);

  //! Send a command
  /*!
   * \param cmdSet
   * 	CMD_RECALL_DATA
   * 	CMD_COPY_DATA
   * 	CMD_POR
   */
  void sendCommand(uint8_t cmdSet);

  //! initialization
  /*!
   * \param a2 MSB of the hardware configured I2C address
   * \param a1 Bit a1 of the hardware configured I2C address
   * \param a0 LSB of the hardware configured I2C address
   * \param res The temperature resolution (9, 10, 11 or 12 bits)
   */
  void init(uint8_t a2, uint8_t a1, uint8_t a0, Resolution res);

private:
  uint8_t _i2cAddr;
  uint8_t _configByte;

  //! Gets the temperature in Celsius from the specified register
  /*!
   * \param regPdef
   *   P_TEMP: get temperature in Celsius
   *   P_THYST: get hysteresis temperature
   *   P_OS: get trip temperature
   */
  float getTemp(Register regPdef);

  //! Sets the thermostat temperature in Celsius
  /*!
   * \param tos trip temperature
   * \param thyst hysteresis temperature
   * \param ft fault tolerance (consecutive out-of-limits conversions before tripping)
   */
  void setThermostat(float tos, float thyst, FaultTolerance ft);
};

#endif
