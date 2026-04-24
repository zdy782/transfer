#include <arm_sve.h>
#include <stdint.h>
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
        int vl = (int)svcntw();
        int col = 0;

        if (beta == 0.0f) {
            svfloat32_t zero_vec = svdup_n_f32(0.0f);
            for (; col < n; col += vl) {
                svbool_t pg = svwhilelt_b32((uint64_t)col, (uint64_t)n);
                svst1_f32(pg, c_row + col, zero_vec);
            }
        } else if (beta != 1.0f) {
            svfloat32_t beta_vec = svdup_n_f32(beta);
            for (; col < n; col += vl) {
                svbool_t pg = svwhilelt_b32((uint64_t)col, (uint64_t)n);
                svfloat32_t c_vec = svld1_f32(pg, c_row + col);
                c_vec = svmul_f32_m(pg, c_vec, beta_vec);
                svst1_f32(pg, c_row + col, c_vec);
            }
        }

        for (int depth = 0; depth < k; ++depth) {
            float scaled_a = alpha * a[(size_t)row * (size_t)lda + depth];
            svfloat32_t scaled_a_vec = svdup_n_f32(scaled_a);
            const float *b_row = b + ((size_t)depth * (size_t)ldb);

            col = 0;
            for (; col < n; col += vl) {
                svbool_t pg = svwhilelt_b32((uint64_t)col, (uint64_t)n);
                svfloat32_t c_vec = svld1_f32(pg, c_row + col);
                svfloat32_t b_vec = svld1_f32(pg, b_row + col);
                c_vec = svmla_f32_m(pg, c_vec, scaled_a_vec, b_vec);
                svst1_f32(pg, c_row + col, c_vec);
            }
        }
    }
}
