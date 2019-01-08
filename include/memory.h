#include <EEPROM.h>

#define MEM_SIZE 12
#define MEM_ADDR_TEMP 0
#define MEM_ADDR_MODE 1
#define MEM_ADDR_SPEED 2
#define MEM_ADDR_AIRFLOW 3
#define MEM_ADDR_SLEEP 4
#define MEM_ADDR_SWING 5
#define MEM_ADDR_POWER 6
#define MEM_ADDR_TURBO 7
#define MEM_ADDR_HOLD 8
#define MEM_ADDR_TIMER_SET 9
#define MEM_ADDR_TIMER_DELAY 10
#define MEM_ADDR_TIMER_FROM 11

// Debug to serial
#ifndef DEBUG_MODE
#define DEBUG_MODE false
#endif

// Initialization mode (clears EEPROM)
#ifndef MEMORY_INIT
#define MEMORY_INIT false
#endif

class Memory {
 public:
  void setup(HvacState &state) {
    EEPROM.begin(MEM_SIZE);

    // Clear memory if initialization mode
    if (MEMORY_INIT) {
      Serial.print("[INIT] Clearing EEPROM memory");
      for (int i = 0; i < MEM_SIZE; ++i) {
        EEPROM.write(i, 0);
        Serial.print(".");
      }
      EEPROM.commit();
      delay(100);
      Serial.println("done!");
      Serial.println("Re-upload sketch without MEMORY_INIT flag");
    }

    // Read memory to state
    read(state);
  }

 private:
  void writeInt(int p, int value) {
    if (EEPROM.read(p) != lowByte(value)) EEPROM.write(p, lowByte(value));
  }

 private:
  void writeBool(int p, bool value) {
    if (EEPROM.read(p) != (byte)value) EEPROM.write(p, (byte)value);
  }

 private:
  void dumpMemory() {
    Serial.print("[DEBUG] Memory dump: ");
    for (int i = 0; i < MEM_SIZE; ++i) {
      Serial.print(String(EEPROM.read(i)) + ",");
    }
    Serial.println();
  }

  /**
   * Save current state in EEPROM
   */
 public:
  void save(HvacState state) {
    writeInt(MEM_ADDR_TEMP, (int)state.temperature);
    writeInt(MEM_ADDR_MODE, (int)state.mode);
    writeInt(MEM_ADDR_SPEED, (int)state.airSpeed);
    writeBool(MEM_ADDR_AIRFLOW, state.airFlow);
    writeBool(MEM_ADDR_SLEEP, state.sleepMode);
    writeInt(MEM_ADDR_SWING, (int)state.swing);
    writeBool(MEM_ADDR_POWER, state.power);
    writeBool(MEM_ADDR_POWER, state.turbo);
    writeBool(MEM_ADDR_HOLD, state.hold);
    writeBool(MEM_ADDR_TIMER_SET, state.timerSet);
    writeInt(MEM_ADDR_TIMER_DELAY, (int)state.timerDelay);
    writeInt(MEM_ADDR_TIMER_FROM, (int)state.timerFrom);
    EEPROM.commit();
    delay(100);
    if (DEBUG_MODE) dumpMemory();
  }

  /**
   * Read current state in EEPROM
   */
 public:
  void read(HvacState &state) {
    if (DEBUG_MODE) dumpMemory();

    state.temperature = EEPROM.read(MEM_ADDR_TEMP);
    state.mode = (Mode)EEPROM.read(MEM_ADDR_MODE);
    state.airSpeed = (Speed)EEPROM.read(MEM_ADDR_SPEED);
    state.swing = EEPROM.read(MEM_ADDR_SWING);
    state.airFlow = EEPROM.read(MEM_ADDR_AIRFLOW);
    state.sleepMode = EEPROM.read(MEM_ADDR_SLEEP);
    state.power = EEPROM.read(MEM_ADDR_POWER);
    state.turbo = EEPROM.read(MEM_ADDR_TURBO);
    state.hold = EEPROM.read(MEM_ADDR_HOLD);
    state.timerSet = EEPROM.read(MEM_ADDR_TIMER_SET);
    state.timerDelay = EEPROM.read(MEM_ADDR_TIMER_DELAY);
    state.timerFrom = EEPROM.read(MEM_ADDR_TIMER_FROM);
  }
};
