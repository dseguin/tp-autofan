/*****************************************************************************
 * A very crude program to automatically set fan speed on some Thinkpad
 * laptops. Not intended for real use.
 *
 * https://github.com/dseguin/tp-autofan/
 * Copyright (c) 2017 David Seguin <davidseguin@live.ca>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#define TP_LINELENGTH    80
#define TP_CHECKINTERVAL 1
#define TP_THRESHOLD1    55
#define TP_THRESHOLD2    70
#define TP_THRESHOLDDIFF 10
#define TP_UIDNONROOT    1000

int quit = 0;

void handle_signal(int sig);
int get_num_cpu_cores(void);
int get_highest_temp(int cores);
int set_fanlevel(int level);

int main(void)
{
    int cores, ctemp, flevel = -1;
    struct sigaction st_sig;
    if(seteuid(TP_UIDNONROOT))
    {
        perror("seteuid error");
        return 1;
    }

    cores = get_num_cpu_cores();
    if(!cores)
    {
        fprintf(stderr, "Error retrieving number of cores\n");
        return 1;
    }

    st_sig.sa_flags = 0;
    st_sig.sa_handler = handle_signal;
    sigemptyset(&st_sig.sa_mask);
    sigaction(SIGINT,  &st_sig, NULL);
    sigaction(SIGQUIT, &st_sig, NULL);
    sigaction(SIGTERM, &st_sig, NULL);
    while(!quit)
    {
        ctemp = get_highest_temp(cores);
        if(flevel == -1)
        {
            if(ctemp > TP_THRESHOLD1)
            {
                flevel = 6;
                if(set_fanlevel(flevel))
                    return 1;
            }
        }
        else if(flevel == 6)
        {
            if(ctemp < TP_THRESHOLD1 - TP_THRESHOLDDIFF)
            {
                flevel = -1;
                if(set_fanlevel(flevel))
                    return 1;
            }
            else if(ctemp > TP_THRESHOLD2)
            {
                flevel = 7;
                if(set_fanlevel(flevel))
                    return 1;
            }
        }
        else if(flevel == 7)
        {
            if(ctemp < TP_THRESHOLD2 - TP_THRESHOLDDIFF)
            {
                flevel = 6;
                if(set_fanlevel(flevel))
                    return 1;
            }
        }
        sleep(TP_CHECKINTERVAL);
    }
    return 0;
}

void handle_signal(int sig)
{
    fprintf(stderr, "Caught signal %d\n", sig);
    quit = sig;
}

int get_num_cpu_cores(void)
{
    int   cores = 0;
    int   i;
    FILE *info;
    char  line[TP_LINELENGTH];
    char *nl;
    const char *key = "cpu cores";

    info = fopen("/proc/cpuinfo","r");
    if(!info)
    {
        fprintf(stderr, "Could not open /proc/cpuinfo\n");
        perror("fopen error");
        return 0;
    }
    while(fgets(line, TP_LINELENGTH, info))
    {
        if((nl = strchr(line, '\n')))
            *nl = '\0';
        for(i = 0; i < 9; i++)
            if(line[i] != key[i]) break;
        if(i != 9) continue;
        nl = strtok(line+10, ": ");
        if(!nl) continue;
        cores = atoi(nl);
        break;
    }
    fclose(info);
    return cores;
}

int get_highest_temp(int cores)
{
    int i;
    int htemp = -1;
    int ctemp = -1;
    size_t nc;
    FILE *temp_input;
    const char *sysfs_path = "/sys/class/hwmon/hwmon2/";
    char temp_path[64];
    char temp_suffix[16];
    char temp_str[4];

    /*temp1_input = package temp, temp<n>_input = core <n> temp*/
    for(i = 1; i < cores+2 && i < 10000; i++)
    {
        strcpy(temp_path, sysfs_path);
        sprintf(temp_suffix, "temp%d_input", i);
        strcat(temp_path, temp_suffix);
        temp_input = fopen(temp_path, "rb");
        if(!temp_input)
        {
            fprintf(stderr, "Could not open %s\n", temp_path);
            perror("fopen error");
            return -1;
        }
        nc = fread(temp_str, 1, 2, temp_input);
        fclose(temp_input);
        if(nc != 2)
        {
            fprintf(stderr,
                    "Error reading from %s: Expected 2 bytes, got %lu\n",
                    temp_path, nc);
            return -1;
        }
        temp_str[2] = '\0';
        ctemp = atoi(temp_str);
        if(!ctemp)
        {
            fprintf(stderr,
                    "Error reading from %s: Expected integer, got %s\n",
                    temp_path, temp_str);
            return -1;
        }
        if(ctemp > htemp) htemp = ctemp;
    }
    return htemp;
}

int set_fanlevel(int level)
{
    char text[16];
    FILE *fan;
    if(level == -1) sprintf(text, "level auto");
    else            sprintf(text, "level %d", level);

    if(seteuid(0))
    {
        perror("seteuid error");
        return 1;
    }
    fan = fopen("/proc/acpi/ibm/fan", "w");
    if(!fan)
    {
        seteuid(TP_UIDNONROOT);
        fprintf(stderr, "Error writing to /proc/acpi/ibm/fan\n");
        perror("fopen error");
        return 1;
    }
    fprintf(fan, "%s", text);
    fclose(fan);
    if(seteuid(TP_UIDNONROOT))
    {
        perror("seteuid error");
        return 1;
    }
    fprintf(stderr, "Set fan to %s\n", text);
    return 0;
}
