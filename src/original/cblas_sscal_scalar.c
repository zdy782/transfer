#include "../common/blas_case_common.h"

void cblas_sscal(int n, float alpha, float *x, int incx) {
    if (n <= 0) {
        return;
    }

    if (incx == 1) {
        for (int index = 0; index < n; ++index) {
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
