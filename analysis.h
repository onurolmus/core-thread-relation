#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "core.h"

typedef struct {
    int algo;
    int maxLoad;
    double variance;
    double speed;       // 1 / maxLoad
    double efficiency;  // speed / variance
    double runtime_ms;  // ölçülen süre (opsiyonel)
} Result;

int maxCoreLoad(Core *cores, int k);


double loadVariance(Core *cores, int k);

#endif
