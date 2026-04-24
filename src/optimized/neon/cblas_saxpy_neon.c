#include <arm_neon.h>
#include "../../common/blas_case_common.h"

void cblas_saxpy(int n, float alpha, const float *x, int incx, float *y, int incy) {
    if (n <= 0) {
        return;
    }

    if (incx == 1 && incy == 1) {
        int index = 0;
        int limit = n & ~3;
        float32x4_t alpha_vec = vdupq_n_f32(alpha);

        for (; index < limit; index += 4) {
            float32x4_t x_vec = vld1q_f32(x + index);
            float32x4_t y_vec = vld1q_f32(y + index);
            y_vec = vfmaq_f32(y_vec, alpha_vec, x_vec);
            vst1q_f32(y + index, y_vec);
        }
        for (; index < n; ++index) {
            y[index] += alpha * x[index];
        }
        return;
    }

    {
        int ix = 0;
        int iy = 0;
        for (int index = 0; index < n; ++index) {
            y[iy] += alpha * x[ix];
            ix += incx;
            iy += incy;
        }
    }
}
