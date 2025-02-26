#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <iostream>

extern "C" {
#include "igo.h"
}

using namespace std;

class TestSolveIncrement : public ::testing::Test {
public:
    igo_common* igo_cm = nullptr;
    igo_sparse* igo_Atilde = nullptr;
    igo_sparse* igo_btilde = nullptr;
    igo_sparse* igo_Ahat = nullptr;
    igo_sparse* igo_bhat = nullptr;
    igo_factor* igo_L = nullptr;

    void SetUp() override {
        igo_cm = (igo_common*) malloc(sizeof(igo_common));
        igo_init(igo_cm);

        igo_Atilde = igo_allocate_sparse(0, 0, 0, igo_cm);
        igo_btilde = igo_allocate_sparse(0, 0, 0, igo_cm);

        igo_Ahat = igo_allocate_sparse(9, 9, 45, igo_cm);

        cholmod_sparse* Ahat = igo_Ahat->A;

        int* Ahatp = (int*) Ahat->p;
        int* Ahati = (int*) Ahat->i;
        double* Ahatx = (double*) Ahat->x;

        int Ahatp_setup[10] = {0, 3, 6, 9, 15, 21, 27, 33, 39, 45};
        int Ahati_setup[45] = {0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 3, 4, 5, 0, 1, 2, 
                            3, 4, 5, 0, 1, 2, 3, 4, 5, 3, 4, 5, 6, 7, 8, 3, 4, 5, 
                            6, 7, 8, 3, 4, 5, 6, 7, 8};
        double Ahatx_setup[45] = {1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 
                               0.000000, 0.000000, 1.000000, 1.000000, 0.000000, 0.000000, 
                               -0.999967, 0.008184, 0.033582, 0.000000, 1.000000, 0.000000, 
                               -0.008184, -0.999967, 0.999098, 0.000000, 0.000000, 1.000000, 
                               -0.000000, -0.000000, -1.000000, 1.000000, 0.000000, 0.000000, 
                               -0.999984, 0.005697, 0.017876, 0.000000, 1.000000, 0.000000, 
                               -0.005697, -0.999984, 1.003478, 0.000000, 0.000000, 1.000000, 
                               -0.000000, -0.000000, -1.000000};

        for(int i = 0; i < 10; i++) {
            Ahatp[i] = Ahatp_setup[i];
        }
        for(int i = 0; i < 45; i++) {
            Ahati[i] = Ahati_setup[i];
            Ahatx[i] = Ahatx_setup[i];
        }

        igo_bhat = igo_allocate_sparse(9, 1, 9, igo_cm);

        cholmod_sparse* bhat = igo_bhat->A;

        int* bhatp = (int*) bhat->p;
        int* bhati = (int*) bhat->i;
        double* bhatx = (double*) bhat->x;

        int bhatp_setup[2] = {0, 9};
        int bhati_setup[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
        double bhatx_setup[9] = {0, 0, 0, -1.11022e-16, -0, 4.3644e-19, 
                              -2.22045e-16, 6.93889e-18, -1.30884e-18};

        for(int i = 0; i < 2; i++) {
            bhatp[i] = bhatp_setup[i];
        }
        for(int i = 0; i < 9; i++) {
            bhati[i] = bhati_setup[i];
            bhatx[i] = bhatx_setup[i];
        }

        igo_L = igo_allocate_factor(3, 3, igo_cm);
    }

    void TearDown() override {
        igo_free_sparse(&igo_Atilde, igo_cm);
        ASSERT_EQ(igo_Atilde, nullptr);
        
        igo_free_sparse(&igo_btilde, igo_cm);
        ASSERT_EQ(igo_btilde, nullptr);

        igo_free_sparse(&igo_Ahat, igo_cm);
        ASSERT_EQ(igo_Ahat, nullptr);

        igo_free_sparse(&igo_bhat, igo_cm);
        ASSERT_EQ(igo_Ahat, nullptr);

        igo_free_factor(&igo_L, igo_cm);
        ASSERT_EQ(igo_L, nullptr);

        igo_finish(igo_cm);
        igo_cm = nullptr;
    }
};

TEST_F(TestSolveIncrement, ObsOnly) {
    igo_solve_increment(igo_Atilde, igo_btilde, igo_Ahat, igo_bhat, igo_cm);

    double x_cor[9] = {-4.44086e-28, 5.11888e-30, -8.75298e-30,
                       1.11e-16, -1.34449e-18,  -4.3644e-19,
                       3.33013e-16, -9.3052e-18, 8.72396e-19};

    double* xx = (double*) igo_cm->x->B->x;

    ASSERT_EQ(igo_cm->x->B->nrow, 9);
    ASSERT_EQ(igo_cm->x->B->ncol, 1);

    for(int i = 0; i < 9; i++) {
        EXPECT_NEAR(xx[i], x_cor[i], 1e-8);
    }

}
