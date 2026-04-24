#include <arm_sme.h>
#include <stdint.h>
#include "../../common/blas_case_common.h"

static void cblas_sger_sme_row_update(int n, float scaled_x, const float *y, float *a_row) __arm_streaming;
static void cblas_sger_sme_row_update(int n, float scaled_x, const float *y, float *a_row) __arm_streaming {
    int col = 0;
    int vl = (int)svcntw();
    svfloat32_t scaled_x_vec = svdup_n_f32(scaled_x);

    for (; col < n; col += vl) {
        svbool_t pg = svwhilelt_b32((uint64_t)col, (uint64_t)n);
        svfloat32_t a_vec = svld1_f32(pg, a_row + col);
        svfloat32_t y_vec = svld1_f32(pg, y + col);
        a_vec = svmla_f32_m(pg, a_vec, scaled_x_vec, y_vec);
        svst1_f32(pg, a_row + col, a_vec);
    }
}

void cblas_sger(
    CBLAS_ORDER order,
    int m,
    int n,
    float alpha,
    const float *x,
    int incx,
    const float *y,
    int incy,
    float *a,
    int lda
) {
    if (order != CblasRowMajor || incx != 1 || incy != 1) {
        return;
    }
    for (int row = 0; row < m; ++row) {
        float scaled_x = alpha * x[row];
        float *a_row = a + ((size_t)row * (size_t)lda);
        cblas_sger_sme_row_update(n, scaled_x, y, a_row);
    }
}
