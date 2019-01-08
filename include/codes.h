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