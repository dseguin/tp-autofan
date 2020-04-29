#ifndef _SETTINGS_H
#define _SETTINGS_H

/* Number of seconds before checking temperature again */
#define TP_CHECKINTERVAL 1

/* First temperature threshold in Celcius */
#define TP_THRESHOLD1    55

/* Second temperature threshold in Celcius */
#define TP_THRESHOLD2    70

/* Difference in temperature before ramping down fan speed */
#define TP_THRESHOLDDIFF 10

/* Fan speed under normal conditions (-1 to 7, -1 = auto) */
#define TP_LEVEL_BASELINE -1

/* Fan speed at first temperature threshold (-1 to 7, -1 = auto) */
#define TP_LEVEL_THRESH1 6

/* Fan speed at second temperature threshold (-1 to 7, -1 = auto) */
#define TP_LEVEL_THRESH2 7

/* UID to switch to when not needing root privileges */
#define TP_UIDNONROOT    1000

#endif /*_SETTINGS_H*/
