#include "../common/blas_case_common.h"

#ifndef APPLY_VECTORIZATION_BASELINE_FUNCTION
#define APPLY_VECTORIZATION_BASELINE_FUNCTION cblas_sgemm_baseline
#endif

#ifndef APPLY_VECTORIZATION_OPTIMIZED_FUNCTION
#define APPLY_VECTORIZATION_OPTIMIZED_FUNCTION cblas_sgemm_optimized
#endif

#ifndef APPLY_VECTORIZATION_SGEMM_M
#define APPLY_VECTORIZATION_SGEMM_M 48
#endif

#ifndef APPLY_VECTORIZATION_SGEMM_N
#define APPLY_VECTORIZATION_SGEMM_N 64
#endif

#ifndef APPLY_VECTORIZATION_SGEMM_K
#define APPLY_VECTORIZATION_SGEMM_K 64
#endif

#ifndef APPLY_VECTORIZATION_SGEMM_ITERS
#define APPLY_VECTORIZATION_SGEMM_ITERS 256
#endif

#ifndef APPLY_VECTORIZATION_SGEMM_TOLERANCE
#define APPLY_VECTORIZATION_SGEMM_TOLERANCE 0.0001f
#endif

#ifndef APPLY_VECTORIZATION_SGEMM_CHECKSUM_TOLERANCE
#define APPLY_VECTORIZATION_SGEMM_CHECKSUM_TOLERANCE 0.001
#endif

void APPLY_VECTORIZATION_BASELINE_FUNCTION(
    CBLAS_ORDER order,
    CBLAS_TRANSPOSE transa,
    CBLAS_TRANSPOSE transb,
    int m,
    int n,
    int k,
    float alpha,
    const float *a,
    int lda,
    const float *b,
    int ldb,
    float beta,
    float *c,
    int ldc
);
void APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
    CBLAS_ORDER order,
    CBLAS_TRANSPOSE transa,
    CBLAS_TRANSPOSE transb,
    int m,
    int n,
    int k,
    float alpha,
    const float *a,
    int lda,
    const float *b,
    int ldb,
    float beta,
    float *c,
    int ldc
);

static double run_kernel(
    void (*kernel)(
        CBLAS_ORDER,
        CBLAS_TRANSPOSE,
        CBLAS_TRANSPOSE,
        int,
        int,
        int,
        float,
        const float *,
        int,
        const float *,
        int,
        float,
        float *,
        int
    ),
    const float *a,
    const float *b,
    const float *seed_c,
    float *scratch_c
) {
    double start = apply_vectorization_seconds();

    for (int iter = 0; iter < APPLY_VECTORIZATION_SGEMM_ITERS; ++iter) {
        apply_vectorization_copy_f32(
            scratch_c,
            seed_c,
            (size_t)APPLY_VECTORIZATION_SGEMM_M * (size_t)APPLY_VECTORIZATION_SGEMM_N
        );
        kernel(
            CblasRowMajor,
            CblasNoTrans,
            CblasNoTrans,
            APPLY_VECTORIZATION_SGEMM_M,
            APPLY_VECTORIZATION_SGEMM_N,
            APPLY_VECTORIZATION_SGEMM_K,
            1.0f,
            a,
            APPLY_VECTORIZATION_SGEMM_K,
            b,
            APPLY_VECTORIZATION_SGEMM_N,
            0.0f,
            scratch_c,
            APPLY_VECTORIZATION_SGEMM_N
        );
    }

    return (apply_vectorization_seconds() - start)
        / (double)APPLY_VECTORIZATION_SGEMM_ITERS;
}

int main(void) {
    const size_t a_count =
        (size_t)APPLY_VECTORIZATION_SGEMM_M * (size_t)APPLY_VECTORIZATION_SGEMM_K;
    const size_t b_count =
        (size_t)APPLY_VECTORIZATION_SGEMM_K * (size_t)APPLY_VECTORIZATION_SGEMM_N;
    const size_t c_count =
        (size_t)APPLY_VECTORIZATION_SGEMM_M * (size_t)APPLY_VECTORIZATION_SGEMM_N;
    float *a = (float *)malloc(sizeof(float) * a_count);
    float *b = (float *)malloc(sizeof(float) * b_count);
    float *seed_c = (float *)malloc(sizeof(float) * c_count);
    float *baseline_c = (float *)malloc(sizeof(float) * c_count);
    float *optimized_c = (float *)malloc(sizeof(float) * c_count);
    double baseline_seconds;
    double optimized_seconds;
    double baseline_sum;
    double optimized_sum;
    double checksum_gap;
    float diff;

    if (a == NULL || b == NULL || seed_c == NULL || baseline_c == NULL || optimized_c == NULL) {
        free(a);
        free(b);
        free(seed_c);
        free(baseline_c);
        free(optimized_c);
        return apply_vectorization_report_alloc_failure("cblas_sgemm");
    }

    apply_vectorization_fill_matrix(
        a,
        APPLY_VECTORIZATION_SGEMM_M,
        APPLY_VECTORIZATION_SGEMM_K,
        APPLY_VECTORIZATION_SGEMM_K,
        0.015625f
    );
    apply_vectorization_fill_matrix(
        b,
        APPLY_VECTORIZATION_SGEMM_K,
        APPLY_VECTORIZATION_SGEMM_N,
        APPLY_VECTORIZATION_SGEMM_N,
        0.03125f
    );
    memset(seed_c, 0, sizeof(float) * c_count);

    baseline_seconds = run_kernel(
        APPLY_VECTORIZATION_BASELINE_FUNCTION,
        a,
        b,
        seed_c,
        baseline_c
    );
    optimized_seconds = run_kernel(
        APPLY_VECTORIZATION_OPTIMIZED_FUNCTION,
        a,
        b,
        seed_c,
        optimized_c
    );

    memset(baseline_c, 0, sizeof(float) * c_count);
    memset(optimized_c, 0, sizeof(float) * c_count);
    APPLY_VECTORIZATION_BASELINE_FUNCTION(
        CblasRowMajor,
        CblasNoTrans,
        CblasNoTrans,
        APPLY_VECTORIZATION_SGEMM_M,
        APPLY_VECTORIZATION_SGEMM_N,
        APPLY_VECTORIZATION_SGEMM_K,
        1.0f,
        a,
        APPLY_VECTORIZATION_SGEMM_K,
        b,
        APPLY_VECTORIZATION_SGEMM_N,
        0.0f,
        baseline_c,
        APPLY_VECTORIZATION_SGEMM_N
    );
    APPLY_VECTORIZATION_OPTIMIZED_FUNCTION(
        CblasRowMajor,
        CblasNoTrans,
        CblasNoTrans,
        APPLY_VECTORIZATION_SGEMM_M,
        APPLY_VECTORIZATION_SGEMM_N,
        APPLY_VECTORIZATION_SGEMM_K,
        1.0f,
        a,
        APPLY_VECTORIZATION_SGEMM_K,
        b,
        APPLY_VECTORIZATION_SGEMM_N,
        0.0f,
        optimized_c,
        APPLY_VECTORIZATION_SGEMM_N
    );

    baseline_sum = apply_vectorization_checksum_f32(baseline_c, (int)c_count);
    optimized_sum = apply_vectorization_checksum_f32(optimized_c, (int)c_count);
    checksum_gap = apply_vectorization_absd(baseline_sum - optimized_sum);
    diff = apply_vectorization_max_abs_diff_f32(baseline_c, optimized_c, (int)c_count);

    printf("标量校验和=%.6f\n", baseline_sum);
    printf("优化后校验和=%.6f\n", optimized_sum);
    printf("最大绝对误差=%.8f\n", diff);

    if (diff > APPLY_VECTORIZATION_SGEMM_TOLERANCE
        || checksum_gap > APPLY_VECTORIZATION_SGEMM_CHECKSUM_TOLERANCE) {
        apply_vectorization_print_summary(
            baseline_seconds,
            optimized_seconds,
            "失败",
            "checksum mismatch"
        );
        free(a);
        free(b);
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
    free(b);
    free(seed_c);
    free(baseline_c);
    free(optimized_c);
    return 0;
}
