#include "../common/blas_case_common.h"

#ifndef APPLY_VECTORIZATION_BASELINE_FUNCTION
#define APPLY_VECTORIZATION_BASELINE_FUNCTION cblas_ssyrk_baseline
#endif

#ifndef APPLY_VECTORIZATION_AUTOVEC_FUNCTION
#define APPLY_VECTORIZATION_AUTOVEC_FUNCTION cblas_ssyrk_autovec
#endif

#ifndef APPLY_VECTORIZATION_OPTIMIZED_FUNCTION
#define APPLY_VECTORIZATION_OPTIMIZED_FUNCTION cblas_ssyrk_optimized
#endif

#ifndef APPLY_VECTORIZATION_SSYRK_N
#define APPLY_VECTORIZATION_SSYRK_N 64
#endif

#ifndef APPLY_VECTORIZATION_SSYRK_K
#define APPLY_VECTORIZATION_SSYRK_K 48
#endif

#ifndef APPLY_VECTORIZATION_SSYRK_ITERS
#define APPLY_VECTORIZATION_SSYRK_ITERS 256
#endif

#ifndef APPLY_VECTORIZATION_SSYRK_TOLERANCE
#define APPLY_VECTORIZATION_SSYRK_TOLERANCE 0.0001f
#endif

#ifndef APPLY_VECTORIZATION_SSYRK_CHECKSUM_TOLERANCE
#define APPLY_VECTORIZATION_SSYRK_CHECKSUM_TOLERANCE 0.001
#endif

void APPLY_VECTORIZATION_BASELINE_FUNCTION(
    CBLAS_ORDER order,
    CBLAS_UPLO uplo,
    CBLAS_TRANSPOSE trans,
    int n,
    int k,
    float alpha,
    const float *a,
    int lda,
    float beta,
    float *c,
    int ldc
);
void APPLY_VECTORIZATION_AUTOVEC_FUNCTION(
    CBLAS_ORDER order,
    CBLAS_UPLO uplo,
    CBLAS_TRANSPOSE trans,
    int n,
    int k,
    float alpha,
    const float *a,
    int lda,
    float beta,
    float *c,
    int ldc
);
void APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
    CBLAS_ORDER order,
    CBLAS_UPLO uplo,
    CBLAS_TRANSPOSE trans,
    int n,
    int k,
    float alpha,
    const float *a,
    int lda,
    float beta,
    float *c,
    int ldc
);

static double run_kernel(
    void (*kernel)(
        CBLAS_ORDER,
        CBLAS_UPLO,
        CBLAS_TRANSPOSE,
        int,
        int,
        float,
        const float *,
        int,
        float,
        float *,
        int
    ),
    int n,
    int k,
    int iters,
    const float *a,
    const float *seed_c,
    float *scratch_c
) {
    double start = apply_vectorization_seconds();

    for (int iter = 0; iter < iters; ++iter) {
        apply_vectorization_copy_f32(scratch_c, seed_c, (size_t)n * (size_t)n);
        kernel(CblasRowMajor, CblasUpper, CblasNoTrans, n, k, 1.0f,
               a, k, 0.0f, scratch_c, n);
    }

    return (apply_vectorization_seconds() - start) / (double)iters;
}

int main(int argc, char **argv) {
    const int n = apply_vectorization_arg_int(
        argc, argv, "-n", APPLY_VECTORIZATION_SSYRK_N
    );
    const int k = apply_vectorization_arg_int(
        argc, argv, "-k", APPLY_VECTORIZATION_SSYRK_K
    );
    const int iters = apply_vectorization_arg_int(
        argc, argv, "-iters", APPLY_VECTORIZATION_SSYRK_ITERS
    );
    const size_t a_count = (size_t)n * (size_t)k;
    const size_t c_count = (size_t)n * (size_t)n;
    const int upper_count = (n * (n + 1)) / 2;
    const float value_tolerance =
        APPLY_VECTORIZATION_SSYRK_TOLERANCE * (1.0f + (float)k);
    const double checksum_tolerance =
        APPLY_VECTORIZATION_SSYRK_CHECKSUM_TOLERANCE * (double)upper_count;
    float *a = (float *)malloc(sizeof(float) * a_count);
    float *seed_c = (float *)malloc(sizeof(float) * c_count);
    float *baseline_c = (float *)malloc(sizeof(float) * c_count);
    float *autovec_c = (float *)malloc(sizeof(float) * c_count);
    float *optimized_c = (float *)malloc(sizeof(float) * c_count);
    double baseline_seconds;
    double autovec_seconds;
    double optimized_seconds;
    double baseline_sum;
    double autovec_sum;
    double optimized_sum;
    double autovec_gap;
    double optimized_gap;
    float autovec_diff;
    float optimized_diff;

    if (a == NULL || seed_c == NULL || baseline_c == NULL
        || autovec_c == NULL || optimized_c == NULL) {
        free(a);
        free(seed_c);
        free(baseline_c);
        free(autovec_c);
        free(optimized_c);
        return apply_vectorization_report_alloc_failure("cblas_ssyrk");
    }

    apply_vectorization_fill_matrix(a, n, k, k, 0.015625f);
    apply_vectorization_fill_symmetric(seed_c, n, n, 0.03125f);

    baseline_seconds = run_kernel(
        APPLY_VECTORIZATION_BASELINE_FUNCTION, n, k, iters, a, seed_c, baseline_c
    );
    autovec_seconds = run_kernel(
        APPLY_VECTORIZATION_AUTOVEC_FUNCTION, n, k, iters, a, seed_c, autovec_c
    );
    optimized_seconds = run_kernel(
        APPLY_VECTORIZATION_OPTIMIZED_FUNCTION, n, k, iters, a, seed_c, optimized_c
    );

    apply_vectorization_copy_f32(baseline_c, seed_c, c_count);
    apply_vectorization_copy_f32(autovec_c, seed_c, c_count);
    apply_vectorization_copy_f32(optimized_c, seed_c, c_count);
    APPLY_VECTORIZATION_BASELINE_FUNCTION(
        CblasRowMajor, CblasUpper, CblasNoTrans, n, k, 1.0f, a, k, 0.0f, baseline_c, n
    );
    APPLY_VECTORIZATION_AUTOVEC_FUNCTION(
        CblasRowMajor, CblasUpper, CblasNoTrans, n, k, 1.0f, a, k, 0.0f, autovec_c, n
    );
    APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
        CblasRowMajor, CblasUpper, CblasNoTrans, n, k, 1.0f, a, k, 0.0f, optimized_c, n
    );

    baseline_sum = apply_vectorization_checksum_upper_f32(baseline_c, n, n);
    autovec_sum = apply_vectorization_checksum_upper_f32(autovec_c, n, n);
    optimized_sum = apply_vectorization_checksum_upper_f32(optimized_c, n, n);
    autovec_gap = apply_vectorization_absd(baseline_sum - autovec_sum);
    optimized_gap = apply_vectorization_absd(baseline_sum - optimized_sum);
    autovec_diff = apply_vectorization_max_abs_diff_upper_f32(baseline_c, autovec_c, n, n);
    optimized_diff = apply_vectorization_max_abs_diff_upper_f32(
        baseline_c, optimized_c, n, n
    );

    printf("标量校验和=%.6f\n", baseline_sum);
    printf("自动向量化校验和=%.6f\n", autovec_sum);
    printf("优化后校验和=%.6f\n", optimized_sum);
    printf("自动向量化最大绝对误差=%.8f\n", autovec_diff);
    printf("优化后最大绝对误差=%.8f\n", optimized_diff);

    if (autovec_diff > value_tolerance
        || optimized_diff > value_tolerance
        || autovec_gap > checksum_tolerance
        || optimized_gap > checksum_tolerance) {
        apply_vectorization_print_summary_three(
            baseline_seconds,
            autovec_seconds,
            optimized_seconds,
            "失败",
            "checksum mismatch"
        );
        free(a);
        free(seed_c);
        free(baseline_c);
        free(autovec_c);
        free(optimized_c);
        return 2;
    }

    apply_vectorization_print_summary_three(
        baseline_seconds,
        autovec_seconds,
        optimized_seconds,
        "通过",
        "checksum ok"
    );

    free(a);
    free(seed_c);
    free(baseline_c);
    free(autovec_c);
    free(optimized_c);
    return 0;
}
