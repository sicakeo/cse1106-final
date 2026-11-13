Test Plan: Smart Classroom Comfort Monitor

1. Purpose

Verify that the isOutOfRange() function correctly identifies temperatures inside and outside the acceptable range.

2. Test Cases
Test ID	Temperature (°F)	Expected Result	Description
T1	75	0	Normal temperature
T2	64	1	Below minimum
T3	76	0	Within acceptable range
T4	64	1	Below minimum
T5	68	0	At minimum boundary
3. Expected Results
0 means the temperature is within the acceptable range.
1 means the temperature is outside the acceptable range.
4. Success Criteria

All five test cases should pass, confirming that the function correctly handles normal, below-minimum, and boundary temperatures.
