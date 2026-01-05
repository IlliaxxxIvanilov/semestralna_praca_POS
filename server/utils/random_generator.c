#include "random_generator.h"
#include <stdlib.h>
#include <time.h>

void random_generator_seed() {
    srand(time(NULL));
}

int random_generator_int(int min, int max) {
    return min + rand() % (max - min + 1);
}