#include "pch.h"
#include "element.h"

element::element(const profilekey& profile, int fadein, int fadeout, double attenuation) :
    profile(profile), fadein(fadein), fadeout(fadeout), attenuation(attenuation)
{
}
