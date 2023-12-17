//#ifdef notdef

/***************************************************************************
| File: comando.c  -  Concretizacao de comandos (exemplo)
|
| Autor: Carlos Almeida (IST)
| Data:  Nov 2002
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t xQueue;




/*-------------------------------------------------------------------------+
| Function: cmd_send - send message
+--------------------------------------------------------------------------*/ 
struct ClockTime {
    int hours;
    int minutes;
    int seconds;
};

extern struct ClockTime global_clock;     // Initialize to midnight
extern int global_monitoring_period;              // Default is deactivated
extern struct ClockTime global_alarm_clock;                // Default is deactivated
extern int global_time_alarm;
extern int global_processing_period;              // Default is deactivated
extern float global_temperature;                    // Placeholder for temperature value
extern int global_luminosity;                     // Placeholder for luminosity value
extern int global_alarm_clock_set;                // Flag to indicate if the alarm clock is set
extern float global_alarm_temperature;              // Placehold er for alarm temperature value
extern int global_alarm_luminosity;               // Placeholder for alarm luminosity value
extern bool global_alarms_active;
extern bool global_tala_gone;
extern bool global_alarm_gone_temp;
extern bool global_alarm_gone_lum;
extern bool global_alarm_gone_clock;
/*-------------------------------------------------------------------------+
| Function: cmd_rc - read clock
+--------------------------------------------------------------------------*/
void cmd_rc(int argc, char** argv)
{
    // Implement the functionality to read the clock here
    printf("Current clock: %02d:%02d:%02d\n", global_clock.hours, global_clock.minutes, global_clock.seconds);
}

/*-------------------------------------------------------------------------+
| Function: cmd_sc - set clock
+--------------------------------------------------------------------------*/
void cmd_sc(int argc, char** argv)
{
    // Implement the functionality to set the clock here
    if (argc == 4) {
        global_clock.hours = atoi(argv[1]);
        global_clock.minutes = atoi(argv[2]);
        global_clock.seconds = atoi(argv[3]);

        printf("Clock set to: %02d:%02d:%02d\n", global_clock.hours, global_clock.minutes, global_clock.seconds);
    } else {
        printf("Usage: sc h m s\n");
    }
}

/*-------------------------------------------------------------------------+
| Function: cmd_rtl - read temperature and luminosity
+--------------------------------------------------------------------------*/
void cmd_rtl(int argc, char** argv)
{
    // Implement the functionality to read temperature and luminosity here
    printf("Temperature: %0.3f, Luminosity: %d\n", global_temperature, global_luminosity);
}

/*-------------------------------------------------------------------------+
| Function: cmd_rp - read parameters (pmon, tala, pproc)
+--------------------------------------------------------------------------*/
void cmd_rp(int argc, char** argv)
{
    // Implement the functionality to read parameters here
    printf("Monitoring Period: %d, Time Alarm: %d, Processing Period: %d\n",
           global_monitoring_period, global_time_alarm, global_processing_period);
}

/*-------------------------------------------------------------------------+
| Function: cmd_mmp - modify monitoring period (seconds - 0 deactivate)
+--------------------------------------------------------------------------*/
void cmd_mmp(int argc, char** argv)
{
    // Implement the functionality to modify monitoring period here
    if (argc == 2) {
        global_monitoring_period = atoi(argv[1]);
        printf("Monitoring Period set to: %d seconds\n", global_monitoring_period);
    } else {
        printf("Usage: mmp seconds\n");
    }
}

// ... (implementations for other commands)


/*-------------------------------------------------------------------------+
| Function: cmd_mta - modify time alarm (seconds)
+--------------------------------------------------------------------------*/
void cmd_mta(int argc, char** argv)
{
    // Implement the functionality to modify time alarm here
    if (argc == 2) {
        global_time_alarm = atoi(argv[1]);
        printf("Time Alarm set to: %d seconds\n", global_time_alarm);
    } else {
        printf("Usage: mta seconds\n");
    }
}

/*-------------------------------------------------------------------------+
| Function: cmd_mpp - modify processing period (seconds - 0 deactivate)
+--------------------------------------------------------------------------*/
void cmd_mpp(int argc, char** argv)
{
    // Implement the functionality to modify processing period here
    if (argc == 2) {
        global_processing_period = atoi(argv[1]);
        printf("Processing Period set to: %d seconds\n", global_processing_period);
    } else {
        printf("Usage: mpp seconds\n");
    }
}

/*-------------------------------------------------------------------------+
| Function: cmd_rai - read alarm info (clock, temperature, luminosity, active/inactive-A/a)
+--------------------------------------------------------------------------*/
void cmd_rai(int argc, char** argv)
{
    // Implement the functionality to read alarm info here
    printf("Alarm Info:\n");
    printf("Clock: %02d:%02d:%02d\n", global_alarm_clock.hours,global_alarm_clock.minutes, global_alarm_clock.seconds);
    printf("Temperature Alarm: %f\n", global_alarm_temperature);
    printf("Luminosity Alarm: %d\n", global_alarm_luminosity);
    if (global_alarms_active){
    printf("Alarms are Activated\n");
    }else {
        printf("Alarms are deactivated\n");
    }
}

/*-------------------------------------------------------------------------+
| Function: cmd_dac - define alarm clock
+--------------------------------------------------------------------------*/
void cmd_dac(int argc, char** argv)
{
    // Implement the functionality to define alarm clock here
    if (argc == 4) {
        global_alarm_clock.hours = atoi(argv[1]);
        global_alarm_clock.minutes = atoi(argv[2]);
        global_alarm_clock.seconds = atoi(argv[3]);
        global_alarm_clock_set = 1;

        printf("Alarm Clock set to: %02d:%02d:%02d\n", global_alarm_clock.hours, global_alarm_clock.minutes, global_alarm_clock.seconds);
    } else {
        printf("Usage: dac h m s\n");
    }
}

/*-------------------------------------------------------------------------+
| Function: cmd_dtl - define alarm temperature and luminosity
+--------------------------------------------------------------------------*/
void cmd_dtl(int argc, char** argv)
{
    // Implement the functionality to define alarm temperature and luminosity here
    if (argc == 3) {
        global_alarm_temperature = atoi(argv[1]);
        global_alarm_luminosity = atoi(argv[2]);

        printf("Temperature Alarm set to: %.3f, Luminosity Alarm set to: %d\n", global_alarm_temperature, global_alarm_luminosity);
    } else {
        printf("Usage: dtl T L\n");
    }
}

/*-------------------------------------------------------------------------+
| Function: cmd_aa - activate/deactivate alarms (A/a)
+--------------------------------------------------------------------------*/
void cmd_aa(int argc, char** argv)
{
    // Implement the functionality to activate/deactivate alarms here
    if (argc == 2) {
        if (argv[1][0] == 'A') {
            global_alarms_active = true;
            printf("Alarms activated\n");
        } else if ( argv[1][0] == 'a'){
            global_alarms_active = false;
            printf("Alarms deactivated\n");
        }
    } else {
        printf("Usage: aa A/a\n");
    }
}
void cmd_cai(int argc, char** argv)
{
    global_alarm_gone_temp=false;
    global_alarm_gone_lum=false;
    global_alarm_gone_clock=false;
    global_alarms_active=false;
    global_tala_gone=false;
    printf("Alarms Cleared\n");
}
//#endif //notdef