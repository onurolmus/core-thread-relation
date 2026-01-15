#include "core.h"

void initCores(Core *cores, int k) {
    for (int i = 0; i < k; i++) {
        cores[i].id = i;
        cores[i].load = 0;
        cores[i].taskCount = 0;
    }
}
