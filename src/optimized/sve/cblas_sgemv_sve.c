#include <arm_sve.h>
#include <stdint.h>
#include "../../common/blas_case_common.h"

void cblas_sgemv(
    CBLAS_ORDER order,
    CBLAS_TRANSPOSE trans,
    int m,
    int n,
    float alpha,
    const float *a,
    int lda,
    const float *x,
    int incx,
    float beta,
    float *y,
    int incy
) {
    if (order != CblasRowMajor || trans != CblasNoTrans || incx != 1 || incy != 1) {
        return;
    }

    for (int row = 0; row < m; ++row) {
        int col = 0;
        int vl = (int)svcntw();
        const float *a_row = a + ((size_t)row * (size_t)lda);
        svfloat32_t accum_vec = svdup_n_f32(0.0f);

        for (; col < n; col += vl) {
            svbool_t pg = svwhilelt_b32((uint64_t)col, (uint64_t)n);
            svfloat32_t a_vec = svld1_f32(pg, a_row + col);
            svfloat32_t x_vec = svld1_f32(pg, x + col);
            accum_vec = svmla_f32_m(pg, accum_vec, a_vec, x_vec);
        }

        y[row] = (alpha * svaddv_f32(svptrue_b32(), accum_vec)) + (beta * y[row]);
    }
}
