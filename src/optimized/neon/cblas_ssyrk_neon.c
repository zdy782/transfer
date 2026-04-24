#include <arm_neon.h>
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
            int limit = k & ~3;
            float partial[4];
            float accum = 0.0f;
            const float *a_col = a + ((size_t)col * (size_t)lda);
            float32x4_t accum_vec = vdupq_n_f32(0.0f);

            for (; depth < limit; depth += 4) {
                float32x4_t row_vec = vld1q_f32(a_row + depth);
                float32x4_t col_vec = vld1q_f32(a_col + depth);
                accum_vec = vfmaq_f32(accum_vec, row_vec, col_vec);
            }
            vst1q_f32(partial, accum_vec);
            accum = partial[0] + partial[1] + partial[2] + partial[3];
            for (; depth < k; ++depth) {
                accum += a_row[depth] * a_col[depth];
            }

            c[(size_t)row * (size_t)ldc + col] =
                (alpha * accum) + (beta * c[(size_t)row * (size_t)ldc + col]);
        }
    }
}
