#pragma once
#include "stdafx.h"
class c
{
public:
    static const int32_t sampling_frequency = 44100;
    static const int32_t length_seconds = 1 * 60;// 1 min
    static const int32_t N = length_seconds * sampling_frequency;
    static const int16_t channels = 2;
    static const int32_t bytes_per_sample = 2;
    static const int32_t overallDataSize = N * channels * bytes_per_sample;
    static const int32_t overallFileSize = overallDataSize + 44;
};

