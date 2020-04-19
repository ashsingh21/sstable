#include <string>
#include <vector>

using std::vector;
using std::string;

#ifndef UTILS_H
#define UTILS_H

namespace merger_utils {
struct Record {
    std::string key;
    std::vector<char> value;
    long timestamp;
};
} // merger utils

#endif