// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#define _USE_MATH_DEFINES
#define exprtk_disable_caseinsensitivity

#include "framework.h"

#ifdef _MSC_VER
#define TMPNAM(f) tmpnam_s(f, FILENAME_MAX)
#else
#define TMPNAM(f) mkstemp(f)
#endif

#include "exprtk.hpp"
#include "json.hpp"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <iomanip>
#endif //PCH_H
