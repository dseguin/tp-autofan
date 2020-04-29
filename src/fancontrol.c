#include "settings.h"
#include "hwmon_wrapper.h"
#include "main.h"
#include <stdio.h>

#define FANCONTROL_FILE "/proc/acpi/ibm/fan"
#define NO_CHANGE 8

int _set_fanlevel(int level)
{
	char cmd[16] = {'\0'};
	FILE *fan;
	if(level < 0 || level > 7)
		snprintf(cmd, 16, "level auto");
	else
		snprintf(cmd, 16, "level %d", level);
	fan = fopen(FANCONTROL_FILE, "w");
	if(!fan) {
		perror(FANCONTROL_FILE);
		return 1;
	}
	fprintf(fan, cmd);
	fclose(fan);
	return 0;
}

int _check_fan_temp(char *driver_path, int current_level)
{
	int temp = 0;
	temp = get_highest_temp(driver_path);
	if(temp < TP_THRESHOLD1) {
		if(current_level == TP_LEVEL_BASELINE)
			return NO_CHANGE;
		if(current_level == TP_LEVEL_THRESH1 && temp > TP_THRESHOLD1 - TP_THRESHOLDDIFF)
			return NO_CHANGE;
		return TP_LEVEL_BASELINE;
	} else if(temp < TP_THRESHOLD2) {
		if(current_level == TP_LEVEL_THRESH1)
			return NO_CHANGE;
		if(current_level == TP_LEVEL_THRESH2 && temp > TP_THRESHOLD2 - TP_THRESHOLDDIFF)
			return NO_CHANGE;
		return TP_LEVEL_THRESH1;
	} else {
		if(current_level == TP_LEVEL_THRESH2)
			return NO_CHANGE;
		return TP_LEVEL_THRESH2;
	}
}

void monitor_temp(void)
{
	char *driver_path = NULL;
	int fan_level = TP_LEVEL_BASELINE;
	driver_path = get_driver_path();
	if(!driver_path)
		return;
	while(not_quitting()) {
		int change = NO_CHANGE;
		change = _check_fan_temp(driver_path, fan_level);
		if(change == NO_CHANGE)
			continue;
		if(!_set_fanlevel(change))
			fan_level = change;
	}
}
