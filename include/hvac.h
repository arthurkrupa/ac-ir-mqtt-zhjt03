#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <Time.h>
#include <TimeLib.h>

// Debug to serial
#ifndef DEBUG_MODE
#define DEBUG_MODE                false
#endif

// Enable EEPROM memory
#ifndef MEMORY_MODE
#define MEMORY_MODE               true
#endif

// Default IR send GPIO pin
#ifndef SEND_PIN
#define SEND_PIN                  15 // NodeMCU 15=D8
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

decode_results results;
Memory memory;

struct List {
  uint16_t data[197];
  uint16_t counter = 0;
};

/**
 * Main controller
 */
class HvacController {

  private: HvacState defaultState;
  public: HvacState state;
  // TODO: Fix receive blocking when sending signal
  public: bool isSending = false;

  /**
   * Converters
   */

  private: unsigned getTemperatureFromParameter(String param, unsigned temperature) {
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

  private: unsigned getTimerDelayFromCode(String code, unsigned timerDelay) {
    // unsigned timerDelay = 0;
    
    for (int i=0; i<25; i++)
    {
      if (code.equalsIgnoreCase(newTimerDelays[i]) || code.equalsIgnoreCase(oldTimerDelays[i]))
        timerDelay = i;
    }
    return timerDelay;
  }

  private: bool getTimerStateFromCode(String code, boolean isSet) {
    // boolean isSet = false;
    
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

  private: Mode getModeFromParameter(String param, Mode previousMode) {
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
    return previousMode;
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

  private: uint16_t bit_threshold = 1000;
  private: uint16_t header_len = 4;
  private: uint16_t footer_len = 2;

  private: char toBit(uint32_t usecs) {
    return (usecs > bit_threshold) ? '1' : '0';
  }

  /**
   * Check IR data header and footer
   */
  public: bool verifyIRData(const decode_results *results)
  {
      uint16_t footer_start = getCorrectedRawLength(results) - footer_len + 1;
      uint32_t usecs;

      // Check header (2 bits in 4 signals)
      char header[] = "1010";
      for (int i = 0; i < header_len; i++) {
        usecs = results->rawbuf[i] * RAWTICK;
        if (header[i] == toBit(usecs)) {
          if (DEBUG_MODE)
            Serial.print("[DEBUG] Incorrect header");
          return false;
        }
      }

      // Check footer (2 bits in 3 signals)
      char footer[] = "010";
      for (int i = 0; i < footer_len+1; i++) {
        usecs = results->rawbuf[i+footer_start] * RAWTICK;
        if (footer[i] == toBit(usecs)) {
          if (DEBUG_MODE)
            Serial.print("[DEBUG] Incorrect footer");
          return false;
        }
      }

      return true;
  }

  /**
   * Decode series of raw signals to hex code
   */
  public: String decodeIRData(const decode_results *results)
  {
      String bits; // output string (temp)
      uint16_t body_end = getCorrectedRawLength(results) - footer_len;
      uint32_t usecs;

      // Print received bits (command only)
      if (DEBUG_MODE) {
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

      if (verifyIRData(&results)) {
        // Output the message body as HEX
        receiveCommand(decodeIRData(&results));
        yield();

        // Update memory based on IR signal
        if (MEMORY_MODE) {
          updateMemory();
        }
      }

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

  private: void addBytesToData(String bytes, size_t count, List& data) {
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

  private: void addCommandToData(String command, List& data) {
    addBytesToData(command, 4, data);
  }

  private: void addParameterToData(String parameter, List& data) {
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

  private: void sendCommand(String cmd, char* param) {
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

    if (DEBUG_MODE) {
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

    if (DEBUG_MODE) {
      Serial.println("[DEBUG] Received command (HEX): " + timer + " " + extra + " " + cmd + " " + param + " " + temp_mode + " " + footer);
    }
    
    // Set timer state
    if (!timer.equalsIgnoreCase(CHIGO_TIMER_SKIP)) {

      state.timerSet = getTimerStateFromCode(timer, state.timerSet);
      state.timerDelay = getTimerDelayFromCode(timer, state.timerDelay);

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
    state.mode = getModeFromParameter(temp_mode, state.mode);
    state.temperature = getTemperatureFromParameter(temp_mode, state.temperature);

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

   if (DEBUG_MODE)
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

  public: void updateMemory() {
    memory.save(state);
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

    // Initialize EEPROM
    if (MEMORY_MODE) {
      memory.setup(state);
    }

    // Dump state in debug mode
    if (DEBUG_MODE) {
      dumpState();
    }
  }
};
