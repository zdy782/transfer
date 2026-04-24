#include <arm_neon.h>
#include "../../common/blas_case_common.h"

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
        int col = 0;
        int limit = n & ~3;
        float partial[4];
        float accum = 0.0f;
        const float *a_row = a + ((size_t)row * (size_t)lda);
        float32x4_t accum_vec = vdupq_n_f32(0.0f);

        for (; col < limit; col += 4) {
            float32x4_t a_vec = vld1q_f32(a_row + col);
            float32x4_t x_vec = vld1q_f32(x + col);
            accum_vec = vfmaq_f32(accum_vec, a_vec, x_vec);
        }
        vst1q_f32(partial, accum_vec);
        accum = partial[0] + partial[1] + partial[2] + partial[3];
        for (; col < n; ++col) {
            accum += a_row[col] * x[col];
        }

        y[row] = (alpha * accum) + (beta * y[row]);
    }
}
