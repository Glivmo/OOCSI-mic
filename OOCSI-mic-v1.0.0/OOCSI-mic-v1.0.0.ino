/******************************************************************************
   Microphone module that measures the maximum decibel each second and
   summarize the measurements per minute. The summary consist of the average
   and the maximum.

   TU/e course of faculty of Industrial Design by Mathias Funk
   DBSU10 - Tyria Team 05

   Use the Serial.println() for values and stage confirmation.

   More information could be found on:
   https://github.com/Glivmo/OOCSI-mic

   Features to be implemented in the future:
   - Ambient sound based on the median
   - Pre-defining array optimalization
   - Sleep/interrupt

 ******************************************************************************/

//  -------------------------

//ab-Exponential regression: y=AB^x = 32.6*1.00099^sensVal

// OOCSI -------------------------

#include "OOCSI.h"

// use this if you want the OOCSI-ESP library to manage the connection to the Wifi
// SSID of your Wifi network, the library currently does not support WPA2 Enterprise networks
const char* ssid = "Tyria_Hotspot";
// Password of your Wifi network.
const char* password = "You're welcome";

// name for connecting with OOCSI (unique handle)
const char* OOCSIName = "Tyria_T05";
// put the adress of your OOCSI server here, can be URL or IP address string
const char* hostserver = "oocsi.id.tue.nl";

OOCSI oocsi = OOCSI();


// SAMPLE ARRAY VARIABLES -------------------------

//array to store the sensor values of the one second measurement
//based on 115200 within one second with no delay() within
int sampleSecondSize = 6000;
int sampleSecondArray[6000];
int indexSampleSecond = 0;

//array that stores the data measurements of each second for the minute summary
int sampleMinuteSize = 70;
int sampleMinuteArray[70];
int indexSampleMinute = 0;
int actualMeasuredPointsMinutes = 60;

//max value for each second
int maxValue = 0;


// DATA OUTPUT VARIABLES  -------------------------

//variable for averaging the values
int sumAllValues = 0;

//average of all max decibels of each second
int amplitudeAverageMax = 0;
int decibelAverageMax = 0;

//highest decibel of the whole minute
int amplitudeTotalMax = 0;
int decibelTotalMax = 0;

//average average decibel of the whole minute (median)
//requires another array as sampleMinuteSize
int amplitudeAmbient = 0;
int decibelAmbient = 0;


// TIME RELATED VARIABES -------------------------
int currentTime = 0;

int previousTimeSampleSecond = 0;
int previousTimeSampleMinute = 0;

int timeIntervalSampleSecond = 1000;
int timeIntervalSampleMinute = 60000;


void setup() {

  Serial.begin(115200);

  // output OOCSI activity on the built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  oocsi.setActivityLEDPin(LED_BUILTIN);

  oocsi.connect(OOCSIName, hostserver, ssid, password, processOOCSI);
}


void loop() {

  currentTime = millis();

  //checking whether the minute has passed
  if (currentTime - previousTimeSampleMinute < timeIntervalSampleMinute) {

    //checking whether the second has passed
    if (currentTime - previousTimeSampleSecond < timeIntervalSampleSecond ) {

      //capture and store the sensor values within one second
      sampleSecondArray[indexSampleSecond] = analogRead(A0);

      indexSampleSecond++;


      //safeguard to prevent array inbound error
      if (indexSampleSecond >= sampleSecondSize) {
        indexSampleSecond = 0;

        //just a warning, but does not delay the data measuring process
        //do enlarge the pre-defined the array if this message occur consistently
        Serial.println("WARNING: array overwrite!");
      }

      //when one second has passed, the measured data point will be stored on the minute array for a new measurement
    } else {
      Serial.println("One second has passed.");
      Serial.print("The new maxValue is: ");

      //find the max value within the one second measurement by going through every single array element
      for (int i = 0; i < sampleSecondSize; i++) {

        //going through every element of the array, overwrite the maxVal if the element is larger to capture the max value
        if (sampleSecondArray[i] > maxValue) {
          maxValue = sampleSecondArray[i];

          Serial.print(maxValue);
          Serial.print(", ");
        }
      }

      Serial.println(" ");

      //storing the data point in the one minute array
      sampleMinuteArray[indexSampleMinute] = maxValue;
      indexSampleMinute++;

      Serial.println("maxValue is stored!");

      //wipe everything second related variabeles after one second for the new measurement
      maxValue = 0;
      indexSampleSecond = 0;

      for (int i = 0; i < sampleSecondSize; i++) {
        sampleSecondArray[i] = 0;
      }

      Serial.println("every second sample variables are wiped. To the next index of the minute array!");
      Serial.print("new minute array index is: ");
      Serial.println(indexSampleMinute);
      Serial.println(" ------ ");
      Serial.println(" ");

      currentTime = millis(); //additional recalibration of time
      previousTimeSampleSecond = currentTime;
    }


    //when one minute has passed, summarize the 60 single second samples
  } else {

    Serial.println("Start summarizing now!");

    //sum up all data points to the sumAllMinuteValues for data analysis calculations
    for (int i = 0; i < sampleMinuteSize; i++) {
      sumAllValues = sumAllValues + sampleMinuteArray[i];
    }

    //calculating the average
    amplitudeAverageMax = sumAllValues / actualMeasuredPointsMinutes;
    //transforming into decibels
    decibelAverageMax = int(32.6 * pow(1.00099, amplitudeAverageMax));

    //    Serial.print("decibelAverageMax is: ");
    //    Serial.println(decibelAverageMax);

    //wipe sumAllValues
    sumAllValues = 0;

    //calculating the max
    for (int i = 0; i < sampleMinuteSize; i++) {

      if (sampleMinuteArray[i] > amplitudeTotalMax) {
        amplitudeTotalMax = sampleMinuteArray[i];
      }
    }

    //transforming into decibels
    decibelTotalMax = int(32.6 * pow(1.00099, amplitudeTotalMax));
    //    Serial.print("decibelTotalMax is: ");
    //    Serial.println(decibelTotalMax);

    //decibelAmbient pending. New array to be made for that feature.


    Serial.println("Sending to oocsi now!");

    //sending a new message through oocsi
    oocsi.newMessage("Tyria_World_Data");

    //    oocsi.addInt("timeStamp", timeStamp);   //timeStamp issues will be resolved in the future

    //pre-define the key names for the database
    oocsi.addString("dataNames", "dataAv,dataMin,dataMax");

    //sending the key values
    oocsi.addInt("dataAv", decibelAverageMax);
    oocsi.addInt("dataMin", 0);
    oocsi.addInt("dataMax", decibelTotalMax);

    // this command will send the message; don't forget to call this after creating a message
    oocsi.sendMessage();

    // prints out the raw message (how it is sent to the OOCSI server)
    oocsi.printSendMessage();

    // needs to be checked in order for OOCSI to process incoming data.
    oocsi.check();
    delay(1000);


    //    Serial.print("timeStamp is: ");
    //    Serial.println(timeStamp);
    //
    //    Serial.print("dataAV is: ");
    //    Serial.println(decibelAverageMax);
    //
    //    Serial.print("dataMax is: ");
    //    Serial.println(decibelTotalMax);
    //
    //    Serial.print("dataMin is: ");
    //    Serial.println("0");

    //now wipe everything for recalibration of the module
    Serial.println("Wiping now!");

    for (int i = 0; i < sampleMinuteSize; i++) {
      sampleMinuteArray[i] = 0;
    }

    indexSampleMinute = 0;

    for (int i = 0; i < sampleSecondSize; i++) {
      sampleSecondArray[i] = 0;
    }

    indexSampleSecond = 0;

    maxValue = 0;

    Serial.println(" ------ Message has been sent");
    Serial.println(" ");

    currentTime = millis(); //additional recalibration of time
    previousTimeSampleSecond = currentTime;
    previousTimeSampleMinute = currentTime;
  }
}


void processOOCSI() {
  // don't do anything; we are sending only
}
