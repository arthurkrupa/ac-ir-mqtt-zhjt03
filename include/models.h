/**
 * Temperature
 * Second and fourth chars (0) get replaced with current Mode chars
 */
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

/**
 * State of device
 */
class HvacState {
  public: unsigned temperature = 25;
  public: Mode mode = Auto;
  public: Speed airSpeed = Smart;
  public: bool airFlow = false;
  public: bool sleepMode = false;
  public: unsigned swing = 0;
  public: bool power = false;
  public: bool turbo = false;
  public: bool hold = false;
  public: bool timerSet = false;
  public: unsigned timerDelay = 0;
  public: unsigned long timerFrom = 0;
};