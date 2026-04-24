#include <arm_sve.h>
#include <stdint.h>
#include "../../common/blas_case_common.h"

float cblas_sdot(int n, const float *x, int incx, const float *y, int incy) {
    float accum = 0.0f;

    if (n <= 0) {
        return 0.0f;
    }

    if (incx == 1 && incy == 1) {
        int index = 0;
        int vl = (int)svcntw();
        svfloat32_t accum_vec = svdup_n_f32(0.0f);

        for (; index < n; index += vl) {
            svbool_t pg = svwhilelt_b32((uint64_t)index, (uint64_t)n);
            svfloat32_t x_vec = svld1_f32(pg, x + index);
            svfloat32_t y_vec = svld1_f32(pg, y + index);
            accum_vec = svmla_f32_m(pg, accum_vec, x_vec, y_vec);
        }
        return svaddv_f32(svptrue_b32(), accum_vec);
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
