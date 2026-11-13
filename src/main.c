#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pigpio.h>
#include <ctype.h>

#define PIN 17
#define MAX_READINGS 100

// Global variables for dynamic temperature ranges
double min_temp = 68.0;
double max_temp = 76.0;

typedef struct {
    float temperature;
    char *status;
    char *led;
} Reading;

double calculateAverage(Reading readings[], int count) {
    if (count == 0) return 0.0;
    double total = 0;
    for (int i = 0; i < count; i++) total += readings[i].temperature;
    return total / count;
}

int isOutOfRange(double temperature) {
    return (temperature < min_temp) || (temperature > max_temp) ? 1 : 0;
}

char* getStatus(int outOfRange) {
    if (outOfRange)
        return "WARNING";
    return "NORMAL";
}

char* getLEDStatus(int outOfRange) {
    if (outOfRange)
        return "ON";
    return "OFF";
}

void displayReadings(Reading readings[], int count) {
    printf("\n%-15s %-15s %-15s\n", "Temperature Readings", "System Status", "Warning LED");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        double temp = readings[i].temperature;
        printf("%-15.1f F %-15s %-15s\n", temp, getStatus(isOutOfRange(temp)), getLEDStatus(isOutOfRange(temp)));
    }
}

void checkReadings(Reading readings[], int count) {
    printf("\nReadings requiring attention:\n");
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (isOutOfRange(readings[i].temperature)) {
            printf("%.1f F\n", readings[i].temperature);
            found = 1;
        }
    }
    if (!found) printf("None.\n");
}

void checkWarningLED(int outOfRange) {
    if (gpioInitialise() >= 0) {
        gpioSetMode(PIN, PI_OUTPUT);
        outOfRange ? gpioWrite(PIN, 1) : gpioWrite(PIN, 0);
        gpioTerminate();
    }
}

// Write the overall status to the status.txt file
void updateStatusFile(int hasWarning) {
    FILE *fp = fopen("../status.txt", "w");
    if (fp == NULL) {
        perror("Error opening status.txt");
        return;
    }
    if (hasWarning) {
        fprintf(fp, "#WARNING\n");
    } else {
        fprintf(fp, "#NORMAL\n");
    }
    fclose(fp);
}

// ----Testing----
int testsRun = 0;
int testsPassed = 0;

void recordTest(const char *testName, int passed) {
    testsRun++;
    if (passed) {
        testsPassed++;
        printf("PASS: %s\n", testName);
    } else {
        printf("FAIL: %s\n", testName);
    }
}

/* Section 3 - Initial Test Suite */
void test_T1_normalTemperature(void) {
    double temperature = 75.0;
    recordTest("T1 - 75 F is within the acceptable range", isOutOfRange(temperature) == 0);
}
void test_T2_belowMinimum(void) {
    double temperature = 64.0;
    recordTest("T2 - 64 F requires attention", isOutOfRange(temperature) == 1);
}
void test_T3_aboveMaximum(void) {
    double temperature = 76;
    recordTest("T3 - 76 F is within the acceptable range", isOutOfRange(temperature) == 0);
}
void test_T4_belowMinimum(void) {
    double temperature = 64;
    recordTest("T4 - 64 F requires attention", isOutOfRange(temperature) == 1);
}
void test_T5_atMinimum(void) {
    double temperature = 68;
    recordTest("T5 - 68 F is within the acceptable range", isOutOfRange(temperature) == 0);
}

void runInitialTests(void) {
    // Temporarily force 68-76 range for tests to remain accurate regardless of user changes
    double old_min = min_temp;
    double old_max = max_temp;
    min_temp = 68.0;
    max_temp = 76.0;

    printf("\n========================================\n");
    printf("CSE 1106 INITIAL TEST SUITE\n");
    printf("========================================\n\n");
    test_T1_normalTemperature();
    test_T2_belowMinimum();
    test_T3_aboveMaximum();
    test_T4_belowMinimum();
    test_T5_atMinimum();
    
    // Restore bounds
    min_temp = old_min;
    max_temp = old_max;
}

int main() {
    Reading readings[MAX_READINGS];
    int num_readings = 0;
    int any_warning = 0;
    char input_buf[100];
    int choice;

    while(1) {
        printf("\nCurrent Acceptable Range: %.1fF - %.1fF\n", min_temp, max_temp);
        printf("1. Enter new temperature reading\n");
        printf("2. Change acceptable temperature range\n");
        printf("3. Exit and calculate average\n");
        printf("Select an option: ");
        
        if (fgets(input_buf, sizeof(input_buf), stdin) == NULL) continue;
        choice = atoi(input_buf);

        if (choice == 1) {
            if (num_readings >= MAX_READINGS) {
                printf("Maximum readings reached for this session.\n");
                continue;
            }

            double temp;
            char *endptr;
            
            printf("Enter the temperature reading: ");
            if (fgets(input_buf, sizeof(input_buf), stdin) == NULL) continue;
            
            temp = strtof(input_buf, &endptr);
            
            while(isspace((unsigned char)*endptr)) endptr++;
            
            if(endptr != input_buf && *endptr == '\0') {
                int valid = isOutOfRange(temp);
                if (valid) any_warning = 1;
                
                readings[num_readings].temperature = temp;
                readings[num_readings].status = getStatus(valid);
                readings[num_readings].led = getLEDStatus(valid);
                
                checkWarningLED(valid);
                printf("Reading accepted.\n");
                num_readings++;
            }
            else {
                printf("\nInvalid input. Try again.\n");
            }
        } 
        else if (choice == 2) {
            printf("Enter NEW minimum acceptable temperature: ");
            if (fgets(input_buf, sizeof(input_buf), stdin) != NULL) {
                min_temp = atof(input_buf);
            }
            printf("Enter NEW maximum acceptable temperature: ");
            if (fgets(input_buf, sizeof(input_buf), stdin) != NULL) {
                max_temp = atof(input_buf);
            }
            printf("Acceptable range updated successfully.\n");
        } 
        else if (choice == 3) {
            break;
        } 
        else {
            printf("Invalid option. Please enter 1, 2, or 3.\n");
        }
    }
    
    if (num_readings > 0) {
        displayReadings(readings, num_readings);
        double average = calculateAverage(readings, num_readings);
        printf("\nAverage Temperature: %.1f F\n", average);
        checkReadings(readings, num_readings);
        
        // Save status to status.txt
        updateStatusFile(any_warning);
        printf("\nSystem status written to '../status.txt'.\n");

        /* Save to CSV file. */
        FILE *fp = fopen("../data/temperature_log.csv", "a+");
        if (fp == NULL) {
            perror("Error opening the CSV file");
        } else {
            fseek(fp, 0, SEEK_END);
            if(ftell(fp) == 0) { // print header only if file is empty
                fprintf(fp, "Temperature, System Status, Warning LED\n");
            }
            
            for(int j = 0; j < num_readings; j++) {
                fprintf(fp, "%-15.1f, %-15s, %-15s\n", readings[j].temperature, readings[j].status, readings[j].led);
            }
            
            fclose(fp);
            printf("Data saved successfully to '../data/temperature_log.csv'.\n");
        }
    } else {
        printf("\nNo readings were entered during this session.\n");
    }
    
    /* The initial test suite run at the end */
    testsRun = 0;
    testsPassed = 0;
    runInitialTests();
    printf("\n========================================\n");
    printf("TEST SUMMARY\n");
    printf("========================================\n");
    printf("Tests passed: %d/%d\n", testsPassed, testsRun);

    return 0;
}