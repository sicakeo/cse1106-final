## Classroom Temperature Monitoring System - Design
1. The system will be developed using Python 3 on a Raspberry Pi 4B.
2. Users will manually enter classroom temperature readings in Celsius.
3. The system will compare readings against an acceptable range of 18°C–26°C.
4. Temperatures within the range will display a NORMAL status.
5. Temperatures outside the range will display a WARNING status.
6. An LED connected to the Raspberry Pi GPIO will turn OFF for NORMAL and ON for WARNING.
7. The system will calculate and display the average of all valid temperature readings.
8. Temperature readings, timestamps, statuses, and LED states will be saved to a CSV file.
9. Invalid inputs and file errors will be handled appropriately.
10. The application will allow users to exit safely and turn off the LED.
