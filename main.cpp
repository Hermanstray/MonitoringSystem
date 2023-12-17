#include "mbed.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "LM75B.h"
#include "C12832.h"
#include "stdint.h"
#include "semphr.h"

Serial pc(USBTX, USBRX);
PwmOut spkr(p26);
C12832 lcd(p5, p7, p6, p8, p11);
LM75B sensor(p28,p27);
DigitalOut myled1(LED1);
DigitalOut myled2(LED2);
DigitalOut myled3(LED3);
DigitalOut myled4(LED4);
PwmOut r (p23);
PwmOut g (p24);
PwmOut b (p25);
AnalogIn p1(p20);
PwmOut buzz(p26);

// bool buzzOn = 0;
//  buzz.period(1.0 / 440.0);  // Set the PWM period for a frequency of 440 Hz (A4 note)
        
//          if (buzzOn){
//             buzz.write(0.5);  // Set the duty cycle to 50% (make sound)
//             buzzOn = 0;
//          } else {
//             buzz.write(0.0);  // Set the duty cycle to 0% (stop sound)
//             buzzOn = 1;
//         }

struct ClockTime {
    int hours;
    int minutes;
    int seconds;
};

 

struct ClockTime global_clock = {0, 0, 0};     // Initialize to midnight
int global_monitoring_period = 3;              // Default is deactivated
struct ClockTime global_alarm_clock= {0, 1, 0};                // Default is deactivated
int global_time_alarm = 5;
int global_processing_period = 0;              // Default is deactivated
float global_temperature = 20;                    // Placeholder for temperature value
int global_luminosity = 1;                     // Placeholder for luminosity value
int global_alarm_clock_set = 0;                // Flag to indicate if the alarm clock is set
float global_alarm_temperature = 27;              // Placehold er for alarm temperature value
int global_alarm_luminosity = 2;               // Placeholder for alarm luminosity value 
int time_in_alarm;
bool global_alarms_active=false;
bool global_tala_gone=false;
bool global_alarm_gone_temp=false;
bool global_alarm_gone_lum=false;
bool global_alarm_gone_clock=false;

QueueHandle_t xQueue;
SemaphoreHandle_t myMutex;
//if (xSemaphoreTake(myMutex, portMAX_DELAY)) {};

extern void monitor(void);

/*-------------------------------------------------------------------------+
| Function: my_fgets        (called from my_getline / monitor) 
+--------------------------------------------------------------------------*/ 
char* my_fgets (char* ln, int sz, FILE* f)
{
//  fgets(line, MAX_LINE, stdin);
//  pc.gets(line, MAX_LINE);
  int i; char c;
  for(i=0; i<sz-1; i++) {
      c = pc.getc();
      ln[i] = c;
      if ((c == '\n') || (c == '\r')) break;
  }
  ln[i] = '\0';

  return ln;
}

void vClockTask( void *pvParameters ) { 

    while(1) {
        
        global_clock.seconds ++;
        if (global_clock.seconds == 60){
            global_clock.seconds = 00;
            global_clock.minutes ++;
        }
        if (global_clock.minutes == 60){
            global_clock.minutes = 00;
            global_clock.hours ++;
        }
        if (global_clock.hours == 24){
            global_clock.hours = 00;
        }
        
        if (xSemaphoreTake(myMutex, portMAX_DELAY)) {
            lcd.rect(0, 15, 127, 31, 0); // Clears the bottom lef half of the screen used for clock
            lcd.locate(1,15);
            lcd.printf("%02d", global_clock.hours);
            lcd.locate(13,15);
            lcd.printf(":%02d", global_clock.minutes);
            lcd.locate(26,15);
            lcd.printf(":%02d", global_clock.seconds);

            if(global_alarm_clock.hours==global_clock.hours && global_alarm_clock.minutes == global_clock.minutes && global_alarm_clock.seconds==global_clock.seconds && global_alarms_active){
                global_alarm_gone_clock=true;
                lcd.locate(70,3);
                lcd.printf("C");
            } 

            if(!global_alarm_gone_lum && !global_alarm_gone_temp && !global_alarm_gone_clock ) {
                lcd.locate(70, 3);
                lcd.printf("    ");
            }

            xSemaphoreGive(myMutex);
        };

        if ((global_alarm_gone_lum || global_alarm_gone_temp || global_alarm_gone_clock) && !global_tala_gone){
            time_in_alarm+=1;
            buzz.write(0.5);

            if(time_in_alarm==global_time_alarm){
                global_tala_gone=true;
                time_in_alarm=0;
                buzz.write(0.0);
            }
        } 
        
        const TickType_t xDelay = 1000; // 1s delay; 
        vTaskDelay(xDelay);
    }
}

void vSensorTask( void *pvParameters ) {

    while(1) {

        //if (xSemaphoreTake(myMutex, portMAX_DELAY)) {
            float analogValue = p1.read();
            float mappedValue = analogValue * 3.0;
            global_luminosity = static_cast<int>(mappedValue);

            if (sensor.open()) {
                //printf("Device detected!\n");
                global_temperature = sensor.temp();
                
                if (xSemaphoreTake(myMutex, portMAX_DELAY)) {
                    lcd.rect(0, 0, 127, 15, 0); // Clear top half of screen used for displaying values
                    lcd.locate(0,3);
                    lcd.printf("Temp = %.3f", global_temperature);

                    if (global_temperature>=global_alarm_temperature && global_alarms_active){
                        global_alarm_gone_temp=true;  
                        lcd.locate(76,3);
                        lcd.printf("T");
                    }
                    if (global_luminosity>=global_alarm_luminosity && global_alarms_active){
                        global_alarm_gone_lum=true;
                        lcd.locate(82,3);
                        lcd.printf("L");
                    }

                    lcd.locate(80,15);
                    lcd.printf("Lum = %1d", global_luminosity);

                    xSemaphoreGive(myMutex);
                 };
                if(global_temperature > global_alarm_temperature) {
                    r = 0;
                    b = 1;
                    g = 1;
                } else if (global_temperature < 25.0){
                    r = 1;
                    b = 0;
                    g = 1;
                } else {
                    r = 1;
                    b = 1;
                    g = 0;
                }
            
                // if(temp > 27.0) {
                //     r = 0;
                //     b = 1;
                //     g = 1;
                // } else if (temp < 25.0){
                //     r = 1;
                //     b = 0;
                //     g = 1;
                // } else {
                //     r = 1;
                //     b = 1;
                //     g = 0;
                // }
            }
                
            

            switch(global_luminosity){
                case 0 :  {
                    myled1 = 1;
                    myled2 = 0;
                    myled3 = 0;
                    myled4 = 0;
                    break;
                }
                case 1 : {
                    myled1 = 0;
                    myled2 = 1;
                    myled3 = 0;
                    myled4 = 0;
                    break;
                }
                case 2 :  {
                    myled1 = 0;
                    myled2 = 0;
                    myled3 = 1;
                    myled4 = 0;
                    break;
                }
                case 3 : {
                    myled1 = 0;
                    myled2 = 0;
                    myled3 = 0;
                    myled4 = 1;
                    break;
                }
                default: {
                    myled1 = 0;
                    myled2 = 0;
                    myled3 = 0;
                    myled4 = 0;
                    break;
                }
        }
        

        const TickType_t xDelay = global_monitoring_period*1000;
        vTaskDelay(xDelay);

    }
}

void vMonitorTask( void *pvParameters ) {

    while(1){

        monitor();

    }
}

int main( void ) {
    /* Perform any hardware setup necessary. */
//    prvSetupHardware();

    pc.baud(115200);
    myMutex = xSemaphoreCreateMutex();
    buzz.period(1.0 / 440.0);

//    printf("Hello from mbed -- FreeRTOS / cmd\n");

    /* --- APPLICATION TASKS CAN BE CREATED HERE --- */

    //xQueue = xQueueCreate( 3, sizeof( int32_t ) );
    
    xTaskCreate( vClockTask, "Clock task",  2*configMINIMAL_STACK_SIZE, NULL, 3 , NULL );
    xTaskCreate( vSensorTask, "Sensor Task", 2*configMINIMAL_STACK_SIZE, NULL, 2, NULL );
    xTaskCreate( vMonitorTask, "Monitor Task", 2*configMINIMAL_STACK_SIZE, NULL, 1, NULL );
    
    /* Start the created tasks running. */
    vTaskStartScheduler();

    /* Execution will only reach here if there was insufficient heap to
    start the scheduler. */
    for( ;; );
    return 0;
}
