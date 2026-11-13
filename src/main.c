#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pigpio.h>
#include <ctype.h>
#define PIN 17

#define READING_COUNT 5
typedef struct {
    float temperature;
    char *status;
    char *led;
} Reading;

double calculateAverage(Reading readings[], int count) {
    double total = 0;
    for (int i = 0; i < count; i++) total += readings[i].temperature;
    return total / count;
}

int isOutOfRange(double temperature) {
    return (temperature < 68) || (temperature > 76) ? 1 : 0;
}

char* getStatus(int outOfRange) {
    if (outOfRange)
        return "WARNING";
    return "NORMAL";
}

char* getLEDStatus(int outOfRange) {
    if (outOfRange)
        return "ON";
    return "OF";
}

void displayReadings(Reading readings[], int count) {
    
    printf("%-15s %-15s %-15s\n", "Temprature Readings", "System Status", "Warning LED");
    printf("------------------------------\n");
    for (int i = 0; i < count; i++) {
        double temp = readings[i].temperature;
        printf("%-15.1f F %-15s %-15s\n", temp, getStatus(isOutOfRange(temp)), getLEDStatus(isOutOfRange(temp)));
    }
}


void checkReadings(Reading readings[], int count) {
    printf("\nReadings requiring attention:\n");
    for (int i = 0; i < count; i++) {
        if (isOutOfRange(readings[i].temperature)) 
            printf("%.1f F\n", readings[i].temperature);
    }
}

void checkWarningLED(int outOfRange) {
    gpioInitialise();
	gpioSetMode(PIN, PI_OUTPUT);
	outOfRange? gpioWrite(PIN, 1) : gpioWrite(PIN, 0);
	gpioTerminate();
}

// ----Testing----
int testsRun = 0;
int testsPassed = 0;
void recordTest(const char *testName, int passed)
{
testsRun++;
if (passed) {
testsPassed++;
printf("PASS: %s\n", testName);
} else {
printf("FAIL: %s\n", testName);
}
}
/* Section 3 - Initial Test Suite */
/* T1: 75 F is within the acceptable range. */
void test_T1_normalTemperature(void) {
double temperature = 75.0;
recordTest("T1 - 75 F is within the acceptable range",
isOutOfRange(temperature) == 0);
}
/* T2: 64 F is below the minimum and should require attention. */
void test_T2_belowMinimum(void) {
double temperature = 64.0;
recordTest("T2 - 64 F requires attention",
isOutOfRange(temperature) == 1);
}
/* T3: 78.5 F is above the maximum and should require attention. */
void test_T3_aboveMaximum(void) {
double temperature = 76;
recordTest("T3 - 76 F is within the acceptable range",
isOutOfRange(temperature) == 0);
}

/* T4: One value below the minimum. */
void test_T4_belowMinimum(void) {
double temperature = 64;
recordTest("T4 - 64 F requires attention",
isOutOfRange(temperature) == 1);
}

/* T5: The minimum */
void test_T5_atMinimum(void) {
double temperature = 68;
recordTest("T5 - 68 F requires attention",
isOutOfRange(temperature) == 0);
}

void runInitialTests(void) {
printf("\n========================================\n");
printf("CSE 1106 INITIAL TEST SUITE\n");
printf("========================================\n\n");
test_T1_normalTemperature();
test_T2_belowMinimum();
test_T3_aboveMaximum();
test_T4_belowMinimum();
test_T5_atMinimum();
}

int main() {
    Reading readings[READING_COUNT];
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
            
            int valid = isOutOfRange(temp);
            readings[i].temperature = temp;
            readings[i].status = getStatus(valid);
            readings[i].led = getLEDStatus(valid);
            checkWarningLED(valid);
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
    
    /* Save to csv file. */
    FILE *fp = fopen("../data/temperature_log.csv", "w");
    if (fp == NULL) {
        perror("Error opening the file");
        return 1;
    }
    
    fprintf(fp, "Temperature, System Status, Warning LED\n");
    
    for(int i = 0; i < READING_COUNT; i++) {
        fprintf(fp, "%-15.1f %-15s %-15s \n", readings[i].temperature, readings[i].status, readings[i].led);
    }
    
    fclose(fp);
    printf("Data saved successfully.\n");
    
    /* The initial test suite. */
    testsRun = 0;
    testsPassed = 0;
    runInitialTests();
    printf("\n========================================\n");
    printf("TEST SUMMARY\n");
    printf("========================================\n");
    printf("Tests passed: %d/%d\n", testsPassed, testsRun);

    return 0;
}
