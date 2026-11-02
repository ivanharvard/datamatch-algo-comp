#ifndef CONFIG_H
#define CONFIG_H

// Matching targets per person. The first matching round aims for SPONSORED
// "can date" matches (never more than MAX_SPONSORED); the second aims for
// TOTAL matches overall (never more than MAX_TOTAL). Targets aren't
// guaranteed: someone with few compatible people can end up with fewer.
#define SPONSORED         2
#define MAX_SPONSORED     2
#define TOTAL             10
#define MAX_TOTAL         17

// Added to a pair's score to mark it as sponsored ("can date").
#define SPONSORED_CUTOFF  1e15

#define NUM_OPTIONS       5

// A user's year number is FRESHMAN_CLASS minus the "year" on their profile
// ("grad" and "alumni" become 5). Only the difference between two users'
// year numbers is used: for the class-year gap dealbreaker and the class-year
// factor (factors/class_year.cc).
#define FRESHMAN_CLASS    2026

// TO-DO: add cutoffs for # LOVE and # FRIENDSHIP matches if people chose both
// Revise weighting constants
// Make sure red flags aren't extremely wrong

#endif
