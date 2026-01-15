#include <math.h>
#include "analysis.h"

double loadVariance(Core *cores, int k) {
    double sum = 0.0;
    for (int i = 0; i < k; i++)
        sum += cores[i].load;

    double mean = sum / k;

    double var = 0.0;
    for (int i = 0; i < k; i++) {
        double diff = cores[i].load - mean;
        var += diff * diff;
    }

    return var / k;
}
int maxCoreLoad(Core *cores, int k) {
    int max = cores[0].load;
    for (int i = 1; i < k; i++)
        if (cores[i].load > max) max = cores[i].load;
    return max;
}
