MPU6050 is a 3 axis accelerometer, 3 axis gyroscope and tempture sensor.

This program initialises the MPUs gyroscopes, calibrates them.

The readings from the mpu are in degrees/second in 16bit twos compliment form, this is condtiioned first into a floating point variable.

Since the reading is in a rate and not an absolute (degrees/second not just degrees), it is necessary to now how much time has elapsed from one reading to the next so as to muplitple the current reading by the time interval (degrees/time)*time = degrees. this is achieved by having a timer increment a counter once every milisecond. Each time the MPU is read the counter is read and compared with what the counter was the last time it was read, giving the time interval (time) required for the above calculation.

this library was wrote from scratch using the MPU6050 datasheet