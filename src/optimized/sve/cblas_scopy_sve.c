#include <arm_sve.h>
#include <stdint.h>
#include "../../common/blas_case_common.h"

void cblas_scopy(int n, const float *x, int incx, float *y, int incy) {
    if (n <= 0) {
        return;
    }

    if (incx == 1 && incy == 1) {
        int index = 0;
        int vl = (int)svcntw();

        for (; index < n; index += vl) {
            svbool_t pg = svwhilelt_b32((uint64_t)index, (uint64_t)n);
            svfloat32_t values = svld1_f32(pg, x + index);
            svst1_f32(pg, y + index, values);
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
