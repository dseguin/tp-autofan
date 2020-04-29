A program to automatically set fan speed on most Thinkpad laptops. It works
by finding the coretemp hwmon interface, then checks the temperature every X
seconds, and decides whether or not to ramp up/down the system fan.

There are 2 temperature thresholds that influence the 3 fan speed levels, all
of which can be configured in `src/settings.h` before compiling.

This makes a number of assumptions:
- running GNU/Linux
- kernel module thinkpad_acpi is running with the option fan_control=1
- /proc/acpi/ibm/fan exists and is writable

`settings.h` contains a number of tweeks that can be changed prior to
compiling. See the following for an explanation on enabling fan control:
https://www.kernel.org/doc/html/v5.3/admin-guide/laptops/thinkpad-acpi.html#fan-control-and-monitoring-fan-speed-fan-enable-disable

Comes with an optional systemd service file that can be installed with
`make install`
