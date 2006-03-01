//*****************************************************************
// Iterative template routine -- CGS
//
// CGS solves the unsymmetric linear system Ax = b 
// using the Conjugate Gradient Squared method
//
// CGS follows the algorithm described on p. 26 of the 
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
template < class Matrix, class Vector, class Preconditioner, class Real >
int 
CGS(const Matrix &A, Vector &x, const Vector &b,
X    const Preconditioner &M, int &max_iter, Real &tol)
{
X  Real resid;
X  Vector rho_1(1), rho_2(1), alpha(1), beta(1);
X  Vector p, phat, q, qhat, vhat, u, uhat;
X
X  Real normb = norm(b);
X  Vector r = b - A*x;
X  Vector rtilde = r;
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
X  for (int i = 1; i <= max_iter; i++) {
X    rho_1(0) = dot(rtilde, r);
X    if (rho_1(0) == 0) {
X      tol = norm(r) / normb;
X      return 2;
X    }
X    if (i == 1) {
X      u = r;
X      p = u;
X    } else {
X      beta(0) = rho_1(0) / rho_2(0);
X      u = r + beta(0) * q;
X      p = u + beta(0) * (q + beta(0) * p);
X    }
X    phat = M.solve(p);
X    vhat = A*phat;
X    alpha(0) = rho_1(0) / dot(rtilde, vhat);
X    q = u - alpha(0) * vhat;
X    uhat = M.solve(u + q);
X    x += alpha(0) * uhat;
X    qhat = A * uhat;
X    r -= alpha(0) * qhat;
X    rho_2(0) = rho_1(0);
X    if ((resid = norm(r) / normb) < tol) {
X      tol = resid;
X      max_iter = i;
X      return 0;
X    }
X  }
X  
X  tol = resid;
X  return 1;
}
X
