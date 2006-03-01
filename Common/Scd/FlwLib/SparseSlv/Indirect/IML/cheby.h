//*****************************************************************
// Iterative template routine -- CHEBY
//
// CHEBY solves the symmetric positive definite linear
// system Ax = b using the Preconditioned Chebyshev Method
//
// CHEBY follows the algorithm described on p. 30 of the 
// SIAM Templates book.
//
// The return value indicates convergence within max_iter (input)
// iterations (0), or no convergence within max_iter iterations (1).
//
// Upon successful return, output arguments have the following values:
//  
//        x  --  approximate solution to Ax = b
// max_iter  --  the number of iterations performed before the
//               tolerance was reached
//      tol  --  the residual after the final iteration
//  
//*****************************************************************
X
X
template < class Matrix, class Vector, class Preconditioner, class Real,
X           class Type >
int 
CHEBY(const Matrix &A, Vector &x, const Vector &b,
X      const Preconditioner &M, int &max_iter, Real &tol,
X      Type eigmin, Type eigmax)
{
X  Real resid;
X  Type alpha, beta, c, d;
X  Vector p, q, z;
X
X  Real normb = norm(b);
X  Vector r = b - A * x;
X
X  if (normb == 0.0)
X    normb = 1;
X  
X  if ((resid = norm(r) / normb) <= tol) {
X    tol = resid;
X    max_iter = 0;
X    return 0;
X  }
X
X  c = (eigmax - eigmin) / 2.0;
X  d = (eigmax + eigmin) / 2.0;
X
X  for (int i = 1; i <= max_iter; i++) {
X    z = M.solve(r);                 // apply preconditioner
X
X    if (i == 1) {
X      p = z;
X      alpha = 2.0 / d;
X    } else {
X      beta = c * alpha / 2.0;       // calculate new beta
X      beta = beta * beta;
X      alpha = 1.0 / (d - beta);     // calculate new alpha
X      p = z + beta * p;             // update search direction
X    }
X
X    q = A * p;
X    x += alpha * p;                 // update approximation vector
X    r -= alpha * q;                 // compute residual
X
X    if ((resid = norm(r) / normb) <= tol) {
X      tol = resid;
X      max_iter = i;
X      return 0;                     // convergence
X    }
X  }
X
X  tol = resid;
X  return 1;                         // no convergence
}
