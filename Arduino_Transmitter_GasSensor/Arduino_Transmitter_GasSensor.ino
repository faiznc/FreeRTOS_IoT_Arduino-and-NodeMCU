#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <RCSwitch.h> //Library Transmitter FS1000A, 433 Mhz

RCSwitch mySwitch = RCSwitch();
SemaphoreHandle_t xSerialSemaphore;

void TaskSendNotif( void *pvParameters );
void TaskAnalogRead( void *pvParameters );
int bacaan,sensorValue;

// the setup function runs once when you press reset or power the board
void setup() {

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  delay(2000);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
//  }

  // Transmitter is connected to Arduino Pin #10  
  mySwitch.enableTransmit(10);
  
  if ( xSerialSemaphore == NULL )  // Check to confirm that the Serial Semaphore has not already been created.
  {
    xSerialSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
    if ( ( xSerialSemaphore ) != NULL )
      xSemaphoreGive( ( xSerialSemaphore ) );  // Make the Serial Port available for use, by "Giving" the Semaphore.
  }

  // Now set up two Tasks to run independently.
  xTaskCreate(
    TaskSendNotif
    ,  "SendNotif"  // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  xTaskCreate(
    TaskAnalogRead
    ,  "AnalogRead"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskAnalogRead( void *pvParameters __attribute__((unused)) )  // This is a Task.
{

  for (;;)
  {
    // Baca nilai analog di pin A0
    int sensorValue = analogRead(A0);

    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
        bacaan = sensorValue;

      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }

    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}


void TaskSendNotif( void *pvParameters __attribute__((unused)) )  // This is a Task.
{

  for (;;) // A Task shall never return or exit.
  {
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
        if(bacaan>=500){
            mySwitch.send(11111, 24); // Jika terdeteksi, kirim "11111"
            delay(1000);    //Diberi delay karena Data rate Transmitter rendah, tunggu data terkirim dahulu baru lanjutkan kode
        }
        else{
            mySwitch.send(11110, 24); // Jika tidak terdeteksi, kirim "11110"
            delay(1000);    //Diberi delay karena Data rate Transmitter rendah, tunggu data terkirim dahulu baru lanjutkan kode
        }
        Serial.println(bacaan);
        delay(2000); //Delay antar pengiriman, sehingga tidak spam broadcast pesan
        
        // "11111" -> 4 digit pertama kita anggap identifier, digit terakhir adalah keadaan.
        // misal "ABCDE", "ABCD" = identifier perangkat, "E" adalah keadaannya, 1 = Terdeteksi, 0 = Aman
        // Hal ini membuat kita selalu mendapat data terupdate dan memastikan transmisi sukses
        
      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }

    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}
