#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

#define N             100000000
#define STUD_ID       331131
#define BLOCK_SIZE    (10 * STUD_ID)

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <thread_count>\n", argv[0]);
        return 1;
    }

    int thread_count = atoi(argv[1]);
    if (thread_count <= 0) {
        fprintf(stderr, "Invalid thread count: %s\n", argv[1]);
        return 1;
    }
    int i;

    omp_set_num_threads(thread_count);

    const double invN = 1.0 / (double)N;
    double sum = 0.0;

    double t0 = omp_get_wtime();

    #pragma omp parallel for schedule(dynamic, BLOCK_SIZE) reduction(+:sum)
    for (i = 0; i < N; i++) {
        double x = (i + 0.5) * invN;
        sum += 4.0 / (1.0 + x * x);
    }

    double t1 = omp_get_wtime();
    double elapsed = t1 - t0;

    // Логгирование в файл
    FILE* f = fopen("result2.log", "a");
    if (f == NULL) {
        perror("fopen");
        return 1;
    }

    fprintf(f, "Threads: %d, Time: %.6f\n", thread_count, elapsed);
    fclose(f);

    return 0;
}
