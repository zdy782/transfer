#include <arm_sme.h>
#include <stdint.h>
#include "../../common/blas_case_common.h"

static void cblas_sgemm_sme_store_tile(
    svbool_t col_pg,
    int row,
    int m,
    int col,
    float beta,
    float *c,
    int ldc
) __arm_streaming __arm_in("za") {
    const int vl = (int)svcntw();

    for (int tile_row = 0; tile_row < vl && row + tile_row < m; ++tile_row) {
        float *c_row = c + ((size_t)(row + tile_row) * (size_t)ldc) + col;

        if (beta == 0.0f) {
            svst1_hor_za32(0, (uint32_t)tile_row, col_pg, c_row);
        } else {
            svfloat32_t acc = svread_hor_za32_f32_m(
                svdup_n_f32(0.0f),
                col_pg,
                0,
                (uint32_t)tile_row
            );
            svfloat32_t c_vec = svld1_f32(col_pg, c_row);

            if (beta == 1.0f) {
                acc = svadd_f32_m(col_pg, acc, c_vec);
            } else {
                acc = svmla_n_f32_m(col_pg, acc, c_vec, beta);
            }
            svst1_f32(col_pg, c_row, acc);
        }
    }
}

__arm_new("za") void cblas_sgemm_sme_za_kernel(
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
) __arm_streaming {
    const int vl = (int)svcntw();
    float *a_work = (float *)__builtin_alloca(sizeof(float) * (size_t)vl);

    for (int row = 0; row < m; row += vl) {
        svbool_t row_pg = svwhilelt_b32((uint64_t)row, (uint64_t)m);
        const float *a_panel = a + ((size_t)row * (size_t)lda);
        int row_count = m - row;

        if (row_count > vl) {
            row_count = vl;
        }

        for (int col = 0; col < n; col += vl) {
            svbool_t col_pg = svwhilelt_b32((uint64_t)col, (uint64_t)n);

            svzero_za();

            for (int depth = 0; depth < k; ++depth) {
                for (int tile_row = 0; tile_row < row_count; ++tile_row) {
                    a_work[tile_row] = alpha * a_panel[
                        ((size_t)tile_row * (size_t)lda) + depth
                    ];
                }
                svfloat32_t a_vec = svld1_f32(row_pg, a_work);
                svfloat32_t b_vec = svld1_f32(
                    col_pg,
                    b + ((size_t)depth * (size_t)ldb) + col
                );

                svmopa_za32_f32_m(0, row_pg, col_pg, a_vec, b_vec);
            }

            cblas_sgemm_sme_store_tile(col_pg, row, m, col, beta, c, ldc);
        }
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
    cblas_sgemm_sme_za_kernel(m, n, k, alpha, a, lda, b, ldb, beta, c, ldc);
}

__attribute__((weak, visibility("default")))
void __arm_tpidr2_save(void) __arm_streaming_compatible {
}

__attribute__((weak, visibility("default")))
void __arm_tpidr2_restore(void) __arm_streaming_compatible {
}

__attribute__((weak, visibility("default")))
void __arm_za_disable(void) __arm_streaming_compatible {
}
