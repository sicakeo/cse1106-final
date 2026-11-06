#include <stdio.h>
#include <math.h>
#define READING_COUNT 5
double calculateAverage(double readings[], int count) {
    double total = 0;
    for (int i = 0; i < count; i++) total += readings[i];
    return total / count;
}

void displayReadings(double readings[], int count) {
    printf("Temperature Readings:\n");
    for (int i = 0; i < count; i++) printf("%.1f F\n", readings[i]);
}

int isOutOfRange(double temperature) {
    return (temperature < 68) || (temperature > 76) ? 1 : 0;
}

void checkReadings(double readings[], int count) {
    printf("\nReadings requiring attention:\n");
    for (int i = 0; i < count; i++) {
        if (isOutOfRange(readings[i])) printf("%.1f F\n", readings[i]);
        }
}


int main() {
    double readings[READING_COUNT] = {72.5, 75.0, 68.0, 77.5, 70.0};
    displayReadings(readings, READING_COUNT);
    double average = calculateAverage(readings, READING_COUNT);
    printf("\nAverage Temperature: %.1f F\n", average);
    checkReadings(readings, READING_COUNT);
    return 0;
}
