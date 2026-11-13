## Classroom Temperature Monitoring System - Requirements
1. The system must run on a Raspberry Pi 4B using Python 3.
2. Users must be able to enter classroom temperatures manually.
3. The system must reject empty and non-numeric inputs.
4. The acceptable temperature range must be configurable (initially 18°C–26°C).
5. The system must display NORMAL when temperatures are within range.
6. The system must display WARNING when temperatures are outside range.
7. The LED must turn ON for WARNING and OFF for NORMAL.
8. The system must calculate and display the average of recorded temperatures.
9. The system must save timestamps, temperatures, statuses, and LED states to a CSV file.
10. The system must preserve previous records and safely release GPIO resources when exiting.
