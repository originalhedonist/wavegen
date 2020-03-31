#pragma once
#include "element.h"
#include "../wavelib/compositionreader.h"

class activeelement
{
public:
    int nStart;
    element e;
    compositionreader* ce;
    activeelement(int nStart, const element& e, compositionreader* ce):
        nStart(nStart), e(e), ce(ce) {}

    double attenuation(int nrel) const
    {
        int fadeinsmp = e.fadein * ce->header.sampling_frequency;
        int fadeoutsmp = e.fadeout * ce->header.sampling_frequency;
        int lensmp = ce->header.length_seconds * ce->header.sampling_frequency;
        if (nrel < fadeinsmp)
            return ((double)nrel) / fadeinsmp;

        if (nrel > lensmp - fadeoutsmp)
            return (((double)lensmp - nrel)) / fadeoutsmp;

        return 1;
    }
};

