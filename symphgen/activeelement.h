#pragma once
#include "element.h"
#include "../wavelib/compositionelement.h"

class activeelement
{
public:
    int nStart;
    element e;
    compositionelement* ce;
    activeelement(int nStart, const element& e, compositionelement* ce):
        nStart(nStart), e(e), ce(ce) {}
};

