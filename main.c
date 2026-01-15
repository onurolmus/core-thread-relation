#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "core.h"
#include "thread.h"
#include "scheduler.h"
#include "data_generator.h"
#include "analysis.h"

#define K 8
#define N 1000
#define GROUPS 50


// ------------------------------------------------------------
// Zaman ölçümü (ms)
static double now_ms(void) {
    return (double)clock() * 1000.0 / (double)CLOCKS_PER_SEC;
}

// Algoritma adları
const char* algoName(int algo) {
    switch (algo) {
        case 1: return "Round-Robin";
        case 2: return "Least-Loaded";
        case 3: return "Priority-Based";
        case 4: return "Affinity-Based";
        case 5: return "Least-Loaded+Work-Stealing";
        default: return "Unknown";
    }
}

// ------------------------------------------------------------
// Sonuçları çekirdek bazında rapor formatında bas (detay)
void printResults(Core *cores, int k, int algo) {
    int totalLoad = 0;
    int maxLoad = cores[0].load;
    int minLoad = cores[0].load;

    for (int i = 0; i < k; i++) {
        totalLoad += cores[i].load;
        if (cores[i].load > maxLoad) maxLoad = cores[i].load;
        if (cores[i].load < minLoad) minLoad = cores[i].load;
    }

    double avgLoad = (double)totalLoad / k;
    double var = loadVariance(cores, k);

    printf("\n================= DETAY SONUÇ =================\n");
    printf(" Algoritma      : %s\n", algoName(algo));
    printf(" Çekirdek sayısı: %d\n", k);
    printf(" Thread sayısı  : %d\n", N);
    printf("-----------------------------------------------\n");
    printf(" CoreID | TotalLoad(ms) | TaskCount\n");
    printf("-----------------------------------------------\n");
    for (int i = 0; i < k; i++) {
        printf(" %5d | %12d | %9d\n", cores[i].id, cores[i].load, cores[i].taskCount);
    }
    printf("-----------------------------------------------\n");
    printf(" Toplam yük     : %d\n", totalLoad);
    printf(" Ortalama yük   : %.2f\n", avgLoad);
    printf(" Min / Max yük  : %d / %d\n", minLoad, maxLoad);
    printf(" Varyans (Cost) : %.4f\n", var);
    printf("===============================================\n\n");
}

// ------------------------------------------------------------
// Karşılaştırma tablosu (algoritma bazında)
void printCompareTable(Result *res, int count) {
    printf("\n================= KARŞILAŞTIRMA TABLOSU =================\n");
    printf("Algoritma | MaxLoad | Cost(Var) | Speed(1/Max) | Efficiency(S/C)\n");
    printf("-------------------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%8d | %7d | %8.4f | %11.8f | %14.12f\n",
               res[i].algo,
               res[i].maxLoad,
               res[i].variance,
               res[i].speed,
               res[i].efficiency);
    }
    printf("==========================================================\n\n");
}

// CSV export (Excel scatter için ideal)
void exportCSV(const char *filename, Result *res, int count) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("CSV dosyası açılamadı: %s\n", filename);
        return;
    }

    // Excel için: X=CostVariance, Y=Speed
    fprintf(f, "AlgorithmID,AlgorithmName,MaxLoad,CostVariance,Speed,Efficiency,RuntimeMS\n");
    for (int i = 0; i < count; i++) {
        fprintf(f, "%d,%s,%d,%.10f,%.12f,%.12f,%.6f\n",
                res[i].algo,
                algoName(res[i].algo),
                res[i].maxLoad,
                res[i].variance,
                res[i].speed,
                res[i].efficiency,
                res[i].runtime_ms);
    }

    fclose(f);
    printf("CSV yazıldı: %s\n", filename);
}

// ------------------------------------------------------------
// TEK YERDE simülasyon: runAlgorithm
// cores parametresi dışarıdan gelir -> main isterse detay basar
Result runAlgorithm(int algo, Thread *threads, int n, Core *cores, int k) {
    int rrIndex = 0;

    // Affinity için: thread->son core
    int lastCoreGroup[GROUPS];
    for (int g = 0; g < GROUPS; g++) lastCoreGroup[g] = -1;


    initCores(cores, k);

    double t0 = now_ms();

    for (int i = 0; i < n; i++) {
        int target;

        switch (algo) {
            case 1:
                target = assignRR(cores, k, threads[i], &rrIndex);
                break;
            case 2:
                target = assignLeastLoaded(cores, k, threads[i]);
                break;
            case 3:
                target = assignPriority(cores, k, threads[i]);
                break;
            case 4:
                target = assignAffinity(cores, k, threads[i], lastCoreGroup);
                break;
            case 5:
                // Work-stealing için önce bir baseline atama yapıyoruz (Least-Loaded)
                target = assignLeastLoaded(cores, k, threads[i]);
                break;
            default:
                target = assignLeastLoaded(cores, k, threads[i]);
                break;
        }

        cores[target].load += threads[i].burst;
        cores[target].taskCount++;
        lastCoreGroup[threads[i].group] = target;
    }

    if (algo == 5) {
        balanceWorkStealing(cores, k);
    }

    double t1 = now_ms();

    Result r;
    r.algo = algo;
    r.maxLoad = maxCoreLoad(cores, k);
    r.variance = loadVariance(cores, k);
    r.speed = 1.0 / (double)r.maxLoad;
    r.efficiency = (r.variance > 0.0) ? (r.speed / r.variance) : 0.0;
    r.runtime_ms = t1 - t0;

    return r;
}

// ------------------------------------------------------------
int main() {
    while (1) {
        int algo;

        printf("\nAlgoritma Sec:\n");
        printf(" 1 = Round-Robin\n");
        printf(" 2 = Least-Loaded\n");
        printf(" 3 = Priority-Based\n");
        printf(" 4 = Affinity-Based\n");
        printf(" 5 = Work-Stealing\n");
        printf(" 6 = Hepsini Kos (Tablo + CSV)\n");
        printf(" 0 = Cikis\n");
        printf("Seciminiz: ");
        if (scanf("%d", &algo) != 1) {
            printf("Hatalı giriş.\n");
            return 1;
        }

        if (algo == 0) {
            printf("Programdan çıkılıyor...\n");
            break;
        }

        // Aynı thread seti ile adil karşılaştırma:
        Thread threads[N];
        generateThreads(threads, N);

        if (algo == 6) {
            Result results[5];

            for (int a = 1; a <= 5; a++) {
                Core cores[K];
                results[a - 1] = runAlgorithm(a, threads, N, cores, K);
            }

            printCompareTable(results, 5);
            exportCSV("results.csv", results, 5);

            printf("Excel için: Scatter Plot -> X=CostVariance, Y=Speed\n");
            printf("(results.csv dosyasını aç)\n");
        } else {
            Core cores[K];
            Result r = runAlgorithm(algo, threads, N, cores, K);

            // Detaylı tablo (rapora koymalık)
            printResults(cores, K, algo);

            // Tek satır özet
            printf("Özet -> Algo=%s | MaxLoad=%d | Cost(Var)=%.4f | Speed=%.8f | Eff=%.12f | Runtime=%.3f ms\n",
                   algoName(algo), r.maxLoad, r.variance, r.speed, r.efficiency, r.runtime_ms);

            // Tek algoritma için de CSV istersen:
            // Result one[1] = { r };
            // exportCSV("single_result.csv", one, 1);
        }
    }

    return 0;
}
