# OOCSI Mic 
 
Microphone module that measures the maximum decibel each second and summarize the measurements per minute. The summary consist of the average and the maximum.

TU/e course of faculty of Industrial Design by Mathias Funk

DBSU10 - Tyria Team 05

## How to use
The microphone module consist of one microphone that measures the surrounding sound and transforms it into decibels. This process consist of two phases. 1. Data sampling and 2. data transformation. 
Use the Serial.println() for values and stage confirmation.

**Data sampling**

The data sampling consist of all values within one second. The maximum value of this sample is the desired result within this phase. To retrieve the maximum value, all values will be first stored within the "sampleSecondArray". After sampling, the maximum value will be captured from this array and be stored within the "sampleMinuteArray". For the new sample, the "sampleSecondArray" will be wiped. 

Whenever data has been captured for the whole minute, all stored data within the "sampleMinuteArray" will be transformed for data analysis.

**Data transformation**

Basic data analysis techniques will be applied on all values within the "sampleMinuteArray". For now the data analysis consist of "average" and "maximum". These results will be transformed into decibels through the modeled exponential regression formula. More information about the decibel transformation process could be found in the [Circuitdigest blog post by Aswinth Raj](https://circuitdigest.com/microcontroller-projects/arduino-sound-level-measurement) about transforming Arduino microphone values into decibels.

## Sending data
The data will be send to the database through OOCSI. Check out the [OOCSI-Database by Glivmo](https://github.com/Glivmo/OOCSI-Database) for more information about sending and receiving the data.

## Future work
Features to be implemented in the future:
- Ambient sound based on the median
- Pre-defining array optimalization
- Sleep/interrupt
