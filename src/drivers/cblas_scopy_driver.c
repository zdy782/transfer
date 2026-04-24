#include "../common/blas_case_common.h"

#ifndef APPLY_VECTORIZATION_BASELINE_FUNCTION
#define APPLY_VECTORIZATION_BASELINE_FUNCTION cblas_scopy_baseline
#endif

#ifndef APPLY_VECTORIZATION_OPTIMIZED_FUNCTION
#define APPLY_VECTORIZATION_OPTIMIZED_FUNCTION cblas_scopy_optimized
#endif

#ifndef APPLY_VECTORIZATION_SCOPY_N
#define APPLY_VECTORIZATION_SCOPY_N 4096
#endif

#ifndef APPLY_VECTORIZATION_SCOPY_ITERS
#define APPLY_VECTORIZATION_SCOPY_ITERS 2048
#endif

#ifndef APPLY_VECTORIZATION_SCOPY_TOLERANCE
#define APPLY_VECTORIZATION_SCOPY_TOLERANCE 0.0001f
#endif

#ifndef APPLY_VECTORIZATION_SCOPY_CHECKSUM_TOLERANCE
#define APPLY_VECTORIZATION_SCOPY_CHECKSUM_TOLERANCE 0.001
#endif

void APPLY_VECTORIZATION_BASELINE_FUNCTION(
    int n, const float *x, int incx, float *y, int incy
);
void APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
    int n, const float *x, int incx, float *y, int incy
);

static double run_kernel(
    void (*kernel)(int, const float *, int, float *, int),
    int n,
    const float *x,
    float *scratch_y
) {
    double start = apply_vectorization_seconds();

    for (int iter = 0; iter < APPLY_VECTORIZATION_SCOPY_ITERS; ++iter) {
        kernel(n, x, 1, scratch_y, 1);
    }

    return apply_vectorization_seconds() - start;
}

int main(void) {
    float *x = (float *)malloc(sizeof(float) * (size_t)APPLY_VECTORIZATION_SCOPY_N);
    float *baseline_y = (float *)malloc(sizeof(float) * (size_t)APPLY_VECTORIZATION_SCOPY_N);
    float *optimized_y = (float *)malloc(sizeof(float) * (size_t)APPLY_VECTORIZATION_SCOPY_N);
    double baseline_seconds;
    double optimized_seconds;
    double baseline_sum;
    double optimized_sum;
    double checksum_gap;
    float diff;

    if (x == NULL || baseline_y == NULL || optimized_y == NULL) {
        free(x);
        free(baseline_y);
        free(optimized_y);
        return apply_vectorization_report_alloc_failure("cblas_scopy");
    }

    apply_vectorization_fill_vector(x, APPLY_VECTORIZATION_SCOPY_N, 0.0390625f);
    memset(baseline_y, 0, sizeof(float) * (size_t)APPLY_VECTORIZATION_SCOPY_N);
    memset(optimized_y, 0, sizeof(float) * (size_t)APPLY_VECTORIZATION_SCOPY_N);

    baseline_seconds = run_kernel(
        APPLY_VECTORIZATION_BASELINE_FUNCTION,
        APPLY_VECTORIZATION_SCOPY_N,
        x,
        baseline_y
    );
    optimized_seconds = run_kernel(
        APPLY_VECTORIZATION_OPTIMIZED_FUNCTION,
        APPLY_VECTORIZATION_SCOPY_N,
        x,
        optimized_y
    );

    memset(baseline_y, 0, sizeof(float) * (size_t)APPLY_VECTORIZATION_SCOPY_N);
    memset(optimized_y, 0, sizeof(float) * (size_t)APPLY_VECTORIZATION_SCOPY_N);
    APPLY_VECTORIZATION_BASELINE_FUNCTION(
        APPLY_VECTORIZATION_SCOPY_N,
        x,
        1,
        baseline_y,
        1
    );
    APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
        APPLY_VECTORIZATION_SCOPY_N,
        x,
        1,
        optimized_y,
        1
    );

    baseline_sum = apply_vectorization_checksum_f32(
        baseline_y,
        APPLY_VECTORIZATION_SCOPY_N
    );
    optimized_sum = apply_vectorization_checksum_f32(
        optimized_y,
        APPLY_VECTORIZATION_SCOPY_N
    );
    checksum_gap = apply_vectorization_absd(baseline_sum - optimized_sum);
    diff = apply_vectorization_max_abs_diff_f32(
        baseline_y,
        optimized_y,
        APPLY_VECTORIZATION_SCOPY_N
    );

    printf("标量校验和=%.6f\n", baseline_sum);
    printf("优化后校验和=%.6f\n", optimized_sum);
    printf("最大绝对误差=%.8f\n", diff);

    if (diff > APPLY_VECTORIZATION_SCOPY_TOLERANCE
        || checksum_gap > APPLY_VECTORIZATION_SCOPY_CHECKSUM_TOLERANCE) {
        apply_vectorization_print_summary(
            baseline_seconds,
            optimized_seconds,
            "失败",
            "checksum mismatch"
        );
        free(x);
        free(baseline_y);
        free(optimized_y);
        return 2;
    }

    apply_vectorization_print_summary(
        baseline_seconds,
        optimized_seconds,
        "通过",
        "checksum ok"
    );

    free(x);
    free(baseline_y);
    free(optimized_y);
    return 0;
}
