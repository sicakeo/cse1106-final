#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pigpio.h>
#include <ctype.h>
#define PIN 17

#define READING_COUNT 5
double calculateAverage(double readings[], int count) {
    double total = 0;
    for (int i = 0; i < count; i++) total += readings[i];
    return total / count;
}

int isOutOfRange(double temperature) {
    return (temperature < 68) || (temperature > 76) ? 1 : 0;
}

const char* getStatus(int outOfRange) {
    if (outOfRange)
        return "WARNING";
    return "NORMAL";
}

const char* getLEDStatus(int outOfRange) {
    if (outOfRange)
        return "ON";
    return "OF";
}

void displayReadings(double readings[], int count) {
    
    printf("%-15s %-15s %-15s\n", "Temprature Readings", "System Status", "Warning LED");
    printf("------------------------------\n");
    for (int i = 0; i < count; i++) {
        double temp = readings[i];
        printf("%-15.1f F %-15s %-15s\n", temp, getStatus(isOutOfRange(temp)), getLEDStatus(isOutOfRange(temp)));
    }
}


void checkReadings(double readings[], int count) {
    printf("\nReadings requiring attention:\n");
    for (int i = 0; i < count; i++) {
        if (isOutOfRange(readings[i])) 
            printf("%.1f F\n", readings[i]);
    }
}

void checkWarningLED(int outOfRange) {
    gpioInitialise();
	gpioSetMode(PIN, PI_OUTPUT);
	outOfRange? gpioWrite(PIN, 1) : gpioWrite(PIN, 0);
	gpioTerminate();
}


int main() {
    double readings[READING_COUNT];
    int i = 0;

    while(1 && i < READING_COUNT) {
        double temp;
        char buffer[100];
        char *endptr;
        
        printf("\nEnter the temparture readings: ");
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
        
        temp = strtof(buffer, &endptr);
        
        while(isspace((unsigned char)*endptr)) endptr++;
        
        if(endptr != buffer && *endptr == '\0') {
            readings[i] = temp;
            checkWarningLED(isOutOfRange(temp));
            i++;
        }
        else {
            printf("\nInvalid input. Try again.\n");
        }
    }
    
    
    displayReadings(readings, READING_COUNT);
    double average = calculateAverage(readings, READING_COUNT);
    printf("\nAverage Temperature: %.1f F\n", average);
    checkReadings(readings, READING_COUNT);
    return 0;
}
