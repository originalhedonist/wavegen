#pragma once
class element
{
public:
    element(const std::string& profile, int fadein, int fadeout);
    std::string profile;
    int fadein;
    int fadeout;
};

