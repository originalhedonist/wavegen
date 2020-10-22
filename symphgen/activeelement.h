#pragma once
#include "element.h"
#include "../wavelib/compositionreader.h"

class activeelement
{
public:
    int nStart;
    element e;
    compositionreader* ce;
    activeelement(int nStart, const element& e, compositionreader* ce);

    double attenuation(int nrel) const;
};

