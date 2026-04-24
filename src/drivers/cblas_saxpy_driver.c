#include "../common/blas_case_common.h"

#ifndef APPLY_VECTORIZATION_BASELINE_FUNCTION
#define APPLY_VECTORIZATION_BASELINE_FUNCTION cblas_saxpy_baseline
#endif

#ifndef APPLY_VECTORIZATION_OPTIMIZED_FUNCTION
#define APPLY_VECTORIZATION_OPTIMIZED_FUNCTION cblas_saxpy_optimized
#endif

#ifndef APPLY_VECTORIZATION_SAXPY_N
#define APPLY_VECTORIZATION_SAXPY_N 4096
#endif

#ifndef APPLY_VECTORIZATION_SAXPY_ITERS
#define APPLY_VECTORIZATION_SAXPY_ITERS 2048
#endif

#ifndef APPLY_VECTORIZATION_SAXPY_TOLERANCE
#define APPLY_VECTORIZATION_SAXPY_TOLERANCE 0.0001f
#endif

#ifndef APPLY_VECTORIZATION_SAXPY_CHECKSUM_TOLERANCE
#define APPLY_VECTORIZATION_SAXPY_CHECKSUM_TOLERANCE 0.001
#endif

void APPLY_VECTORIZATION_BASELINE_FUNCTION(
    int n, float alpha, const float *x, int incx, float *y, int incy
);
void APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
    int n, float alpha, const float *x, int incx, float *y, int incy
);

static double run_kernel(
    void (*kernel)(int, float, const float *, int, float *, int),
    int n,
    float alpha,
    const float *x,
    const float *seed_y,
    float *scratch_y
) {
    double start = apply_vectorization_seconds();

    for (int iter = 0; iter < APPLY_VECTORIZATION_SAXPY_ITERS; ++iter) {
        apply_vectorization_copy_f32(scratch_y, seed_y, (size_t)n);
        kernel(n, alpha, x, 1, scratch_y, 1);
    }

    return (apply_vectorization_seconds() - start)
        / (double)APPLY_VECTORIZATION_SAXPY_ITERS;
}

int main(void) {
    const float alpha = 1.25f;
    float *x = (float *)malloc(sizeof(float) * (size_t)APPLY_VECTORIZATION_SAXPY_N);
    float *seed_y = (float *)malloc(sizeof(float) * (size_t)APPLY_VECTORIZATION_SAXPY_N);
    float *baseline_y = (float *)malloc(sizeof(float) * (size_t)APPLY_VECTORIZATION_SAXPY_N);
    float *optimized_y = (float *)malloc(sizeof(float) * (size_t)APPLY_VECTORIZATION_SAXPY_N);
    double baseline_seconds;
    double optimized_seconds;
    double baseline_sum;
    double optimized_sum;
    double checksum_gap;
    float diff;

    if (x == NULL || seed_y == NULL || baseline_y == NULL || optimized_y == NULL) {
        free(x);
        free(seed_y);
        free(baseline_y);
        free(optimized_y);
        return apply_vectorization_report_alloc_failure("cblas_saxpy");
    }

    apply_vectorization_fill_vector(x, APPLY_VECTORIZATION_SAXPY_N, 0.03125f);
    apply_vectorization_fill_vector(seed_y, APPLY_VECTORIZATION_SAXPY_N, 0.046875f);

    baseline_seconds = run_kernel(
        APPLY_VECTORIZATION_BASELINE_FUNCTION,
        APPLY_VECTORIZATION_SAXPY_N,
        alpha,
        x,
        seed_y,
        baseline_y
    );
    optimized_seconds = run_kernel(
        APPLY_VECTORIZATION_OPTIMIZED_FUNCTION,
        APPLY_VECTORIZATION_SAXPY_N,
        alpha,
        x,
        seed_y,
        optimized_y
    );

    apply_vectorization_copy_f32(baseline_y, seed_y, APPLY_VECTORIZATION_SAXPY_N);
    apply_vectorization_copy_f32(optimized_y, seed_y, APPLY_VECTORIZATION_SAXPY_N);
    APPLY_VECTORIZATION_BASELINE_FUNCTION(
        APPLY_VECTORIZATION_SAXPY_N,
        alpha,
        x,
        1,
        baseline_y,
        1
    );
    APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
        APPLY_VECTORIZATION_SAXPY_N,
        alpha,
        x,
        1,
        optimized_y,
        1
    );

    baseline_sum = apply_vectorization_checksum_f32(
        baseline_y,
        APPLY_VECTORIZATION_SAXPY_N
    );
    optimized_sum = apply_vectorization_checksum_f32(
        optimized_y,
        APPLY_VECTORIZATION_SAXPY_N
    );
    checksum_gap = apply_vectorization_absd(baseline_sum - optimized_sum);
    diff = apply_vectorization_max_abs_diff_f32(
        baseline_y,
        optimized_y,
        APPLY_VECTORIZATION_SAXPY_N
    );

    printf("标量校验和=%.6f\n", baseline_sum);
    printf("优化后校验和=%.6f\n", optimized_sum);
    printf("最大绝对误差=%.8f\n", diff);

    if (diff > APPLY_VECTORIZATION_SAXPY_TOLERANCE
        || checksum_gap > APPLY_VECTORIZATION_SAXPY_CHECKSUM_TOLERANCE) {
        apply_vectorization_print_summary(
            baseline_seconds,
            optimized_seconds,
            "失败",
            "checksum mismatch"
        );
        free(x);
        free(seed_y);
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
    free(seed_y);
    free(baseline_y);
    free(optimized_y);
    return 0;
}
