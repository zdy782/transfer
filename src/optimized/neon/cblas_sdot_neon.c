#include <arm_neon.h>
#include "../../common/blas_case_common.h"

float cblas_sdot(int n, const float *x, int incx, const float *y, int incy) {
    float accum = 0.0f;

    if (n <= 0) {
        return 0.0f;
    }

    if (incx == 1 && incy == 1) {
        int index = 0;
        int limit = n & ~3;
        float partial[4];
        float32x4_t accum_vec = vdupq_n_f32(0.0f);

        for (; index < limit; index += 4) {
            float32x4_t x_vec = vld1q_f32(x + index);
            float32x4_t y_vec = vld1q_f32(y + index);
            accum_vec = vfmaq_f32(accum_vec, x_vec, y_vec);
        }
        vst1q_f32(partial, accum_vec);
        accum = partial[0] + partial[1] + partial[2] + partial[3];
        for (; index < n; ++index) {
            accum += x[index] * y[index];
        }
        return accum;
    }

    {
        int ix = 0;
        int iy = 0;
        for (int index = 0; index < n; ++index) {
            accum += x[ix] * y[iy];
            ix += incx;
            iy += incy;
        }
    }
    return accum;
}
