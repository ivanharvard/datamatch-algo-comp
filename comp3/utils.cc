#include <cstdarg>
#include <sstream>

#include <sys/stat.h>
#include "lib/json.hpp"

#include "utils.h"

using json = nlohmann::json;

Logger Logger::logger;

void Logger::set_level(LogLevel level) {
    min_level = level;
}

void Logger::log(LogLevel level, const char* format, ...) {
    if (!out) {
        printf("No logging outfile specified!\n");
        exit(EXIT_SUCCESS);
    }

    if (level < min_level) {
        return;
    }

    std::string concatenated;
    if (level == INFO) {
        concatenated = std::string(format) + "\n";
    } else {
        concatenated = headers.at(level) + std::string(format) + "\n";
    }

    char* output = const_cast<char*>(concatenated.c_str());

    va_list args1;
    va_start(args1, format);
    va_list args2;
    va_copy(args2, args1);
    vfprintf(out, output, args1);
    vprintf(output, args2);
    va_end(args2);
    va_end(args1);
}

FILE* Logger::create_file() {
    time_t ttime = time(0);
    char* datetime = ctime(&ttime);

    std::string path = "./logs";
    struct stat st;
    if (stat(path.c_str(), &st) == -1) {
        mkdir(path.c_str(), (mode_t)0777);
    }

    std::string concatenated = path + "/" + std::string(datetime) + ".log";
    char* output = const_cast<char*>(concatenated.c_str());
    return fopen(output, "w");
}

Logger& Logger::instance() {
    return logger;
}

Logger& Logger::init(FILE* new_out) {
    // Normalize null to stderr (or stdout, your choice)
    if (!new_out) new_out = stderr;

    // If already initialized to the same stream, do nothing.
    if (logger.out == new_out) return logger;

    // Close previous stream if we own it (not std streams)
    if (logger.out && logger.out != stdout && logger.out != stderr) {
        fclose(logger.out);
    }

    logger.out = new_out;
    return logger;
}

void Logger::shutdown() {
    if (logger.out && logger.out != stdout && logger.out != stderr) {
        fclose(logger.out);
    }
    logger.out = nullptr;
}

size_t get_index(const std::vector<std::string> v, const std::string target) {
    return find(v.begin(), v.end(), target) - v.begin();
}

float normal_cdf(float x) {
    return 0.5 * erfc(-x * M_SQRT1_2);
}

bool validate_json(const json f,
                   const std::vector<std::string>& required_fields) {
    for (size_t i = 0; i < required_fields.size(); ++i) {
        if (!f.contains(required_fields[i])) {
            return false;
        }
    }
    return true;
}

bool comparator(std::pair<int, int>& a, std::pair<int, int>& b) {
    return a.second > b.second;
}

void sort_on_other(int* main, int* other, int size) {
    std::pair<int, int> pairs[size];
    for (int i = 0; i < size; ++i) {
        pairs[i] = std::make_pair(main[i], other[i]);
    }
    std::sort(pairs, pairs + size, comparator);
    for (int i = 0; i < size; ++i) {
        main[i] = pairs[i].first;
    }
}

std::vector<std::string> split(std::string str, const char delim) {
    std::vector<std::string> substrs;
    std::stringstream sstrm(str);
    while (sstrm.good()) {
        std::string substr;
        std::getline(sstrm, substr, delim);
        substrs.push_back(substr);
    }
    return substrs;
}
