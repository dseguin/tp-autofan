A very crude program to automatically set fan speed on some Thinkpad
laptops. Not intended for real use. Will only work on linux.

This makes a number of assumptions:
- running GNU/Linux
- kernel module thinkpad_acpi is running with the option fan_control=1
- fan control hwmon interface is located at /sys/class/hwmon/hwmon2/
- /proc/acpi/ibm/fan exists and is writable

Comes with an optional systemd service file that can be installed with
`make install` 

