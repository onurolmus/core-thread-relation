#include "scheduler.h"

int assignRR(Core *cores, int k, Thread t, int *rrIndex) {
    int chosen = *rrIndex;
    *rrIndex = (*rrIndex + 1) % k;
    return chosen;
}

int assignLeastLoaded(Core *cores, int k, Thread t) {
    int min = 0;
    for (int i = 1; i < k; i++)
        if (cores[i].load < cores[min].load)
            min = i;
    return min;
}

int assignPriority(Core *cores, int k, Thread t) {
    // priority 0..4 (0 en yüksek)
    // yüksek öncelik -> daha agresif dengeleme: min load core
    // düşük öncelik -> RR benzeri hafifçe dağıt
    if (t.priority <= 1) {
        return assignLeastLoaded(cores, k, t);
    }

    // düşük öncelikler için: en az yüklü 2 core'dan birine
    int min1 = 0;
    for (int i = 1; i < k; i++)
        if (cores[i].load < cores[min1].load) min1 = i;

    int min2 = (min1 == 0) ? 1 : 0;
    for (int i = 0; i < k; i++) {
        if (i == min1) continue;
        if (cores[i].load < cores[min2].load) min2 = i;
    }

    // priority arttıkça min2 seçme olasılığı artsın
    // (çok basit: priority 2->min1, 3->min2, 4->min2)
    if (t.priority >= 3) return min2;
    return min1;
}


int assignAffinity(Core *cores, int k, Thread t, int *lastCoreGroup) {
    int g = t.group;

    // İlk kez geliyorsa: dengeli başlat
    if (lastCoreGroup[g] < 0 || lastCoreGroup[g] >= k) {
        return assignLeastLoaded(cores, k, t);
    }

    int preferred = lastCoreGroup[g];

    // Soft-affinity: eğer tercih edilen core çok doluysa, kaç
    int min = 0;
    for (int i = 1; i < k; i++)
        if (cores[i].load < cores[min].load) min = i;

    // Eşik: tercih edilen core, en az yüklü core’dan %25 daha fazlaysa kaç
    if (cores[preferred].load > (cores[min].load * 5) / 4) {
        return min;
    }

    return preferred;
}

void balanceWorkStealing(Core *cores, int k) {
    // Basit: en yüklüden en az yüklüye, yük farkı küçülene kadar transfer simülasyonu
    // Not: Task listesi tutmadığınız için "yük transferi" sadece load seviyesinde yapılır.
    for (int step = 0; step < 1000; step++) {
        int max = 0, min = 0;
        for (int i = 1; i < k; i++) {
            if (cores[i].load > cores[max].load) max = i;
            if (cores[i].load < cores[min].load) min = i;
        }

        int diff = cores[max].load - cores[min].load;
        if (diff <= 5) break; // yeterince dengeli

        // Diff’in küçük bir kısmını aktar
        int move = diff / 10;
        if (move < 1) move = 1;

        cores[max].load -= move;
        cores[min].load += move;
    }
}
