#include "../common/blas_case_common.h"

#ifndef APPLY_VECTORIZATION_BASELINE_FUNCTION
#define APPLY_VECTORIZATION_BASELINE_FUNCTION cblas_ssyrk_baseline
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
    const float *a,
    const float *seed_c,
    float *scratch_c
) {
    double start = apply_vectorization_seconds();

    for (int iter = 0; iter < APPLY_VECTORIZATION_SSYRK_ITERS; ++iter) {
        apply_vectorization_copy_f32(
            scratch_c,
            seed_c,
            (size_t)APPLY_VECTORIZATION_SSYRK_N * (size_t)APPLY_VECTORIZATION_SSYRK_N
        );
        kernel(
            CblasRowMajor,
            CblasUpper,
            CblasNoTrans,
            APPLY_VECTORIZATION_SSYRK_N,
            APPLY_VECTORIZATION_SSYRK_K,
            1.0f,
            a,
            APPLY_VECTORIZATION_SSYRK_K,
            0.0f,
            scratch_c,
            APPLY_VECTORIZATION_SSYRK_N
        );
    }

    return apply_vectorization_seconds() - start;
}

int main(void) {
    const size_t a_count =
        (size_t)APPLY_VECTORIZATION_SSYRK_N * (size_t)APPLY_VECTORIZATION_SSYRK_K;
    const size_t c_count =
        (size_t)APPLY_VECTORIZATION_SSYRK_N * (size_t)APPLY_VECTORIZATION_SSYRK_N;
    float *a = (float *)malloc(sizeof(float) * a_count);
    float *seed_c = (float *)malloc(sizeof(float) * c_count);
    float *baseline_c = (float *)malloc(sizeof(float) * c_count);
    float *optimized_c = (float *)malloc(sizeof(float) * c_count);
    double baseline_seconds;
    double optimized_seconds;
    double baseline_sum;
    double optimized_sum;
    double checksum_gap;
    float diff;

    if (a == NULL || seed_c == NULL || baseline_c == NULL || optimized_c == NULL) {
        free(a);
        free(seed_c);
        free(baseline_c);
        free(optimized_c);
        return apply_vectorization_report_alloc_failure("cblas_ssyrk");
    }

    apply_vectorization_fill_matrix(
        a,
        APPLY_VECTORIZATION_SSYRK_N,
        APPLY_VECTORIZATION_SSYRK_K,
        APPLY_VECTORIZATION_SSYRK_K,
        0.015625f
    );
    apply_vectorization_fill_symmetric(
        seed_c,
        APPLY_VECTORIZATION_SSYRK_N,
        APPLY_VECTORIZATION_SSYRK_N,
        0.03125f
    );

    baseline_seconds = run_kernel(
        APPLY_VECTORIZATION_BASELINE_FUNCTION,
        a,
        seed_c,
        baseline_c
    );
    optimized_seconds = run_kernel(
        APPLY_VECTORIZATION_OPTIMIZED_FUNCTION,
        a,
        seed_c,
        optimized_c
    );

    apply_vectorization_copy_f32(baseline_c, seed_c, c_count);
    apply_vectorization_copy_f32(optimized_c, seed_c, c_count);
    APPLY_VECTORIZATION_BASELINE_FUNCTION(
        CblasRowMajor,
        CblasUpper,
        CblasNoTrans,
        APPLY_VECTORIZATION_SSYRK_N,
        APPLY_VECTORIZATION_SSYRK_K,
        1.0f,
        a,
        APPLY_VECTORIZATION_SSYRK_K,
        0.0f,
        baseline_c,
        APPLY_VECTORIZATION_SSYRK_N
    );
    APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
        CblasRowMajor,
        CblasUpper,
        CblasNoTrans,
        APPLY_VECTORIZATION_SSYRK_N,
        APPLY_VECTORIZATION_SSYRK_K,
        1.0f,
        a,
        APPLY_VECTORIZATION_SSYRK_K,
        0.0f,
        optimized_c,
        APPLY_VECTORIZATION_SSYRK_N
    );

    baseline_sum = apply_vectorization_checksum_upper_f32(
        baseline_c,
        APPLY_VECTORIZATION_SSYRK_N,
        APPLY_VECTORIZATION_SSYRK_N
    );
    optimized_sum = apply_vectorization_checksum_upper_f32(
        optimized_c,
        APPLY_VECTORIZATION_SSYRK_N,
        APPLY_VECTORIZATION_SSYRK_N
    );
    checksum_gap = apply_vectorization_absd(baseline_sum - optimized_sum);
    diff = apply_vectorization_max_abs_diff_upper_f32(
        baseline_c,
        optimized_c,
        APPLY_VECTORIZATION_SSYRK_N,
        APPLY_VECTORIZATION_SSYRK_N
    );

    printf("标量校验和=%.6f\n", baseline_sum);
    printf("优化后校验和=%.6f\n", optimized_sum);
    printf("最大绝对误差=%.8f\n", diff);

    if (diff > APPLY_VECTORIZATION_SSYRK_TOLERANCE
        || checksum_gap > APPLY_VECTORIZATION_SSYRK_CHECKSUM_TOLERANCE) {
        apply_vectorization_print_summary(
            baseline_seconds,
            optimized_seconds,
            "失败",
            "checksum mismatch"
        );
        free(a);
        free(seed_c);
        free(baseline_c);
        free(optimized_c);
        return 2;
    }

    apply_vectorization_print_summary(
        baseline_seconds,
        optimized_seconds,
        "通过",
        "checksum ok"
    );

    free(a);
    free(seed_c);
    free(baseline_c);
    free(optimized_c);
    return 0;
}
