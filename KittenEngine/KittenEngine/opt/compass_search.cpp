# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <ctime>

using namespace std;

# include "compass_search.hpp"

//****************************************************************************80

double *compass_search ( std::function<double(int, double[])> function_handle, int m,
  double x0[], double delta_tol, double delta_init, int k_max, double &fx, 
  int &k )

//****************************************************************************80
//
//  Purpose:
//
//    COMPASS_SEARCH carries out a direct search minimization algorithm.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    05 January 2012
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Tamara Kolda, Robert Michael Lewis, Virginia Torczon,
//    Optimization by Direct Search: New Perspectives on Some Classical 
//    and Modern Methods,
//    SIAM Review,
//    Volume 45, Number 3, 2003, pages 385-482. 
//
//  Parameters:
//
//    Input, double FUNCTION_HANDLE ( int m, double x[] ), the name of
//    a function which evaluates the function to be minimized.
//
//    Input, int M, the number of variables.
//
//    Input, double X0[M], a starting estimate for the minimizer.
//
//    Input, double DELTA_TOL, the smallest step size that is allowed.
//
//    Input, double DELTA_INIT, the starting stepsize.  
//
//    Input, int K_MAX, the maximum number of steps allowed.
//
//    Output, double COMPASS_SEARCH[M], the estimated minimizer.
//
//    Output, double &FX, the function value at X.
//
//    Output, int &K, the number of steps taken.
//
{
  bool decrease;
  double delta;
  double fxd;
  int i;
  int ii;
  double s;
  double *x;
  double *xd;

  k = 0;
  x = new double[m];
  xd = new double[m];
  r8vec_copy ( m, x0, x );
  fx = function_handle ( m, x );

  if ( delta_tol <= 0 )
  {
    cerr << "\n";
    cerr << "COMPASS_SEARCH - Fatal error!\n";
    cerr << "  DELTA_TOL <= 0.0.\n";
    cerr << "  DELTA_TOL = " << delta_tol << "\n";
    exit ( 1 );
  }

  if ( delta_init <= delta_tol )
  {
    cerr << "\n";
    cerr << "COMPASS_SEARCH - Fatal error!\n";
    cerr << "  DELTA_INIT < DELTA_TOL.\n";
    cerr << "  DELTA_INIT = " << delta_init << "\n";
    cerr << "  DELTA_TOL = " << delta_tol << "\n";
    exit ( 1 );
  }

  delta = delta_init;

  while ( k < k_max )
  {
    k = k + 1;
//
//  For each coordinate direction I, seek a lower function value
//  by increasing or decreasing X(I) by DELTA.
//
    decrease = false;
    s = + 1.0;
    i = 0;

    for ( ii = 1; ii <= 2 * m; ii++ )
    {
      r8vec_copy ( m, x, xd );
      xd[i] = xd[i] + s * delta;
      fxd = function_handle ( m, xd );
//
//  As soon as a decrease is noticed, accept the new point.
//
      if ( fxd < fx )
      {
        r8vec_copy ( m, xd, x );
        fx = fxd;
        decrease = true;
        break;
      }

      s = - s;
      if ( s == + 1.0 )
      {
        i = i + 1;
      }
    }
//
//  If no decrease occurred, reduce DELTA.
//
    if ( !decrease )
    {
      delta = delta / 2.0;
      if ( delta < delta_tol )
      {
        break;
      }
    }
  }

  delete [] xd;

  return x;
}
//****************************************************************************80
