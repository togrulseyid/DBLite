/** @file
* Created by Toghrul on 27.04.17.
*/

#ifndef DBLITE_DBLITE_H
#define DBLITE_DBLITE_H

#include <cstdint>
#include <cmath>
#include <ctime>
#include <cstring>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <stdarg.h>  // For va_start, va_end.
#include <iomanip>
#include "version.h"

/**
 * # String formatter
 * Returns string value of given inputs.
 * \snippet printf style string formatter
 */
inline std::string string_formatter(const std::string fmt_str, ...) {
    int final_n, n = ((int) fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
    std::string str;
    std::unique_ptr<char[]> formatted;
    va_list ap;
    while(1) {
        formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
        strcpy(&formatted[0], fmt_str.c_str());
        va_start(ap, fmt_str);
        final_n = vsnprintf(&formatted[0], (size_t) n, fmt_str.c_str(), ap);
        va_end(ap);
        if(final_n < 0 || final_n >= n)
            n += abs(final_n - n + 1);
        else
            break;
    }
    return std::string(formatted.get());
}

#endif  /* DBLITE_DBLITE_H */
