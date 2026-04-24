#include "../common/blas_case_common.h"

#ifndef APPLY_VECTORIZATION_BASELINE_FUNCTION
#define APPLY_VECTORIZATION_BASELINE_FUNCTION cblas_sscal_baseline
#endif

#ifndef APPLY_VECTORIZATION_OPTIMIZED_FUNCTION
#define APPLY_VECTORIZATION_OPTIMIZED_FUNCTION cblas_sscal_optimized
#endif

#ifndef APPLY_VECTORIZATION_SSCAL_N
#define APPLY_VECTORIZATION_SSCAL_N 4096
#endif

#ifndef APPLY_VECTORIZATION_SSCAL_ITERS
#define APPLY_VECTORIZATION_SSCAL_ITERS 2048
#endif

#ifndef APPLY_VECTORIZATION_SSCAL_TOLERANCE
#define APPLY_VECTORIZATION_SSCAL_TOLERANCE 0.0001f
#endif

#ifndef APPLY_VECTORIZATION_SSCAL_CHECKSUM_TOLERANCE
#define APPLY_VECTORIZATION_SSCAL_CHECKSUM_TOLERANCE 0.001
#endif

void APPLY_VECTORIZATION_BASELINE_FUNCTION(int n, float alpha, float *x, int incx);
void APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(int n, float alpha, float *x, int incx);

static double run_kernel(
    void (*kernel)(int, float, float *, int),
    int n,
    float alpha,
    const float *seed_x,
    float *scratch_x
) {
    double start = apply_vectorization_seconds();

    for (int iter = 0; iter < APPLY_VECTORIZATION_SSCAL_ITERS; ++iter) {
        apply_vectorization_copy_f32(scratch_x, seed_x, (size_t)n);
        kernel(n, alpha, scratch_x, 1);
    }

    return apply_vectorization_seconds() - start;
}

int main(void) {
    const float alpha = -0.75f;
    float *seed_x = (float *)malloc(sizeof(float) * (size_t)APPLY_VECTORIZATION_SSCAL_N);
    float *baseline_x = (float *)malloc(sizeof(float) * (size_t)APPLY_VECTORIZATION_SSCAL_N);
    float *optimized_x = (float *)malloc(sizeof(float) * (size_t)APPLY_VECTORIZATION_SSCAL_N);
    double baseline_seconds;
    double optimized_seconds;
    double baseline_sum;
    double optimized_sum;
    double checksum_gap;
    float diff;

    if (seed_x == NULL || baseline_x == NULL || optimized_x == NULL) {
        free(seed_x);
        free(baseline_x);
        free(optimized_x);
        return apply_vectorization_report_alloc_failure("cblas_sscal");
    }

    apply_vectorization_fill_vector(seed_x, APPLY_VECTORIZATION_SSCAL_N, 0.0546875f);

    baseline_seconds = run_kernel(
        APPLY_VECTORIZATION_BASELINE_FUNCTION,
        APPLY_VECTORIZATION_SSCAL_N,
        alpha,
        seed_x,
        baseline_x
    );
    optimized_seconds = run_kernel(
        APPLY_VECTORIZATION_OPTIMIZED_FUNCTION,
        APPLY_VECTORIZATION_SSCAL_N,
        alpha,
        seed_x,
        optimized_x
    );

    apply_vectorization_copy_f32(baseline_x, seed_x, APPLY_VECTORIZATION_SSCAL_N);
    apply_vectorization_copy_f32(optimized_x, seed_x, APPLY_VECTORIZATION_SSCAL_N);
    APPLY_VECTORIZATION_BASELINE_FUNCTION(
        APPLY_VECTORIZATION_SSCAL_N,
        alpha,
        baseline_x,
        1
    );
    APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
        APPLY_VECTORIZATION_SSCAL_N,
        alpha,
        optimized_x,
        1
    );

    baseline_sum = apply_vectorization_checksum_f32(
        baseline_x,
        APPLY_VECTORIZATION_SSCAL_N
    );
    optimized_sum = apply_vectorization_checksum_f32(
        optimized_x,
        APPLY_VECTORIZATION_SSCAL_N
    );
    checksum_gap = apply_vectorization_absd(baseline_sum - optimized_sum);
    diff = apply_vectorization_max_abs_diff_f32(
        baseline_x,
        optimized_x,
        APPLY_VECTORIZATION_SSCAL_N
    );

    printf("标量校验和=%.6f\n", baseline_sum);
    printf("优化后校验和=%.6f\n", optimized_sum);
    printf("最大绝对误差=%.8f\n", diff);

    if (diff > APPLY_VECTORIZATION_SSCAL_TOLERANCE
        || checksum_gap > APPLY_VECTORIZATION_SSCAL_CHECKSUM_TOLERANCE) {
        apply_vectorization_print_summary(
            baseline_seconds,
            optimized_seconds,
            "失败",
            "checksum mismatch"
        );
        free(seed_x);
        free(baseline_x);
        free(optimized_x);
        return 2;
    }

    apply_vectorization_print_summary(
        baseline_seconds,
        optimized_seconds,
        "通过",
        "checksum ok"
    );

    free(seed_x);
    free(baseline_x);
    free(optimized_x);
    return 0;
}
