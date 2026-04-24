#include "../common/blas_case_common.h"

void cblas_scopy(int n, const float *x, int incx, float *y, int incy) {
    if (n <= 0) {
        return;
    }

    if (incx == 1 && incy == 1) {
        for (int index = 0; index < n; ++index) {
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
