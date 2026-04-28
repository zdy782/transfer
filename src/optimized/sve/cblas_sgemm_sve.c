#include <arm_sve.h>
#include <stdint.h>
#include "../../common/blas_case_common.h"

static inline svfloat32_t cblas_sgemm_sve_load_c(
    svbool_t pg, const float *c_row, int col, float beta
) {
    if (beta == 0.0f) {
        return svdup_n_f32(0.0f);
    }
    {
        svfloat32_t c_vec = svld1_f32(pg, c_row + col);
        if (beta != 1.0f) {
            c_vec = svmul_f32_m(pg, c_vec, svdup_n_f32(beta));
        }
        return c_vec;
    }
}

static void cblas_sgemm_sve_row_tail(
    int n,
    int k,
    float alpha,
    const float *a_row,
    const float *b,
    int ldb,
    float beta,
    float *c_row
) {
    int vl = (int)svcntw();

    for (int col = 0; col < n; col += vl) {
        svbool_t pg = svwhilelt_b32((uint64_t)col, (uint64_t)n);
        svfloat32_t c_vec = cblas_sgemm_sve_load_c(pg, c_row, col, beta);

        for (int depth = 0; depth < k; ++depth) {
            svfloat32_t b_vec = svld1_f32(
                pg,
                b + ((size_t)depth * (size_t)ldb) + col
            );
            c_vec = svmla_f32_m(
                pg,
                c_vec,
                svdup_n_f32(alpha * a_row[depth]),
                b_vec
            );
        }
        svst1_f32(pg, c_row + col, c_vec);
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

    int vl = (int)svcntw();
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

        for (int col = 0; col < n; col += vl) {
            svbool_t pg = svwhilelt_b32((uint64_t)col, (uint64_t)n);
            svfloat32_t c0_vec = cblas_sgemm_sve_load_c(pg, c0, col, beta);
            svfloat32_t c1_vec = cblas_sgemm_sve_load_c(pg, c1, col, beta);
            svfloat32_t c2_vec = cblas_sgemm_sve_load_c(pg, c2, col, beta);
            svfloat32_t c3_vec = cblas_sgemm_sve_load_c(pg, c3, col, beta);

            for (int depth = 0; depth < k; ++depth) {
                svfloat32_t b_vec = svld1_f32(
                    pg,
                    b + ((size_t)depth * (size_t)ldb) + col
                );
                c0_vec = svmla_f32_m(pg, c0_vec, svdup_n_f32(alpha * a0[depth]), b_vec);
                c1_vec = svmla_f32_m(pg, c1_vec, svdup_n_f32(alpha * a1[depth]), b_vec);
                c2_vec = svmla_f32_m(pg, c2_vec, svdup_n_f32(alpha * a2[depth]), b_vec);
                c3_vec = svmla_f32_m(pg, c3_vec, svdup_n_f32(alpha * a3[depth]), b_vec);
            }

            svst1_f32(pg, c0 + col, c0_vec);
            svst1_f32(pg, c1 + col, c1_vec);
            svst1_f32(pg, c2 + col, c2_vec);
            svst1_f32(pg, c3 + col, c3_vec);
        }
    }

    for (; row < m; ++row) {
        const float *a_row = a + ((size_t)row * (size_t)lda);
        float *c_row = c + ((size_t)row * (size_t)ldc);
        cblas_sgemm_sve_row_tail(n, k, alpha, a_row, b, ldb, beta, c_row);
    }
}
