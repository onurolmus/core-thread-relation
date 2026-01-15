#ifndef CORE_H
#define CORE_H

typedef struct {
    int id;
    int load;       // o ana kadar aldığı toplam iş
    int taskCount;  // kaç thread aldı
} Core;

void initCores(Core *cores, int k);

#endif
