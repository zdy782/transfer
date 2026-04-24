#include <arm_sve.h>
#include <stdint.h>
#include "../../common/blas_case_common.h"

void cblas_ssyrk(
    CBLAS_ORDER order,
    CBLAS_UPLO uplo,
    CBLAS_TRANSPOSE trans,
    int n,
    int k,
    float alpha,
    const float *a,
    int lda,
    float beta,
    float *c,
    int ldc
) {
    if (order != CblasRowMajor || uplo != CblasUpper || trans != CblasNoTrans) {
        return;
    }

    for (int row = 0; row < n; ++row) {
        const float *a_row = a + ((size_t)row * (size_t)lda);

        for (int col = row; col < n; ++col) {
            int depth = 0;
            int vl = (int)svcntw();
            const float *a_col = a + ((size_t)col * (size_t)lda);
            svfloat32_t accum_vec = svdup_n_f32(0.0f);

            for (; depth < k; depth += vl) {
                svbool_t pg = svwhilelt_b32((uint64_t)depth, (uint64_t)k);
                svfloat32_t row_vec = svld1_f32(pg, a_row + depth);
                svfloat32_t col_vec = svld1_f32(pg, a_col + depth);
                accum_vec = svmla_f32_m(pg, accum_vec, row_vec, col_vec);
            }

            c[(size_t)row * (size_t)ldc + col] =
                (alpha * svaddv_f32(svptrue_b32(), accum_vec))
                + (beta * c[(size_t)row * (size_t)ldc + col]);
        }
    }
}
