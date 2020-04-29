#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#define HWMON_SYSFS "/sys/class/hwmon"
#define HWMON_DRIVER "coretemp"
#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)

char *_check_hwmon(char *hwmon, const char *driver)
{
	FILE *hw_name;
	char *path = NULL;
	char *temp_path = NULL;
	char name[32] = {'\0'};
	unsigned path_len = 0;
	if(!strstr(hwmon, "hwmon"))
		return temp_path;
	path_len = strnlen(hwmon, 255) + strlen(HWMON_SYSFS) + 7;
	path = malloc(path_len);
	snprintf(path, path_len, "%s/%s/name", HWMON_SYSFS, hwmon);
	hw_name = fopen(path, "r");
	if(!hw_name)
		return temp_path;
	fgets(name, sizeof(name), hw_name);
	if(strstr(name, driver)) {
		temp_path = malloc(path_len - 4);
		memcpy(temp_path, path, path_len - 4);
		temp_path[path_len - 5] = '\0';
	}
	fclose(hw_name);
	free(path);
	return temp_path;
}

char *get_driver_path(void)
{
	char *temp_path = NULL;
	DIR *hw_root;
	struct dirent *d;
	hw_root = opendir(HWMON_SYSFS);
	if(!hw_root) {
		fprintf(stderr, "Could not open %s\n", HWMON_SYSFS);
		perror("opendir error");
		return temp_path;
	}
	while((d = readdir(hw_root))) {
		temp_path = _check_hwmon(d->d_name, HWMON_DRIVER);
		if(temp_path)
			break;
	}
	closedir(hw_root);
	return temp_path;
}

int _get_temp(char *input, char *path)
{
	int temp = 0;
	char tmp_str[8] = {'\0'};
	FILE *f_input = NULL;
	char *input_path = NULL;
	input_path = malloc(strlen(path) + strlen(input));
	memcpy(input_path, path, strlen(path));
	memcpy(input_path + strlen(path), input, strlen(input));
	f_input = fopen(input_path, "r");
	if(!f_input)
		return -1;
	fgets(tmp_str, 8, f_input);
	temp = atoi(tmp_str);
	temp /= 1000;
	fclose(f_input);
	return temp;
}

int get_highest_temp(char *driver_path)
{
	int temp = 0;
	DIR *drv_root;
	struct dirent *d;
	drv_root = opendir(driver_path);
	if(!drv_root) {
		fprintf(stderr, "Could not open %s\n", driver_path);
		perror("opendir error");
		return -1;
	}
	while((d = readdir(drv_root))) {
		int new_temp = 0;
		if(!strstr(d->d_name, "temp") || !strstr(d->d_name, "input"))
			continue;
		new_temp = _get_temp(d->d_name, driver_path);
		temp = MAX(new_temp, temp);
	}
	closedir(drv_root);
	return temp;
}
