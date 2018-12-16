#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <Time.h>
#include <TimeLib.h>

//           __    _           
//     _____/ /_  (_)___ _____ 
//    / ___/ __ \/ / __ `/ __ \
//   / /__/ / / / / /_/ / /_/ /
//   \___/_/ /_/_/\__, /\____/ 
//               /____/        

#ifndef CHIGO_DEBUG
#define CHIGO_DEBUG              false
#endif

// Meta
#define CHIGO_FOOTER              "54AB"

// Timers (signal range 0-3)
#define CHIGO_TIMER_SKIP          "FF00"
#define CHIGO_TIMER_NEW_0h        "FB04"
#define CHIGO_TIMER_OLD_0h        "FB04"
#define CHIGO_TIMER_NEW_1h        "7A85"
#define CHIGO_TIMER_OLD_1h        "7E81"
#define CHIGO_TIMER_NEW_2h        "BA45"
#define CHIGO_TIMER_OLD_2h        "BE41"
#define CHIGO_TIMER_NEW_3h        "3AC5"
#define CHIGO_TIMER_OLD_3h        "3EC1"
#define CHIGO_TIMER_NEW_4h        "DA25"
#define CHIGO_TIMER_OLD_4h        "DE21"
#define CHIGO_TIMER_NEW_5h        "5AA5"
#define CHIGO_TIMER_OLD_5h        "5EA1"
#define CHIGO_TIMER_NEW_6h        "9A65"
#define CHIGO_TIMER_OLD_6h        "9E61"
#define CHIGO_TIMER_NEW_7h        "1AE5"
#define CHIGO_TIMER_OLD_7h        "1EE1"
#define CHIGO_TIMER_NEW_8h        "EA15"
#define CHIGO_TIMER_OLD_8h        "EE11"
#define CHIGO_TIMER_NEW_9h        "6A95"
#define CHIGO_TIMER_OLD_9h        "6E91"
#define CHIGO_TIMER_NEW_10h       "AA55"
#define CHIGO_TIMER_OLD_10h       "AE51"
#define CHIGO_TIMER_NEW_11h       "2AD5"
#define CHIGO_TIMER_OLD_11h       "2ED1"
#define CHIGO_TIMER_NEW_12h       "CA35"
#define CHIGO_TIMER_OLD_12h       "CE31"
#define CHIGO_TIMER_NEW_13h       "4AB5"
#define CHIGO_TIMER_OLD_13h       "4EB1"
#define CHIGO_TIMER_NEW_14h       "8A75"
#define CHIGO_TIMER_OLD_14h       "8E71"
#define CHIGO_TIMER_NEW_15h       "0AF5"
#define CHIGO_TIMER_OLD_15h       "0EF1"
#define CHIGO_TIMER_NEW_16h       "F20D"
#define CHIGO_TIMER_OLD_16h       "F609"
#define CHIGO_TIMER_NEW_17h       "728D"
#define CHIGO_TIMER_OLD_17h       "7689"
#define CHIGO_TIMER_NEW_18h       "B24D"
#define CHIGO_TIMER_OLD_18h       "B649"
#define CHIGO_TIMER_NEW_19h       "32CD"
#define CHIGO_TIMER_OLD_19h       "36C9"
#define CHIGO_TIMER_NEW_20h       "D22D"
#define CHIGO_TIMER_OLD_20h       "D629"
#define CHIGO_TIMER_NEW_21h       "52AD"
#define CHIGO_TIMER_OLD_21h       "56A9"
#define CHIGO_TIMER_NEW_22h       "926D"
#define CHIGO_TIMER_OLD_22h       "9669"
#define CHIGO_TIMER_NEW_23h       "12ED"
#define CHIGO_TIMER_OLD_23h       "16E9"
#define CHIGO_TIMER_NEW_24h       "E21D"
#define CHIGO_TIMER_OLD_24h       "E619"

// Extra (signal range 4-7)
#define CHIGO_EXTRA_DEFAULT       "FF00"
#define CHIGO_EXTRA_TURBO         "EF10"
#define CHIGO_EXTRA_HOLD          "DF20"
#define CHIGO_EXTRA_TURBO_HOLD    "CF30"


// Commands (signal range 8-11)
#define CHIGO_CMD_TEMP_UP         "BF40"
#define CHIGO_CMD_TEMP_DOWN       "3FC0"
#define CHIGO_CMD_MODE            "7F80"
#define CHIGO_CMD_SPEED           "5FA0"
#define CHIGO_CMD_SLEEP           "6F90"
#define CHIGO_CMD_POWER           "FF00"
#define CHIGO_CMD_SWING           "DF20"
#define CHIGO_CMD_AIRFLOW         "1FE0"
// TODO: Clean, Lamp

// Power parameters (signal range 12-15)
// Merged with air speed code
#define CHIGO_PARAM_POWEROFF_SWING_0 "E010"
#define CHIGO_PARAM_POWEROFF_SWING_1 "F000"
#define CHIGO_PARAM_POWEROFF_SWING_2 "D020"

// Swing, speed and air flow parameters (signal range 12-15)
// Speed and swing params have to be merged (e.g. 0906 + A050 = A956)
#define CHIGO_PARAM_SPEED_SLOW       "0906" // AirFlow off
#define CHIGO_PARAM_SPEED_MEDIUM     "0D02" // AirFlow off
#define CHIGO_PARAM_SPEED_FAST       "0B04" // AirFlow off
#define CHIGO_PARAM_SPEED_SMART      "0F00" // AirFlow off
#define CHIGO_PARAM_SPEED_AF_SLOW    "010E" // AirFlow on
#define CHIGO_PARAM_SPEED_AF_MEDIUM  "050A" // AirFlow on
#define CHIGO_PARAM_SPEED_AF_FAST    "030C" // AirFlow on
#define CHIGO_PARAM_SPEED_AF_SMART   "0708" // AirFlow on
#define CHIGO_PARAM_SWING_0          "A050" // Sleep Mode off
#define CHIGO_PARAM_SWING_1          "B040" // Sleep Mode off
#define CHIGO_PARAM_SWING_2          "9060" // Sleep Mode off
#define CHIGO_PARAM_SWING_SLEEP_0    "20D0" // Sleep Mode on
#define CHIGO_PARAM_SWING_SLEEP_1    "30C0" // Sleep Mode on
#define CHIGO_PARAM_SWING_SLEEP_2    "10E0" // Sleep Mode on

// Temperature and AC mode parameters (signal range 16-19)
#define CHIGO_PARAM_TEMP_16          "F000"
#define CHIGO_PARAM_TEMP_17          "7080"
#define CHIGO_PARAM_TEMP_18          "B040"
#define CHIGO_PARAM_TEMP_19          "30C0"
#define CHIGO_PARAM_TEMP_20          "D020"
#define CHIGO_PARAM_TEMP_21          "50A0"
#define CHIGO_PARAM_TEMP_22          "9060"
#define CHIGO_PARAM_TEMP_23          "10E0"
#define CHIGO_PARAM_TEMP_24          "E010"
#define CHIGO_PARAM_TEMP_25          "6090"
#define CHIGO_PARAM_TEMP_26          "A050"
#define CHIGO_PARAM_TEMP_27          "20D0"
#define CHIGO_PARAM_TEMP_28          "C030"
#define CHIGO_PARAM_TEMP_29          "40B0"
#define CHIGO_PARAM_TEMP_30          "8070"
#define CHIGO_PARAM_TEMP_31          "00F0"
#define CHIGO_PARAM_TEMP_32          "F000"
#define CHIGO_PARAM_MODE_AUTO        "0F00"
#define CHIGO_PARAM_MODE_COOL        "0B04"
#define CHIGO_PARAM_MODE_COOL_ALT    "030C"
#define CHIGO_PARAM_MODE_HEAT        "0E01"
#define CHIGO_PARAM_MODE_HEAT_ALT    "0609"
#define CHIGO_PARAM_MODE_DRY         "0D02"
#define CHIGO_PARAM_MODE_FAN         "0906"
#define CHIGO_PARAM_MODE_FAN_ALT     "010E"

// Other
#define CHIGO_TEMP_MIN            16U
#define CHIGO_TEMP_MAX            32U

// Default IR send GPIO pin
#ifndef SEND_PIN
#define SEND_PIN                  4 // NodeMCU 4=D2
#endif

// Default IR receive GPIO pin
#ifndef RECV_PIN
#define RECV_PIN                  14 // NodeMCU 14=D5
#endif

// IR signal features
#define SEND_RATE_KHZ             38
#define BAUD_RATE                 115200
#define CAPTURE_BUFFER_SIZE       1024
#define TIMEOUT                   50U
#define MIN_UNKNOWN_SIZE          12

IRsend irsend(SEND_PIN);
IRrecv irrecv(RECV_PIN, CAPTURE_BUFFER_SIZE, TIMEOUT, true);

/**
 * Models
 */

decode_results results;

// Second and fourth chars (0) get replaced
// with current Mode chars
char temperatures[17][5] = {
  CHIGO_PARAM_TEMP_16,
  CHIGO_PARAM_TEMP_17,
  CHIGO_PARAM_TEMP_18,
  CHIGO_PARAM_TEMP_19,
  CHIGO_PARAM_TEMP_20,
  CHIGO_PARAM_TEMP_21,
  CHIGO_PARAM_TEMP_22,
  CHIGO_PARAM_TEMP_23,
  CHIGO_PARAM_TEMP_24,
  CHIGO_PARAM_TEMP_25,
  CHIGO_PARAM_TEMP_26,
  CHIGO_PARAM_TEMP_27,
  CHIGO_PARAM_TEMP_28,
  CHIGO_PARAM_TEMP_29,
  CHIGO_PARAM_TEMP_30,
  CHIGO_PARAM_TEMP_31,
  CHIGO_PARAM_TEMP_32
};

char newTimerDelays[25][5] = {
  CHIGO_TIMER_NEW_0h,
  CHIGO_TIMER_NEW_1h,
  CHIGO_TIMER_NEW_2h,
  CHIGO_TIMER_NEW_3h,
  CHIGO_TIMER_NEW_4h,
  CHIGO_TIMER_NEW_5h,
  CHIGO_TIMER_NEW_6h,
  CHIGO_TIMER_NEW_7h,
  CHIGO_TIMER_NEW_8h,
  CHIGO_TIMER_NEW_9h,
  CHIGO_TIMER_NEW_10h,
  CHIGO_TIMER_NEW_11h,
  CHIGO_TIMER_NEW_12h,
  CHIGO_TIMER_NEW_13h,
  CHIGO_TIMER_NEW_14h,
  CHIGO_TIMER_NEW_15h,
  CHIGO_TIMER_NEW_16h,
  CHIGO_TIMER_NEW_17h,
  CHIGO_TIMER_NEW_18h,
  CHIGO_TIMER_NEW_19h,
  CHIGO_TIMER_NEW_20h,
  CHIGO_TIMER_NEW_21h,
  CHIGO_TIMER_NEW_22h,
  CHIGO_TIMER_NEW_23h,
  CHIGO_TIMER_NEW_24h,
};

char oldTimerDelays[25][5] = {
  CHIGO_TIMER_OLD_0h,
  CHIGO_TIMER_OLD_1h,
  CHIGO_TIMER_OLD_2h,
  CHIGO_TIMER_OLD_3h,
  CHIGO_TIMER_OLD_4h,
  CHIGO_TIMER_OLD_5h,
  CHIGO_TIMER_OLD_6h,
  CHIGO_TIMER_OLD_7h,
  CHIGO_TIMER_OLD_8h,
  CHIGO_TIMER_OLD_9h,
  CHIGO_TIMER_OLD_10h,
  CHIGO_TIMER_OLD_11h,
  CHIGO_TIMER_OLD_12h,
  CHIGO_TIMER_OLD_13h,
  CHIGO_TIMER_OLD_14h,
  CHIGO_TIMER_OLD_15h,
  CHIGO_TIMER_OLD_16h,
  CHIGO_TIMER_OLD_17h,
  CHIGO_TIMER_OLD_18h,
  CHIGO_TIMER_OLD_19h,
  CHIGO_TIMER_OLD_20h,
  CHIGO_TIMER_OLD_21h,
  CHIGO_TIMER_OLD_22h,
  CHIGO_TIMER_OLD_23h,
  CHIGO_TIMER_OLD_24h,
};

enum Mode {
  Auto = 0, Cool, Dry, Heat, Fan
};

enum Speed {
  Slow = 0, Medium, Fast, Smart
};

struct List {
  uint16_t data[197];
  uint16_t counter = 0;
};

/**
 * Hold current state of device
 */
class HvacState {
  public: unsigned temperature = 25;
  public: Mode mode = Auto;
  public: Speed airSpeed = Smart;
  public: bool airFlow = false;
  public: bool sleepMode = false;
  public: unsigned swing = 0;
  public: bool power = true;
  public: bool turbo = false;
  public: bool hold = false;
  public: bool timerSet = false;
  public: unsigned timerDelay = 0;
  public: unsigned long timerFrom = 0;
};

class HvacController {

  private: HvacState defaultState;
  public: HvacState state;
  // TODO: Fix receive blocking when sending signal
  public: bool isSending = false;

  /**
   * Converters
   */

  private: unsigned getTemperatureFromParameter(String param) {
    unsigned temperature = 25;

    // Split into temperature and mode codes
    String tempCode = param;
    String modeCode = param;
    tempCode.setCharAt(1,'0');
    tempCode.setCharAt(3,'0');
    modeCode.setCharAt(0,'0');
    modeCode.setCharAt(2,'0');

    // Match temperature code
    for (int i=0; i<16; i++)
      if (tempCode.equalsIgnoreCase(temperatures[i])) {
        temperature = i + 16;
        // Exception if alt heat param is used, then temp is 32
        if (modeCode.equalsIgnoreCase(CHIGO_PARAM_MODE_HEAT_ALT))
          temperature += 16;
        break;
      }

    return temperature;
  }

  private: unsigned getTimerDelayFromCode(String code) {
    unsigned timerDelay = 0;
    
    for (int i=0; i<25; i++)
    {
      if (code.equalsIgnoreCase(newTimerDelays[i]) || code.equalsIgnoreCase(oldTimerDelays[i]))
        timerDelay = i;
    }
    return timerDelay;
  }

  private: bool getTimerStateFromCode(String code) {
    unsigned isSet = false;
    
    for (int i=0; i<25; i++)
    {
      if (code.equalsIgnoreCase(oldTimerDelays[i]))
        isSet = true;
    }
    return isSet;
  }

  private: char* getExtraAsCode(bool turbo = false, bool hold = false) {
    if (turbo && hold)
      return CHIGO_EXTRA_TURBO_HOLD;
    else if (turbo && !hold)
      return CHIGO_EXTRA_TURBO;
    else if (!turbo && hold)
      return CHIGO_EXTRA_HOLD;
    else 
      return CHIGO_EXTRA_DEFAULT;
  }

  private: bool getTurboFromCode(String param) {
    if (param.equalsIgnoreCase(CHIGO_EXTRA_TURBO_HOLD) || param.equalsIgnoreCase(CHIGO_EXTRA_TURBO))
      return true;
    else
      return false;
  }

  private: bool getHoldFromCode(String param) {
    if (param.equalsIgnoreCase(CHIGO_EXTRA_TURBO_HOLD) || param.equalsIgnoreCase(CHIGO_EXTRA_HOLD))
      return true;
    else
      return false;
  }

  private: char* getPowerAsParameter(bool power) {
    char *param = getCompositeSpeedAsParameter();
    if (!power) {
      char *swingMode;
      switch(state.swing) {
        case 0:
          swingMode = (char*) CHIGO_PARAM_POWEROFF_SWING_0;
        case 1:
          swingMode = (char*) CHIGO_PARAM_POWEROFF_SWING_1;
        case 2:
          swingMode = (char*) CHIGO_PARAM_POWEROFF_SWING_2;
        default:
          swingMode = (char*) CHIGO_PARAM_POWEROFF_SWING_0;
      }
      
      param[0] = swingMode[0];
      param[2] = swingMode[2];
    }
    return param;
  }

  private: bool getPowerFromParameter(String param) {
    // Remove insignificant pieces of information
    param[1] = '0';
    param[3] = '0';
    if (
      param.equalsIgnoreCase(CHIGO_PARAM_POWEROFF_SWING_0) ||
      param.equalsIgnoreCase(CHIGO_PARAM_POWEROFF_SWING_1) ||
      param.equalsIgnoreCase(CHIGO_PARAM_POWEROFF_SWING_2)
      )
    {
      return false;
    }
    else
      return true;
  }

  private: char* getModeAsParameter(Mode mode) {
    switch (mode) {
      case Auto:
        return CHIGO_PARAM_MODE_AUTO;
      case Cool:
        if (state.temperature == 32)
          return CHIGO_PARAM_MODE_COOL_ALT;
        else
          return CHIGO_PARAM_MODE_COOL;
      case Dry:
        return CHIGO_PARAM_MODE_DRY;
      case Heat:
        if (state.temperature == 32)
          return CHIGO_PARAM_MODE_HEAT_ALT;
        else
          return CHIGO_PARAM_MODE_HEAT;
      case Fan:
        if (state.temperature == 32)
          return CHIGO_PARAM_MODE_FAN_ALT;
        else
          return CHIGO_PARAM_MODE_FAN;
    }
  }

  private: Mode getModeFromParameter(String param) {
    // Remove temperature code
    param.setCharAt(0, '0');
    param.setCharAt(2, '0');
    if (param.equalsIgnoreCase(CHIGO_PARAM_MODE_AUTO))
      return Auto;
    if (param.equalsIgnoreCase(CHIGO_PARAM_MODE_COOL) || param.equalsIgnoreCase(CHIGO_PARAM_MODE_COOL_ALT))
      return Cool;
    if (param.equalsIgnoreCase(CHIGO_PARAM_MODE_DRY))
      return Dry;
    if (param.equalsIgnoreCase(CHIGO_PARAM_MODE_HEAT) || param.equalsIgnoreCase(CHIGO_PARAM_MODE_HEAT_ALT))
      return Heat;
    if (param.equalsIgnoreCase(CHIGO_PARAM_MODE_FAN) || param.equalsIgnoreCase(CHIGO_PARAM_MODE_FAN_ALT))
      return Fan;
  }

  private: char* getSpeedAsParameter(Speed airSpeed, bool airFlow) {
    if (airFlow) {
      switch (airSpeed) {
        case Slow:
          return CHIGO_PARAM_SPEED_AF_SLOW;
        case Medium:
          return CHIGO_PARAM_SPEED_AF_MEDIUM;
        case Fast:
          return CHIGO_PARAM_SPEED_AF_FAST;
        case Smart:  
          return CHIGO_PARAM_SPEED_AF_SMART;
      }
    }
    else {
      switch (airSpeed) {
        case Slow:
          return CHIGO_PARAM_SPEED_SLOW;
        case Medium:
          return CHIGO_PARAM_SPEED_MEDIUM;
        case Fast:
          return CHIGO_PARAM_SPEED_FAST;
        case Smart:  
          return CHIGO_PARAM_SPEED_SMART;
      }
    }
  }

  private: Speed getSpeedFromParameter(String param) {
    // Remove 'swing' component from param
    param[0] = '0';
    param[2] = '0';

    // Interpret speed
    if (param.equalsIgnoreCase(CHIGO_PARAM_SPEED_SLOW) || param.equalsIgnoreCase(CHIGO_PARAM_SPEED_AF_SLOW)) {
      return Slow;
    }
      
    if (param.equalsIgnoreCase(CHIGO_PARAM_SPEED_MEDIUM) || param.equalsIgnoreCase(CHIGO_PARAM_SPEED_AF_MEDIUM)) {
      return Medium;
    }
      
    if (param.equalsIgnoreCase(CHIGO_PARAM_SPEED_FAST) || param.equalsIgnoreCase(CHIGO_PARAM_SPEED_AF_FAST)) {
      return Fast;
    }
      
    if (param.equalsIgnoreCase(CHIGO_PARAM_SPEED_SMART) || param.equalsIgnoreCase(CHIGO_PARAM_SPEED_AF_SMART)) {
      return Smart;
    }
  }

  private: bool getAirFlowFromParameter(String param) {
    // Remove 'swing' component from param
    param[0] = '0';
    param[2] = '0';

    if (
      param.equalsIgnoreCase(CHIGO_PARAM_SPEED_AF_SLOW) || 
      param.equalsIgnoreCase(CHIGO_PARAM_SPEED_AF_MEDIUM) || 
      param.equalsIgnoreCase(CHIGO_PARAM_SPEED_AF_FAST) || 
      param.equalsIgnoreCase(CHIGO_PARAM_SPEED_AF_SMART))
    {
      return true;
    }
    else {
      return false;
    }
  }

  private: char* getSwingAsParameter(unsigned swing, bool sleepMode) {
    if (sleepMode) {
      switch (swing) {
        case 0:
          return CHIGO_PARAM_SWING_SLEEP_0;
        case 1:
          return CHIGO_PARAM_SWING_SLEEP_1;
        case 2:
          return CHIGO_PARAM_SWING_SLEEP_2;
      }
    }
    else {
      switch (swing) {
        case 0:
          return CHIGO_PARAM_SWING_0;
        case 1:
          return CHIGO_PARAM_SWING_1;
        case 2:
          return CHIGO_PARAM_SWING_2;
      }
    }
  }

  private: unsigned getSwingFromParameter(String param) {
    // Remove 'speed' component from param
    param[1] = '0';
    param[3] = '0';

    // Interpret swing mode
    if (param.equalsIgnoreCase(CHIGO_PARAM_SWING_0))
      return 0;
    else if (param.equalsIgnoreCase(CHIGO_PARAM_SWING_1))
      return 1;
    else if (param.equalsIgnoreCase(CHIGO_PARAM_SWING_2))
      return 2;
    else
      return 0;
  }

  // Get output parameter from swing, speed and air flow
  private: char* getCompositeSpeedAsParameter() {
    String airSpeedComponent = getSpeedAsParameter(state.airSpeed, state.airFlow);
    String swingComponent = getSwingAsParameter(state.swing, state.sleepMode);
    char compositeSpeed[5];
    compositeSpeed[0] = swingComponent.charAt(0);
    compositeSpeed[1] = airSpeedComponent.charAt(1);
    compositeSpeed[2] = swingComponent.charAt(2);
    compositeSpeed[3] = airSpeedComponent.charAt(3);
    return compositeSpeed;
  }

  private: bool getSleepModeFromParameter(String param) {
    // Remove 'speed' component from param
    param[1] = '0';
    param[3] = '0';

    if (
      param.equalsIgnoreCase(CHIGO_PARAM_SWING_SLEEP_0) || 
      param.equalsIgnoreCase(CHIGO_PARAM_SWING_SLEEP_1) || 
      param.equalsIgnoreCase(CHIGO_PARAM_SWING_SLEEP_2)
      )
    {
      return true;
    }
    else {
      return false;
    }
  }

  /**
   * Decode series of raw signals to hex code
   */
  public: String decodeIRData(const decode_results *results)
  {
      String bits; // output string (temp)
      uint16_t bit_threshold = 1000;
      uint16_t header_len = 4;
      uint16_t footer_len = 2;
      uint16_t body_end = getCorrectedRawLength(results) - footer_len;
      uint32_t usecs;

      // Print received bits (command only)
      if (CHIGO_DEBUG) {
        Serial.print("[DEBUG] Received command (BIN): ");
        for (int i = header_len; i < body_end; i+=2) {
          usecs = results->rawbuf[i] * RAWTICK;
          bool val = (usecs > bit_threshold) ? 1 : 0;
          Serial.print(val);
        }
        Serial.println();
      }

      // Iterate over data (skip header, footer) and convert
      // every 8 datapoints (4 * LOW+HIGH) into an integer code      
      for (int i = header_len; i < body_end; i+=8) {
          char code_str[5]; // 4 digits
          for (int j = 0; j < 7; j+=2)
          {
            usecs = results->rawbuf[i+j] * RAWTICK; // check every second tick pair-wise
            code_str[j/2] = (usecs > bit_threshold) ? '1' : '0'; // If second bit is over threshold, treat as 1 or 0
          }
          uint16_t code = strtol(code_str,0,2); // convert 4-bit binary string to int
          bits += String(code, HEX); // append to string (temp)
      };
      return bits;
  }

  /**
   * Check if the IR code has been received.
   */
  public: HvacState checkIR()
  {
    if (!this->isSending && irrecv.decode(&results)) {
      if (results.overflow)
      {
        Serial.printf("[WARNING] IR code exceeds buffer (>= %d). ", CAPTURE_BUFFER_SIZE);
        Serial.println();
      }

      // Output the message body as HEX
      receiveCommand(decodeIRData(&results));
      yield();
      return state;
    }
  }

  /**
   * Encoding helpers
   */

  private: void addToList(List& data, unsigned int value) {
    data.data[data.counter++] = value;
  }

  private: byte hexToByte(char hex) {
    if (hex >= '0' && hex <= '9') {
      return hex - '0';
    }
    return hex - 'A' + 10;
  }

  private: unsigned int highEndRawData[2] = {500, 1570};

  private: void byteToRawData(byte bytee, List& data) {
    for (int i = 3; i >= 0; --i) {
      addToList(data, highEndRawData[0]);
      addToList(data, highEndRawData[bitRead(bytee, i)]);
    }
  }

  private: void addBytesToData(char* bytes, size_t count, List& data) {
    for (int i = 0; i < count; ++i) {
      byteToRawData(hexToByte(bytes[i]), data);
    }
  }

  private: void addHeaderToData(List& data) {
    addToList(data, 6234);
    addToList(data, 7302);
  }

  private: void addTimerToData(List& data) {
    if (!state.timerSet && state.timerDelay == 0) {
      // Skip timer header if delay hasn't changed      
      addBytesToData(CHIGO_TIMER_SKIP, 4, data);
    }
    else if (state.timerSet && state.timerDelay > 0) {
      // Use old delay header if timer was already set
      addBytesToData(oldTimerDelays[state.timerDelay], 4, data);
    }
    else {
      addBytesToData(newTimerDelays[state.timerDelay], 4, data);
      if (state.timerDelay > 0) {
        state.timerSet = true;
        state.timerFrom = now();
      }
      else {
        state.timerSet = false;
        state.timerFrom = 0;
      }
    }
  }

  private: void addExtraToData(List& data) {
    addBytesToData(getExtraAsCode(state.turbo, state.hold), 4, data);
  }

  private: void addCommandToData(char* command, List& data) {
    addBytesToData(command, 4, data);
  }

  private: void addParameterToData(char* parameter, List& data) {
    addBytesToData(parameter, 4, data);
  }

  private: void addTemperatureAndModeToData(int temp, String mode, List& data) {
    unsigned int realTempIndex = temp - 16;
    char tempAndMode[5] = {0};
    strcpy(tempAndMode, temperatures[realTempIndex]);

    tempAndMode[1] = mode.charAt(1);
    tempAndMode[3] = mode.charAt(3);
    addBytesToData(tempAndMode, 4, data);
  }

  private: void addFooterToData(List& data) {
    addBytesToData(CHIGO_FOOTER, 4, data);

    addToList(data, 608);
    addToList(data, 7372);
    addToList(data, 616);
  }

  public: void dumpState() {
    Serial.println("[DEBUG] Current state");

    String power = state.power ? "on" : "off";
    Serial.println("  power: " + power);

    String turbo = state.turbo ? "on" : "off";
    Serial.println("  turbo: " + turbo);

    String hold = state.hold ? "on" : "off";
    Serial.println("  hold: " + hold);

    String sleepMode = state.sleepMode ? "on" : "off";
    Serial.println("  sleep mode: " + sleepMode);
    
    Serial.println("  temperature: " + String(state.temperature, DEC) + " C");

    String modes[5] = {"auto", "cool", "dry", "heat", "fan"};
    Serial.println("  mode: " + modes[state.mode]);

    String fan_speeds[4] = {"slow", "medium", "fast", "smart"};
    Serial.println("  speed: " + fan_speeds[state.airSpeed]);

    String airFlow = state.airFlow ? "on" : "off";
    Serial.println("  air flow: " + airFlow);

    String swing_modes[3] = {"horizontal", "fixed", "natural"};
    Serial.println("  swing: " + swing_modes[state.swing]);

    if (state.timerSet) {
      Serial.println("  timer: " + String(state.timerDelay, DEC) + "h from " + String(state.timerFrom, DEC));
    }

    Serial.println();
  }

  private: void sendCommand(char* cmd, char* param) {
    List data;
    addHeaderToData(data);

    // TODO: implement Timers
    // addTimerToData(data);
    addBytesToData("FF00", 4, data);

    // TODO: implement Extra modes
    // addExtraToData(data);
    addBytesToData("FF00", 4, data);

    addCommandToData(cmd, data);
    addParameterToData(param, data);
    addTemperatureAndModeToData(state.temperature, getModeAsParameter(state.mode), data);
    addFooterToData(data);
    this->isSending = true;

    if (CHIGO_DEBUG) {
      // Print full IR signal (with header and footer)
      Serial.print("[DEBUG] Sent IR signal: ");
      for (int i=0; i<197; i++) {
        Serial.print(data.data[i]);
        Serial.print(",");
      }
      Serial.println();
      // Print binary (command only)
      Serial.print("[DEBUG] Sent command (BIN): ");
      for (int i=3; i<195; i+=2) {
        if (data.data[i] > 1000)
          Serial.print(1);
        else
          Serial.print(0);
      }
      Serial.println();
    }

    irsend.sendRaw(data.data, data.counter, SEND_RATE_KHZ);
    this->isSending = false;
  }

  private: void receiveCommand(String codes) {

    // Extract individual codes
    String timer = codes.substring(0,4);
    String extra = codes.substring(4,8);
    String cmd = codes.substring(8,12);
    String param = codes.substring(12,16);
    String temp_mode = codes.substring(16,20);
    String footer = codes.substring(20,24);

    if (CHIGO_DEBUG) {
      Serial.println("[DEBUG] Received command (HEX): " + timer + " " + extra + " " + cmd + " " + param + " " + temp_mode + " " + footer);
    }
    
    // Set timer state
    if (!timer.equalsIgnoreCase(CHIGO_TIMER_SKIP)) {

      state.timerSet = getTimerStateFromCode(timer);
      state.timerDelay = getTimerDelayFromCode(timer);

      // Add timer details if delay is new
      if (state.timerDelay > 0 && !state.timerSet) {
        state.timerFrom = now();
        state.timerSet = true;
      }

      // Reset timer if no delay
      if (state.timerDelay == 0) {
        state.timerFrom = 0;
        state.timerSet = false;
      }
    }
    else {
      state.timerDelay = 0;
      state.timerFrom = 0;
      state.timerSet = false;
    }

    // Set extra states
    state.turbo = getTurboFromCode(extra);
    state.hold = getHoldFromCode(extra);

    // Set power state
    // assume "power on" if any other command than "power off"
    state.power = true;
    if (cmd.equalsIgnoreCase(CHIGO_CMD_POWER))
      state.power = getPowerFromParameter(param);

    // Set mode and temperature state (always)
    state.mode = getModeFromParameter(temp_mode);
    state.temperature = getTemperatureFromParameter(temp_mode);

    // Set air speed, air flow, swing and sleep state
    // if command is passed
    if (
      cmd.equalsIgnoreCase(CHIGO_CMD_SPEED) ||
      cmd.equalsIgnoreCase(CHIGO_CMD_AIRFLOW) ||
      cmd.equalsIgnoreCase(CHIGO_CMD_SWING) ||
      cmd.equalsIgnoreCase(CHIGO_CMD_SLEEP)
      )
    {
      state.airSpeed = getSpeedFromParameter(param);
      state.airFlow = getAirFlowFromParameter(param);
      state.sleepMode = getSleepModeFromParameter(param);
      state.swing = getSwingFromParameter(param);
    }

   if (CHIGO_DEBUG)
    dumpState();

  }

  /**
   * Commands
   */

  public: void update() {
    // Any device update has to be send along with "power on" signal
    state.power = true;
    sendCommand(CHIGO_CMD_POWER, getPowerAsParameter(state.power));
  }

  public: void turnOn() {
    // reset state
    // state = defaultState;
    update();
  }

  public: void turnOff() {
    state.power = false;
    sendCommand(CHIGO_CMD_POWER, getPowerAsParameter(state.power));
  }

  public: void setModeTo(Mode mode) {
    state.mode = mode;
    state.power = true;

    // Set default temperature in auto, fan and dry mode
    if (
      mode == Auto ||
      mode == Fan ||
      mode == Dry
      )
    {
      state.temperature = defaultState.temperature;
    }

    sendCommand(CHIGO_CMD_MODE, getCompositeSpeedAsParameter());
  }

  public: void setTimerTo(unsigned timerDelay = 0) {
    state.timerDelay = timerDelay;
    state.timerSet = false;
    update();
  }

  public: int unsigned getTemperature() {
    return state.temperature;
  }

  public: void setTemperatureTo(int unsigned temperature) {
    state.power = true;
    if (temperature >= state.temperature) {
      state.temperature = temperature;
      sendCommand(CHIGO_CMD_TEMP_UP, getCompositeSpeedAsParameter());
    }
    else {
      state.temperature = temperature;
      sendCommand(CHIGO_CMD_TEMP_DOWN, getCompositeSpeedAsParameter());
    }
  }

  public: void holdOn() {
    state.hold = true;
    update();
  }

  public: void holdOff() {
    state.hold = false;
    update();
  }

  public: void turboOn() {
    state.turbo = true;
    update();
  }

  public: void turboOff() {
    state.turbo = false;
    update();
  }

  public: void setAirFlowTo(bool airFlow) {
    state.airFlow = airFlow;
    state.power = true;
    sendCommand(CHIGO_CMD_AIRFLOW, getCompositeSpeedAsParameter());
  }

  public: void setSpeedTo(Speed airSpeed) {
    state.airSpeed = airSpeed;
    state.power = true;
    sendCommand(CHIGO_CMD_SPEED, getCompositeSpeedAsParameter());
  }

  public: void setSwingTo(unsigned swing) {
    state.swing = swing;
    state.power = true;
    sendCommand(CHIGO_CMD_SWING, getCompositeSpeedAsParameter());
  }

  public: void setSleepModeTo(bool sleepMode) {
    state.sleepMode = sleepMode;
    state.power = true;
    sendCommand(CHIGO_CMD_SLEEP, getCompositeSpeedAsParameter());
  }

  public: void setup() {
    // Start serial connection (for logging)
    Serial.begin(BAUD_RATE, SERIAL_8N1, SERIAL_TX_ONLY);
    delay(1000);

    // Ignore messages with less than minimum on or off pulses.
   irrecv.setUnknownThreshold(MIN_UNKNOWN_SIZE);

    // Start the receiver
   irrecv.enableIRIn();

    // Start the sender
    irsend.begin();

    if (CHIGO_DEBUG)
      dumpState();
  }
};
