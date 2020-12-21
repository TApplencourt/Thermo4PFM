#ifndef included_NewtonSolver
#define included_NewtonSolver

namespace Thermo4PFM
{

class NewtonSolver
{
public:
    NewtonSolver(const int ndim);

    virtual ~NewtonSolver(){};

    virtual int ComputeSolution(double* const conc);

    void SetTolerance(const double t) { tolerance_ = t; }

    void SetMaxIterations(const int m) { max_iters_ = m; }

    void SetVerbose(const bool verbose) { verbose_ = verbose; }

    void SetDamping(const double alpha) { alpha_ = alpha; }

    virtual void UpdateSolution(
        double* const x, const double* const fvec, double** const fjac);

    virtual void RHS(const double* const x, double* const fvec) = 0;

protected:
    double Determinant(double** const mat);
    void CopyMatrix(double** const dst, double** const src);

private:
    virtual void Jacobian(const double* const x, double** const fjac) = 0;

    bool CheckTolerance(const double* const fvec);

    // Number of equations in system
    int ndim_;

    int max_iters_;

    // damping factor
    double alpha_;

    double tolerance_;
    bool verbose_;

    // function to compute determinant of matrix of size ndim_
    double (*det_fun_ptr_)(double** const matrix);
};
}

#endif
