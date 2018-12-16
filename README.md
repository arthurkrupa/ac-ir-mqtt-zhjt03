# AC Infrared to MQTT adapter ZH/JT-03

An Arduino/ESP8266 infrared sender and receiver compatible with AC units controlled by ZH/JT-03 remotes.

## Supported AC units

The ZH/JT-03 remote supports a wide range of generic AC units from different brands:

- Chigo
- Komeco
- Lloyd
- Blaupunkt
- EPiK

## Building

1. Import project into Platform.IO
2. Create a `include/config.h` file based on `include/config-sample.h` with your configuration
3. Build

## Protocol specification

The IR signals are sent at 38 KHz. The main message body is placed between two high header signals (`6234`, `7392`) and three footer signals (`608`, `7372`, `616`).

### Message codes

#### Command structure

|1|2|3|4|5|6|
|-|-|-|-|-|-|
|`ff00`|`ff00`|`bf40`|`a956`|`3bc6`|`54ab`|
|Timer|Extra|Main|Fan|Temp+mode|-|

#### 1. Timer [not implemented]

|Code|Timer mode|
|-|-|
|`ff00`|no timer|
|`_e_1`|Set new timer (0-15 hrs delay)|
|`_6_9`|Set new timer (16-24 hrs delay)|
|`_a_5`|Keep old timer (0-15 hrs delay)|
|`_2_d`|Keep old timer (16-24 hrs delay)|

If a timer is set or kept, the code has to be merged with a  delay code:

|Code|Timer delay|
|-|-|
|`f_0_`|0h|
|`7_8_`|1h|
|`b_4_`|2h|
|`3_c_`|3h|
|`d_2_`|4h|
|`5_a_`|5h|
|`9_6_`|6h|
|`1_e_`|7h|
|`e_1_`|8h|
|`6_9_`|9h|
|`a_5_`|10h|
|`2_d_`|11h|
|`c_3_`|12h|
|`4_b_`|13h|
|`8_7_`|14h|
|`0_f_`|15h|
|`f_0_`|16h|
|`7_8_`|17h|
|`b_4_`|18h|
|`3_c_`|19h|
|`d_2_`|20h|
|`5_a_`|21h|
|`9_6_`|22h|
|`1_e_`|23h|
|`e_1_`|24h|

#### 2. Extra (Turbo, Hold) [not implemented]

|Code|Extra command|
|-|-|
|`ef10`|Turbo mode|
|`df20`|Hold, i.e. lock current settings|
|`cf30`|Hold with Turbo mode on|

#### 3. Main Command

The main command always has to be combined with relevant Fan settings (#4) and Temperature settings (#5).

|Code|Main command|
|-|-|
|`ff00`|Power (On/Off)|
|`6f90`|Change Sleep Mode|
|`5fa0`|Change Air Speed|
|`bf40`|Temperature up|
|`7f80`|Change AC mode (cool, heat etc.)|
|`3fc0`|Temperature down|
|`df20`|Change Swing mode|
|`1fe0`|Change Air Flow|

#### 4. Fan settings

This part is combined from two components:

- power/swing/sleep
- speed/air flow

|Code|Power / swing / sleep|
|-|-|
|`e_1_`|Power off with swing mode horizontal|
|`f_0_`|Power off with swing mode fixed|
|`d_2_`|Power off with swing mode natural|
|`a_5_`|Swing mode horizontal|
|`b_4_`|Swing mode fixed|
|`9_6_`|Swing mode natural|
|`2_d_`|Swing mode horizontal with Sleep mode on|
|`3_c_`|Swing mode fixed with Sleep mode on|
|`1_e_`|Swing mode natural with Sleep mode on|

|Code|Speed / Air flow|
|-|-|
|`_9_6`|Air Speed slow|
|`_d_2`|Air Speed medium|
|`_b_4`|Air Speed fast|
|`_f_0`|Air Speed smart|
|`_1_e`|Air Speed slow with Air Flow on|
|`_5_a`|Air Speed medium with Air Flow on|
|`_3_c`|Air Speed fast with Air Flow on|
|`_7_8`|Air Speed smart with Air Flow on|

#### 5. Temperature and mode settings

This part is combined from two components:

- temperature
- AC operation mode

|Code|Temperature|
|-|-|
|`f_0`|16°C (the same as 32°C)|
|`7_8`|17°C|
|`b_4`|18°C|
|`3_c`|19°C|
|`d_2`|20°C|
|`5_a`|21°C|
|`9_6`|22°C|
|`1_e`|23°C|
|`e_1`|24°C|
|`6_9`|25°C|
|`a_5`|26°C|
|`2_d`|27°C|
|`c_3`|28°C|
|`4_b`|29°C|
|`8_7`|30°C|
|`0_f`|31°C|
|`f_0`|32°C (the same as 16°C)|

|Code|Operation mode|
|-|-|
|`_f_0`|Auto|
|`_b_4`|Cool (16-31°C)|
|`_3_c`|Cool (32°C)|
|`_e_1`|Heat (16-31°C)|
|`_6_9`|Heat (32°C)|
|`_d_2`|Dry|
|`_9_6`|Fan (16-31°C)|
|`_1_e`|Fan (32°C)|

#### 6. Footer

Always `54ab` (doesn't change).