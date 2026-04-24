#include "../common/blas_case_common.h"

#ifndef APPLY_VECTORIZATION_BASELINE_FUNCTION
#define APPLY_VECTORIZATION_BASELINE_FUNCTION cblas_sdot_baseline
#endif

#ifndef APPLY_VECTORIZATION_OPTIMIZED_FUNCTION
#define APPLY_VECTORIZATION_OPTIMIZED_FUNCTION cblas_sdot_optimized
#endif

#ifndef APPLY_VECTORIZATION_DOT_N
#define APPLY_VECTORIZATION_DOT_N 4096
#endif

#ifndef APPLY_VECTORIZATION_DOT_ITERS
#define APPLY_VECTORIZATION_DOT_ITERS 4096
#endif

#ifndef APPLY_VECTORIZATION_DOT_TOLERANCE
#define APPLY_VECTORIZATION_DOT_TOLERANCE 0.0001f
#endif

float APPLY_VECTORIZATION_BASELINE_FUNCTION(
    int n, const float *x, int incx, const float *y, int incy
);
float APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
    int n, const float *x, int incx, const float *y, int incy
);

static double run_kernel(
    float (*kernel)(int, const float *, int, const float *, int),
    int n,
    const float *x,
    const float *y,
    float *result_out
) {
    double start = apply_vectorization_seconds();
    float sink = 0.0f;

    for (int iter = 0; iter < APPLY_VECTORIZATION_DOT_ITERS; ++iter) {
        sink += kernel(n, x, 1, y, 1);
    }

    *result_out = sink;
    return (apply_vectorization_seconds() - start)
        / (double)APPLY_VECTORIZATION_DOT_ITERS;
}

int main(void) {
    float *x = (float *)malloc(sizeof(float) * (size_t)APPLY_VECTORIZATION_DOT_N);
    float *y = (float *)malloc(sizeof(float) * (size_t)APPLY_VECTORIZATION_DOT_N);
    float baseline_value = 0.0f;
    float optimized_value = 0.0f;
    double baseline_seconds;
    double optimized_seconds;
    float diff;

    if (x == NULL || y == NULL) {
        free(x);
        free(y);
        return apply_vectorization_report_alloc_failure("cblas_sdot");
    }

    apply_vectorization_fill_vector(x, APPLY_VECTORIZATION_DOT_N, 0.0625f);
    apply_vectorization_fill_vector(y, APPLY_VECTORIZATION_DOT_N, 0.046875f);

    baseline_seconds = run_kernel(
        APPLY_VECTORIZATION_BASELINE_FUNCTION,
        APPLY_VECTORIZATION_DOT_N,
        x,
        y,
        &baseline_value
    );
    optimized_seconds = run_kernel(
        APPLY_VECTORIZATION_OPTIMIZED_FUNCTION,
        APPLY_VECTORIZATION_DOT_N,
        x,
        y,
        &optimized_value
    );

    diff = apply_vectorization_absf(baseline_value - optimized_value);
    printf("基线结果=%.6f\n", baseline_value);
    printf("优化结果=%.6f\n", optimized_value);
    printf("最大绝对误差=%.8f\n", diff);

    if (diff > APPLY_VECTORIZATION_DOT_TOLERANCE) {
        apply_vectorization_print_summary(
            baseline_seconds,
            optimized_seconds,
            "失败",
            "dot mismatch"
        );
        free(x);
        free(y);
        return 2;
    }

    apply_vectorization_print_summary(
        baseline_seconds,
        optimized_seconds,
        "通过",
        "dot ok"
    );

    free(x);
    free(y);
    return 0;
}
