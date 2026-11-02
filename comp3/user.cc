#include "user.h"
#include <iostream>

std::set<Gender> get_gender_prefs(const std::string prefs_str) {
    std::set<Gender> result;
    if (prefs_str == "people of all genders") {
        result.insert(MALE);
        result.insert(FEMALE);
        result.insert(NONBINARY);
        result.insert(UNSPECIFIED);
        return result;
    }

    size_t and_idx = prefs_str.find("&");
    if (and_idx < prefs_str.size()) {
        std::string first = prefs_str.substr(0, and_idx - 1);
        std::string second = prefs_str.substr(and_idx + 2,
                                            prefs_str.size() - and_idx - 2);
        if (first == "men" || second == "men") {
            result.insert(MALE);
        }
        if (first == "women" || second == "women") {
            result.insert(FEMALE);
        }
        if (first == "nonbinary people" || second == "nonbinary people") {
            result.insert(NONBINARY);
        }
    } else {
        if (prefs_str == "men") {
            result.insert(MALE);
        } else if (prefs_str == "women") {
            result.insert(FEMALE);
        } else if (prefs_str == "nonbinary people") {
            result.insert(NONBINARY);
        }
    }

    return result;
}
