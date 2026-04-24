#include "../common/blas_case_common.h"

float cblas_sdot(int n, const float *x, int incx, const float *y, int incy) {
    float accum = 0.0f;

    if (n <= 0) {
        return 0.0f;
    }

    if (incx == 1 && incy == 1) {
        for (int index = 0; index < n; ++index) {
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
