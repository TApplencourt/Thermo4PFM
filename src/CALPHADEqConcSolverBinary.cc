#include "CALPHADEqConcSolverBinary.h"
#include "CALPHADFunctions.h"

namespace Thermo4PFM
{

#ifdef HAVE_OPENMP_OFFLOAD
#pragma omp declare target
#endif
void CALPHADEqConcSolverBinary::RHS(
    const double* const c, // composition of species A in various phases
    double* const fvec)
{
    // tbox::pout<<"Compute RHS for CALPHAD..."<<endl;
    double dfdci[2];

    dfdci[0] = fA_[0] - fB_[0]
               + CALPHADcomputeFMix_derivBinary(
                     Lmix_L_[0], Lmix_L_[1], Lmix_L_[2], Lmix_L_[3], c[0])
               + CALPHADcomputeFIdealMix_derivBinary(RT_, c[0]);
    dfdci[1] = fA_[1] - fB_[1]
               + CALPHADcomputeFMix_derivBinary(
                     Lmix_A_[0], Lmix_A_[1], Lmix_A_[2], Lmix_A_[3], c[1])
               + CALPHADcomputeFIdealMix_derivBinary(RT_, c[1]);

    fvec[0] = c[0] * fA_[0] + (1.0 - c[0]) * fB_[0]
              + CALPHADcomputeFIdealMixBinary(RT_, c[0])
              + CALPHADcomputeFMixBinary(
                    Lmix_L_[0], Lmix_L_[1], Lmix_L_[2], Lmix_L_[3], c[0])
              - c[1] * fA_[1] - (1.0 - c[1]) * fB_[1]
              - CALPHADcomputeFIdealMixBinary(RT_, c[1])
              - CALPHADcomputeFMixBinary(
                    Lmix_A_[0], Lmix_A_[1], Lmix_A_[2], Lmix_A_[3], c[1])
              - (c[0] - c[1]) * (dfdci[1]);

    // dfL/dcL - dfS/dcS
    fvec[1] = dfdci[0] - dfdci[1];
}

//=======================================================================

void CALPHADEqConcSolverBinary::Jacobian(
    const double* const c, JacobianDataType** const fjac)
{
    double dfdci[2];

    // loop over phases
    dfdci[0] = fA_[0] - fB_[0]
               + CALPHADcomputeFMix_derivBinary(
                     Lmix_L_[0], Lmix_L_[1], Lmix_L_[2], Lmix_L_[3], c[0])
               + CALPHADcomputeFIdealMix_derivBinary(RT_, c[0]);
    dfdci[1] = fA_[1] - fB_[1]
               + CALPHADcomputeFMix_derivBinary(
                     Lmix_A_[0], Lmix_A_[1], Lmix_A_[2], Lmix_A_[3], c[1])
               + CALPHADcomputeFIdealMix_derivBinary(RT_, c[1]);

    // f[i][j]=df[i]/dc[j]
    fjac[0][0] = dfdci[0] - dfdci[1];
    fjac[0][1] = -(c[0] - c[1])
                 * (CALPHADcomputeFIdealMix_deriv2Binary(RT_, c[1])
                       + CALPHADcomputeFMix_deriv2Binary(Lmix_A_[0], Lmix_A_[1],
                             Lmix_A_[2], Lmix_A_[3], c[1]));

    fjac[1][0] = CALPHADcomputeFMix_deriv2Binary(
                     Lmix_L_[0], Lmix_L_[1], Lmix_L_[2], Lmix_L_[3], c[0])
                 + CALPHADcomputeFIdealMix_deriv2Binary(RT_, c[0]);

    fjac[1][1] = -CALPHADcomputeFMix_deriv2Binary(
                     Lmix_A_[0], Lmix_A_[1], Lmix_A_[2], Lmix_A_[3], c[1])
                 - CALPHADcomputeFIdealMix_deriv2Binary(RT_, c[1]);
}

//=======================================================================

void CALPHADEqConcSolverBinary::setup(const double RTinv,
    const CalphadDataType* const Lmix_L, const CalphadDataType* const Lmix_A,
    const CalphadDataType* const fA, const CalphadDataType* const fB)
{
    RTinv_ = RTinv;
    RT_    = 1. / RTinv;

    for (int ii = 0; ii < 4; ii++)
    {
        Lmix_L_[ii] = Lmix_L[ii];
        Lmix_A_[ii] = Lmix_A[ii];
    }
    for (int ii = 0; ii < 2; ii++)
    {
        fA_[ii] = fA[ii];
        fB_[ii] = fB[ii];
    }
}
#ifdef HAVE_OPENMP_OFFLOAD
#pragma omp end declare target
#endif
}
