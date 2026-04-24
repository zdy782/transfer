#include <arm_sve.h>
#include <stdint.h>
#include "../../common/blas_case_common.h"

void cblas_sscal(int n, float alpha, float *x, int incx) {
    if (n <= 0) {
        return;
    }

    if (incx == 1) {
        int index = 0;
        int vl = (int)svcntw();
        svfloat32_t alpha_vec = svdup_n_f32(alpha);

        for (; index < n; index += vl) {
            svbool_t pg = svwhilelt_b32((uint64_t)index, (uint64_t)n);
            svfloat32_t x_vec = svld1_f32(pg, x + index);
            x_vec = svmul_f32_m(pg, x_vec, alpha_vec);
            svst1_f32(pg, x + index, x_vec);
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
