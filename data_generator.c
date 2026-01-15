#include <stdlib.h>
#include <time.h>
#include "data_generator.h"

#define GROUPS 50  // üstte veya header’da

void generateThreads(Thread *threads, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        threads[i].id = i;
        threads[i].burst = (rand() % 20) + 1;
        threads[i].priority = rand() % 5;
        threads[i].group = rand() % GROUPS;   // <-- EKLE
    }
}
