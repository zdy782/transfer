#include "../common/blas_case_common.h"

void cblas_ssyrk(
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
) {
    if (order != CblasRowMajor || uplo != CblasUpper || trans != CblasNoTrans) {
        return;
    }

    for (int row = 0; row < n; ++row) {
        for (int col = row; col < n; ++col) {
            float accum = 0.0f;

            for (int depth = 0; depth < k; ++depth) {
                accum += a[row * lda + depth] * a[col * lda + depth];
            }

            c[row * ldc + col] = (alpha * accum) + (beta * c[row * ldc + col]);
        }
    }
}
