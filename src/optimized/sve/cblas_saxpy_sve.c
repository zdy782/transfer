#include <arm_sve.h>
#include <stdint.h>
#include "../../common/blas_case_common.h"

void cblas_saxpy(int n, float alpha, const float *x, int incx, float *y, int incy) {
    if (n <= 0) {
        return;
    }

    if (incx == 1 && incy == 1) {
        int index = 0;
        int vl = (int)svcntw();
        svfloat32_t alpha_vec = svdup_n_f32(alpha);

        for (; index < n; index += vl) {
            svbool_t pg = svwhilelt_b32((uint64_t)index, (uint64_t)n);
            svfloat32_t x_vec = svld1_f32(pg, x + index);
            svfloat32_t y_vec = svld1_f32(pg, y + index);
            y_vec = svmla_f32_m(pg, y_vec, alpha_vec, x_vec);
            svst1_f32(pg, y + index, y_vec);
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
