#define BAT_REUIRED      1
#define TEMP_REQUIRED    1
#define FAN_REQUIRED     0
#define NETWORK_REQUIRED 1
#define CPU_REQUIRED     1
#define RAM_REQUIRED     1
#define LANG_REQUIRED    1
#define TIME_REQUIRED    1


#define BAT_CAP_PATH "/sys/class/power_supply/BAT0/capacity"
#define BAT_STAT_PATH "/sys/class/power_supply/BAT0/status"
#define BAT1_CAP_PATH "/sys/class/power_supply/BAT1/capacity"
#define BAT1_STAT_PATH "/sys/class/power_supply/BAT1/status"
#define CPU_TEMP_PATH "/sys/class/thermal/thermal_zone0/temp"
#define FAN_PATH "/sys/class/hwmon/hwmon1/device/fan1_input"
