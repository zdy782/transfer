#include "../common/blas_case_common.h"

#ifndef APPLY_VECTORIZATION_BASELINE_FUNCTION
#define APPLY_VECTORIZATION_BASELINE_FUNCTION cblas_sdot_baseline
#endif

#ifndef APPLY_VECTORIZATION_AUTOVEC_FUNCTION
#define APPLY_VECTORIZATION_AUTOVEC_FUNCTION cblas_sdot_autovec
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

#ifndef APPLY_VECTORIZATION_DOT_REL_TOLERANCE
#define APPLY_VECTORIZATION_DOT_REL_TOLERANCE 0.005f
#endif

float APPLY_VECTORIZATION_BASELINE_FUNCTION(
    int n, const float *x, int incx, const float *y, int incy
);
float APPLY_VECTORIZATION_AUTOVEC_FUNCTION(
    int n, const float *x, int incx, const float *y, int incy
);
float APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
    int n, const float *x, int incx, const float *y, int incy
);

static double run_kernel(
    float (*kernel)(int, const float *, int, const float *, int),
    int n,
    int iters,
    const float *x,
    const float *y,
    float *result_out
) {
    double start = apply_vectorization_seconds();
    volatile float sink = 0.0f;

    for (int iter = 0; iter < iters; ++iter) {
        sink += kernel(n, x, 1, y, 1);
    }

    *result_out = sink / (float)iters;
    return (apply_vectorization_seconds() - start) / (double)iters;
}

int main(int argc, char **argv) {
    const int n = apply_vectorization_arg_int(
        argc, argv, "-n", APPLY_VECTORIZATION_DOT_N
    );
    const int iters = apply_vectorization_arg_int(
        argc, argv, "-iters", APPLY_VECTORIZATION_DOT_ITERS
    );
    float tolerance;
    float *x = (float *)malloc(sizeof(float) * (size_t)n);
    float *y = (float *)malloc(sizeof(float) * (size_t)n);
    float baseline_value = 0.0f;
    float autovec_value = 0.0f;
    float optimized_value = 0.0f;
    double baseline_seconds;
    double autovec_seconds;
    double optimized_seconds;
    float autovec_diff;
    float optimized_diff;

    if (x == NULL || y == NULL) {
        free(x);
        free(y);
        return apply_vectorization_report_alloc_failure("cblas_sdot");
    }

    apply_vectorization_fill_vector(x, n, 0.0625f);
    apply_vectorization_fill_vector(y, n, 0.046875f);

    baseline_seconds = run_kernel(
        APPLY_VECTORIZATION_BASELINE_FUNCTION, n, iters, x, y, &baseline_value
    );
    autovec_seconds = run_kernel(
        APPLY_VECTORIZATION_AUTOVEC_FUNCTION, n, iters, x, y, &autovec_value
    );
    optimized_seconds = run_kernel(
        APPLY_VECTORIZATION_OPTIMIZED_FUNCTION, n, iters, x, y, &optimized_value
    );

    autovec_diff = apply_vectorization_absf(baseline_value - autovec_value);
    optimized_diff = apply_vectorization_absf(baseline_value - optimized_value);
    tolerance = APPLY_VECTORIZATION_DOT_TOLERANCE * (float)n;
    {
        float relative_tolerance =
            APPLY_VECTORIZATION_DOT_REL_TOLERANCE
            * apply_vectorization_absf(baseline_value);
        if (relative_tolerance > tolerance) {
            tolerance = relative_tolerance;
        }
    }

    printf("基线结果=%.6f\n", baseline_value);
    printf("自动向量化结果=%.6f\n", autovec_value);
    printf("优化结果=%.6f\n", optimized_value);
    printf("自动向量化最大绝对误差=%.8f\n", autovec_diff);
    printf("优化后最大绝对误差=%.8f\n", optimized_diff);

    if (autovec_diff > tolerance || optimized_diff > tolerance) {
        apply_vectorization_print_summary_three(
            baseline_seconds,
            autovec_seconds,
            optimized_seconds,
            "失败",
            "dot mismatch"
        );
        free(x);
        free(y);
        return 2;
    }

    apply_vectorization_print_summary_three(
        baseline_seconds,
        autovec_seconds,
        optimized_seconds,
        "通过",
        "dot ok"
    );

    free(x);
    free(y);
    return 0;
}
