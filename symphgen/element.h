#pragma once
class element
{
public:
    element(const std::string& profile, int fadein, int fadeout, double attenuation);
    std::string profile;
    int fadein;
    int fadeout;
    double attenuation;
};

