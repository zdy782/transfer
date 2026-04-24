#include "../common/blas_case_common.h"

void cblas_sgemm(
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
) {
    if (order != CblasRowMajor || transa != CblasNoTrans || transb != CblasNoTrans) {
        return;
    }

    for (int row = 0; row < m; ++row) {
        for (int col = 0; col < n; ++col) {
            c[row * ldc + col] *= beta;
        }

        for (int depth = 0; depth < k; ++depth) {
            float scaled_a = alpha * a[row * lda + depth];

            for (int col = 0; col < n; ++col) {
                c[row * ldc + col] += scaled_a * b[depth * ldb + col];
            }
        }
    }
}
