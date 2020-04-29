#ifndef _HWMON_WRAPPER_H
#define _HWMON_WRAPPER_H

char *get_driver_path(void);
int get_highest_temp(char *driver_path);

#endif /*_HWMON_WRAPPER_H*/
