#include "../common/blas_case_common.h"

void cblas_sger(
    CBLAS_ORDER order,
    int m,
    int n,
    float alpha,
    const float *x,
    int incx,
    const float *y,
    int incy,
    float *a,
    int lda
) {
    if (order != CblasRowMajor || incx != 1 || incy != 1) {
        return;
    }

    for (int row = 0; row < m; ++row) {
        float scaled_x = alpha * x[row];

        for (int col = 0; col < n; ++col) {
            a[row * lda + col] += scaled_x * y[col];
        }
    }
}
