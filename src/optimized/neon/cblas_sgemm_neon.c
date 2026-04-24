#include <arm_neon.h>
#include "../../common/blas_case_common.h"

void cblas_sgemm(
    CBLAS_ORDER order,
    CBLAS_TRANSPOSE transa,
    CBLAS_TRANSPOSE transb,
    int m,
    int n,
    int k,
    float alpha,
    const float *a,
    int lda,
    const float *b,
    int ldb,
    float beta,
    float *c,
    int ldc
) {
    if (order != CblasRowMajor || transa != CblasNoTrans || transb != CblasNoTrans) {
        return;
    }

    for (int row = 0; row < m; ++row) {
        float *c_row = c + ((size_t)row * (size_t)ldc);
        int col = 0;
        int limit = n & ~3;

        if (beta == 0.0f) {
            float32x4_t zero_vec = vdupq_n_f32(0.0f);
            for (; col < limit; col += 4) {
                vst1q_f32(c_row + col, zero_vec);
            }
            for (; col < n; ++col) {
                c_row[col] = 0.0f;
            }
        } else if (beta != 1.0f) {
            float32x4_t zero_vec = vdupq_n_f32(0.0f);
            float32x4_t beta_vec = vdupq_n_f32(beta);
            for (; col < limit; col += 4) {
                float32x4_t c_vec = vld1q_f32(c_row + col);
                c_vec = vfmaq_f32(zero_vec, c_vec, beta_vec);
                vst1q_f32(c_row + col, c_vec);
            }
            for (; col < n; ++col) {
                c_row[col] *= beta;
            }
        }

        for (int depth = 0; depth < k; ++depth) {
            float scaled_a = alpha * a[(size_t)row * (size_t)lda + depth];
            float32x4_t scaled_a_vec = vdupq_n_f32(scaled_a);
            const float *b_row = b + ((size_t)depth * (size_t)ldb);

            col = 0;
            for (; col < limit; col += 4) {
                float32x4_t c_vec = vld1q_f32(c_row + col);
                float32x4_t b_vec = vld1q_f32(b_row + col);
                c_vec = vfmaq_f32(c_vec, scaled_a_vec, b_vec);
                vst1q_f32(c_row + col, c_vec);
            }
            for (; col < n; ++col) {
                c_row[col] += scaled_a * b_row[col];
            }
        }
    }
}
