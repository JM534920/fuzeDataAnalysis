#include "fit.h"

static int gaussian_f(const gsl_vector *paramVec, void *dataStruct, gsl_vector *costFun);
static int gaussian_df(const gsl_vector *paramVec, void *data, gsl_matrix *jacobianMatrix);
static void iterCallBack(const size_t iter, void *params,
			 const gsl_multifit_nlinear_workspace *workSpace);



/******************************************************************************
 *
 * This is the source file to fit data for the FuZE experiment
 *
 ******************************************************************************/


/*
 * Declares the struct dataStruct for use in this file 
 */
struct dataStruct {
  size_t numPoints;
  double *yValues;
};


static int gaussian_f(const gsl_vector *paramVec, void *dataStruct, gsl_vector *costFun) {
  
  size_t numPoints = ((struct dataStruct *)dataStruct)->numPoints;
  double *yValues = ((struct dataStruct *)dataStruct)->yValues;

  double A = gsl_vector_get (paramVec, 0);
  double center = gsl_vector_get (paramVec, 1);
  double sigma = gsl_vector_get (paramVec, 2);
  double b = gsl_vector_get (paramVec, 3);

  size_t ii;
  double x, Yi, ex;

  for (ii = 0; ii < numPoints; ii++) {
    /* Model Yi = A * exp(-(i - i0)^2/sigma^2) + b */
    x = (double) ii;
    ex = gsl_pow_2(x-center)/gsl_pow_2(sigma);
    Yi = A * gsl_sf_exp(-ex) + b;
    gsl_vector_set (costFun, ii, Yi - yValues[ii]);
      
    }

  return GSL_SUCCESS;
}

static int gaussian_df(const gsl_vector *paramVec, void *dataStruct, gsl_matrix *jacobianMatrix) {
  
  size_t n = ((struct dataStruct *)dataStruct)->numPoints;

  double A = gsl_vector_get (paramVec, 0);
  double center = gsl_vector_get (paramVec, 1);
  double sigma = gsl_vector_get (paramVec, 2);

  size_t ii;

  double val, ex, x;

  for (ii = 0; ii < n; ii++) {
    /* Jacobian matrix J(i,j) = dfi / dxj, */
    /* where fi = (Yi - yi)/sigma[i],      */
    /*       Yi = A * exp(-(x-center)^2/sigma^2 ) + b  */
    /* and the xj are the parameters (A,center, sigma,b) */

    x = (double) ii;
    ex = gsl_pow_2(x-center)/gsl_pow_2(sigma);
    val = gsl_sf_exp(-ex);
    gsl_matrix_set (jacobianMatrix, ii, 0, val); 
    gsl_matrix_set (jacobianMatrix, ii, 1, -2 * A/gsl_pow_2(sigma) * (x - center) * val);
    gsl_matrix_set (jacobianMatrix, ii, 2, 2 * A /gsl_pow_3(sigma) *gsl_pow_2(x-center) * val);
    gsl_matrix_set (jacobianMatrix, ii, 3, 1.0);
    }
  return GSL_SUCCESS;
}



static void iterCallBack(const size_t iter, void *params,
			 const gsl_multifit_nlinear_workspace *workSpace) {
  
  gsl_vector *f = gsl_multifit_nlinear_residual(workSpace);
  gsl_vector *x = gsl_multifit_nlinear_position(workSpace);
  double rcond;

  /* compute reciprocal condition number of J(x) */
  gsl_multifit_nlinear_rcond(&rcond, workSpace);

  fprintf(stderr, "iter %2zu: A = %.4f, center = %.4f, width = %.4f, b = %.4f, cond(J) = %8.4f, \
|f(x)| = %.4f\n",
          iter,
          gsl_vector_get(x, 0),
          gsl_vector_get(x, 1),
          gsl_vector_get(x, 2),
	  gsl_vector_get(x, 3),
          1.0 / rcond,
          gsl_blas_dnrm2(f));
}


/******************************************************************************
 * Function: fitGaussian
 * Inputs: gsl_vector *, gsl_vector *, double, double
 * Returns: int
 * Description: This will fit a gaussian to the passed x vs. y vectors and
 * set the amplitude and width parameters
 ******************************************************************************/

int fitGaussian (gsl_vector *vecY, double *amplitude, double *center,
		 double *width, double *offset) {

  /* 
   * Specifies the type of algorhtym used to solve non linear least squares problem.
   * this is the only option. "Trust Region Method"
   */
  const gsl_multifit_nlinear_type *trustRegionMethod = gsl_multifit_nlinear_trust;
  gsl_multifit_nlinear_workspace *workSpace;
  gsl_multifit_nlinear_fdf fdf;
  gsl_multifit_nlinear_parameters fdf_params = gsl_multifit_nlinear_default_parameters();
  const size_t numPoints = vecY->size;
  const size_t numParameters = 4;

  /* Cost function */
  gsl_vector *costFun;

  /* Jacobian matrix */
  gsl_matrix *jacobMatrix;
  
  gsl_matrix *covar = gsl_matrix_alloc (numParameters, numParameters);
  double weights[numPoints];
  double *yValues = vecY->data;
  struct dataStruct dataStruct = { numPoints, yValues };
  
  /* Initial guess at parameters */
  double paramInit[numParameters];
  paramInit[0] = *amplitude;
  paramInit[1] = *center;
  paramInit[2] = *width;
  paramInit[3] = *offset;

  /* Setting the weights to 1/sigma, where sigma is the error in the ith measurements */
  int ii;
  for (ii = 0; ii < numPoints; ii++) {
    weights[ii] = yValues[ii];
  }
  
  gsl_vector_view paramInitVec = gsl_vector_view_array(paramInit, numParameters);
  gsl_vector_view wts = gsl_vector_view_array(weights, numPoints);
  double chisq, chisq0;
  int status, info;

  const double xtol = 1e-8;
  const double gtol = 1e-8;
  const double ftol = 0.0;

  /* define the function to be minimized */
  fdf.f = gaussian_f;
  fdf.df = NULL;            /* set to NULL for finite-difference Jacobian */
  //fdf.df = gaussian_df;   /* set to NULL for finite-difference Jacobian */
  fdf.fvv = NULL;     /* not using geodesic acceleration */
  fdf.n = numPoints;
  fdf.p = numParameters;
  fdf.params = &dataStruct;


  /* allocate workspace with default parameters */
  workSpace = gsl_multifit_nlinear_alloc (trustRegionMethod, &fdf_params, numPoints, numParameters);

  /* initialize solver with starting point and weights */
  gsl_multifit_nlinear_winit (&paramInitVec.vector, &wts.vector, &fdf, workSpace);

  /* compute initial cost function */
  costFun = gsl_multifit_nlinear_residual(workSpace);
  gsl_blas_ddot(costFun, costFun, &chisq0);

  /* solve the system with a maximum of 50 iterations */
  status = gsl_multifit_nlinear_driver(50, xtol, gtol, ftol,
                                       iterCallBack, NULL, &info, workSpace);

  /* compute covariance of best fit parameters */
  jacobMatrix = gsl_multifit_nlinear_jac(workSpace);
  gsl_multifit_nlinear_covar (jacobMatrix, 0.0, covar);

  /* compute final cost */
  gsl_blas_ddot(costFun, costFun, &chisq);

  *amplitude = gsl_vector_get(workSpace->x, 0);
  *center = gsl_vector_get(workSpace->x, 1);
  *width = gsl_vector_get(workSpace->x, 2);
  *offset = gsl_vector_get(workSpace->x, 3);
  
#define FIT(i) gsl_vector_get(workSpace->x, i)
#define ERR(i) sqrt(gsl_matrix_get(covar,i,i))


  double dof = numPoints - numParameters;
  double c = GSL_MAX_DBL(1, sqrt(chisq / dof));

  fprintf(stderr, "chisq/dof = %g\n", chisq / dof);

  fprintf (stderr, "A      = %.5f +/- %.5f\n", FIT(0), c*ERR(0));
  fprintf (stderr, "center = %.5f +/- %.5f\n", FIT(1), c*ERR(1));
  fprintf (stderr, "width  = %.5f +/- %.5f\n", FIT(2), c*ERR(2));
  fprintf (stderr, "offset = %.5f +/- %.5f\n", FIT(3), c*ERR(3));

  fprintf (stderr, "status = %s\n", gsl_strerror (status));

  gsl_multifit_nlinear_free(workSpace);
  gsl_matrix_free(covar);

  return 0;
  
}
