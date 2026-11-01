#pragma once
#include <cstdio>

// expect(cond, msg):
//     Prints "FAIL: msg" if `cond` is false. Tests return `failures` so that
//     a nonzero exit code means at least one expectation failed.

static int failures = 0;

static void expect(bool cond, const char* msg) {
    if (!cond) {
        std::printf("FAIL: %s\n", msg);
        ++failures;
    }
}
