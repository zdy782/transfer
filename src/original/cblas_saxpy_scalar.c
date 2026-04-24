#include "../common/blas_case_common.h"

void cblas_saxpy(int n, float alpha, const float *x, int incx, float *y, int incy) {
    if (n <= 0) {
        return;
    }

    if (incx == 1 && incy == 1) {
        for (int index = 0; index < n; ++index) {
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
