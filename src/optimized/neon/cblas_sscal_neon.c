#include <arm_neon.h>
#include "../../common/blas_case_common.h"

void cblas_sscal(int n, float alpha, float *x, int incx) {
    if (n <= 0) {
        return;
    }

    if (incx == 1) {
        int index = 0;
        int limit = n & ~3;
        float32x4_t zero_vec = vdupq_n_f32(0.0f);
        float32x4_t alpha_vec = vdupq_n_f32(alpha);

        for (; index < limit; index += 4) {
            float32x4_t x_vec = vld1q_f32(x + index);
            float32x4_t scaled_vec = vfmaq_f32(zero_vec, x_vec, alpha_vec);
            vst1q_f32(x + index, scaled_vec);
        }
        for (; index < n; ++index) {
            x[index] *= alpha;
        }
        return;
    }

    {
        int ix = 0;
        for (int index = 0; index < n; ++index) {
            x[ix] *= alpha;
            ix += incx;
        }
    }
}
