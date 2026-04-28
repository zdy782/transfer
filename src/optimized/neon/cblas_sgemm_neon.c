#include <arm_neon.h>
#include "../../common/blas_case_common.h"

static inline float32x4_t cblas_sgemm_neon_load_c4(const float *c_row, int col, float beta) {
    if (beta == 0.0f) {
        return vdupq_n_f32(0.0f);
    }
    {
        float32x4_t c_vec = vld1q_f32(c_row + col);
        if (beta != 1.0f) {
            c_vec = vmulq_f32(c_vec, vdupq_n_f32(beta));
        }
        return c_vec;
    }
}

static void cblas_sgemm_neon_row_tail(
    int n,
    int k,
    float alpha,
    const float *a_row,
    const float *b,
    int ldb,
    float beta,
    float *c_row
) {
    int col = 0;
    int limit = n & ~3;

    for (; col < limit; col += 4) {
        float32x4_t c_vec = cblas_sgemm_neon_load_c4(c_row, col, beta);

        for (int depth = 0; depth < k; ++depth) {
            float32x4_t a_vec = vdupq_n_f32(alpha * a_row[depth]);
            float32x4_t b_vec = vld1q_f32(b + ((size_t)depth * (size_t)ldb) + col);
            c_vec = vfmaq_f32(c_vec, a_vec, b_vec);
        }
        vst1q_f32(c_row + col, c_vec);
    }

    for (; col < n; ++col) {
        float accum = beta == 0.0f ? 0.0f : c_row[col] * beta;

        for (int depth = 0; depth < k; ++depth) {
            accum += (alpha * a_row[depth]) * b[(size_t)depth * (size_t)ldb + col];
        }
        c_row[col] = accum;
    }
}

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

    int row = 0;
    for (; row + 4 <= m; row += 4) {
        const float *a0 = a + ((size_t)(row + 0) * (size_t)lda);
        const float *a1 = a + ((size_t)(row + 1) * (size_t)lda);
        const float *a2 = a + ((size_t)(row + 2) * (size_t)lda);
        const float *a3 = a + ((size_t)(row + 3) * (size_t)lda);
        float *c0 = c + ((size_t)(row + 0) * (size_t)ldc);
        float *c1 = c + ((size_t)(row + 1) * (size_t)ldc);
        float *c2 = c + ((size_t)(row + 2) * (size_t)ldc);
        float *c3 = c + ((size_t)(row + 3) * (size_t)ldc);
        int col = 0;
        int limit = n & ~3;

        for (; col < limit; col += 4) {
            float32x4_t c0_vec = cblas_sgemm_neon_load_c4(c0, col, beta);
            float32x4_t c1_vec = cblas_sgemm_neon_load_c4(c1, col, beta);
            float32x4_t c2_vec = cblas_sgemm_neon_load_c4(c2, col, beta);
            float32x4_t c3_vec = cblas_sgemm_neon_load_c4(c3, col, beta);

            for (int depth = 0; depth < k; ++depth) {
                float32x4_t b_vec = vld1q_f32(b + ((size_t)depth * (size_t)ldb) + col);
                c0_vec = vfmaq_f32(c0_vec, vdupq_n_f32(alpha * a0[depth]), b_vec);
                c1_vec = vfmaq_f32(c1_vec, vdupq_n_f32(alpha * a1[depth]), b_vec);
                c2_vec = vfmaq_f32(c2_vec, vdupq_n_f32(alpha * a2[depth]), b_vec);
                c3_vec = vfmaq_f32(c3_vec, vdupq_n_f32(alpha * a3[depth]), b_vec);
            }

            vst1q_f32(c0 + col, c0_vec);
            vst1q_f32(c1 + col, c1_vec);
            vst1q_f32(c2 + col, c2_vec);
            vst1q_f32(c3 + col, c3_vec);
        }

        for (; col < n; ++col) {
            float acc0 = beta == 0.0f ? 0.0f : c0[col] * beta;
            float acc1 = beta == 0.0f ? 0.0f : c1[col] * beta;
            float acc2 = beta == 0.0f ? 0.0f : c2[col] * beta;
            float acc3 = beta == 0.0f ? 0.0f : c3[col] * beta;

            for (int depth = 0; depth < k; ++depth) {
                float b_value = b[(size_t)depth * (size_t)ldb + col];
                acc0 += (alpha * a0[depth]) * b_value;
                acc1 += (alpha * a1[depth]) * b_value;
                acc2 += (alpha * a2[depth]) * b_value;
                acc3 += (alpha * a3[depth]) * b_value;
            }
            c0[col] = acc0;
            c1[col] = acc1;
            c2[col] = acc2;
            c3[col] = acc3;
        }
    }

    for (; row < m; ++row) {
        const float *a_row = a + ((size_t)row * (size_t)lda);
        float *c_row = c + ((size_t)row * (size_t)ldc);
        cblas_sgemm_neon_row_tail(n, k, alpha, a_row, b, ldb, beta, c_row);
    }
}
