#pragma once
typedef std::pair<std::string, std::map<std::string, double>> profilekey;
class element
{
public:
    element(const profilekey& profile, int fadein, int fadeout, double attenuation);
    profilekey profile;
    int fadein;
    int fadeout;
    double attenuation;
};

