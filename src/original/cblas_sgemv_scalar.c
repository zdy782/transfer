#include "../common/blas_case_common.h"

void cblas_sgemv(
    CBLAS_ORDER order,
    CBLAS_TRANSPOSE trans,
    int m,
    int n,
    float alpha,
    const float *a,
    int lda,
    const float *x,
    int incx,
    float beta,
    float *y,
    int incy
) {
    if (order != CblasRowMajor || trans != CblasNoTrans || incx != 1 || incy != 1) {
        return;
    }

    for (int row = 0; row < m; ++row) {
        float accum = 0.0f;

        for (int col = 0; col < n; ++col) {
            accum += a[row * lda + col] * x[col];
        }

        y[row] = (alpha * accum) + (beta * y[row]);
    }
}
