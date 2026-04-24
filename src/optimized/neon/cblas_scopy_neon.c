#include <arm_neon.h>
#include "../../common/blas_case_common.h"

void cblas_scopy(int n, const float *x, int incx, float *y, int incy) {
    if (n <= 0) {
        return;
    }

    if (incx == 1 && incy == 1) {
        int index = 0;
        int limit = n & ~3;

        for (; index < limit; index += 4) {
            float32x4_t values = vld1q_f32(x + index);
            vst1q_f32(y + index, values);
        }
        for (; index < n; ++index) {
            y[index] = x[index];
        }
        return;
    }

    {
        int ix = 0;
        int iy = 0;
        for (int index = 0; index < n; ++index) {
            y[iy] = x[ix];
            ix += incx;
            iy += incy;
        }
    }
}
