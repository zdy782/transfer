#include <arm_neon.h>
#include "../../common/blas_case_common.h"

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
        int col = 0;
        int limit = n & ~3;
        float scaled_x = alpha * x[row];
        float *a_row = a + ((size_t)row * (size_t)lda);
        float32x4_t scaled_x_vec = vdupq_n_f32(scaled_x);

        for (; col < limit; col += 4) {
            float32x4_t a_vec = vld1q_f32(a_row + col);
            float32x4_t y_vec = vld1q_f32(y + col);
            a_vec = vfmaq_f32(a_vec, scaled_x_vec, y_vec);
            vst1q_f32(a_row + col, a_vec);
        }
        for (; col < n; ++col) {
            a_row[col] += scaled_x * y[col];
        }
    }
}
