#pragma once

class headerdata
{
public:
    static const int32_t sampling_frequency = 44100;
    const int32_t length_seconds;
    const int32_t N;
    const int16_t channels;
    static const int32_t bytes_per_sample = 2;
    const int32_t overallDataSize;
    const int32_t overallFileSize;

    headerdata(const headerdata& h) : headerdata(h.length_seconds, h.channels) {}

    headerdata(int32_t length_seconds, int16_t channels) :
        length_seconds(length_seconds),
        N(length_seconds * sampling_frequency),
        channels(channels),
        overallDataSize(N* channels* bytes_per_sample),
        overallFileSize(overallDataSize + 44)
    {}
};
