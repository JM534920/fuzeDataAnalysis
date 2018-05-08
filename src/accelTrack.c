#include "accelTrack.h"

/******************************************************************************
 *
 * This file will solve a system of ODE's that will track a charged particles
 * through the accelerator region of the FuZE experiment
 *
 ******************************************************************************/

static const double QP   = 4.8032E-10;    // Charge of a proton in stat-columbs
static const double MP   = 1.673E-24;     // Mass of proton in g
//static const double ME   = 9.109e-28;   // Mass of electron in g
static const double PI   = 3.1416;        // Value of pi
static const double B_R  = .1008;         // ID of outer electrode
static const double A_R  = .1008;         // OD of inner electrode
static const double MU_0 = 1.257E-6;      // Permeability of free space in m kg s^-2 A^-2
static const int DIM_SYS = 6;             // Dimensions of system of equations  

/******************************************************************************
 * Function: equationSystem
 * Inputs: double, double*, double*, void *
 * Returns: int
 * Description: The system of equations that represent that charged particles
 * equations of motion (These are technically ODE's).
 * y0 = r, y1 = theta, y2 = z, y3 = Vr, y4 = Vt, y5 = Vz
 * dy0/dt = y3, dy1/dt = y4, dy2/dt = y5,
 * dy3/dt = y0*y4^2 + q/m*V0/ln(b/a)*1/y0 - q/m*y5*MU_0*I/(2*pi*y0)
 * dy4/dt = -2/y0*y3*y4
 * dy5/dt = q/m*y3*MU_0*I/(2*pi*y0)
 ******************************************************************************/

static int equationSystem(double time, const double y[], double dydt[], void *params) {
  
  (void)(time); /* avoid unused parameter warning */
  double *paramCast = (double *)params;
  double V0 = *paramCast;
  paramCast++;
  double I = *paramCast;
  paramCast++;
  double m = *paramCast;
  paramCast++;
  double q = *paramCast;

  double Bt = y[5]*MU_0*I/(2*PI*y[0]);
  double Er = V0/log(B_R/A_R)/y[0];
  
  dydt[0] = y[3];
  dydt[1] = y[4];
  dydt[2] = y[5];
  dydt[3] = y[0]*gsl_pow_2(y[4]) + q/m*Er - q/m*Bt;
  dydt[4] = -2/y[0]*y[3]*y[4];
  dydt[5] = q/m*y[3]*Bt;
  
  return GSL_SUCCESS;

}


/******************************************************************************
 * Function: jacobian
 * Inputs: double, double, double *, double*, void *
 * Returns: int
 * Description: The jacobian of the system of ODE's
 ******************************************************************************/

static int jacobian(double time, const double y[], double *dfdy, double dfdt[], void *params) {
  
  (void)(time); /* avoid unused parameter warning */
  double mu = *(double *)params;
  gsl_matrix_view dfdy_mat = gsl_matrix_view_array (dfdy, DIM_SYS, DIM_SYS);
  gsl_matrix *m = &dfdy_mat.matrix; 
  gsl_matrix_set(m, 0, 0, 0.0);
  gsl_matrix_set(m, 0, 1, 0.0);
  gsl_matrix_set(m, 0, 2, 0.0);
  gsl_matrix_set(m, 0, 3, 0.0);
  gsl_matrix_set(m, 0, 3, 0.0);
  gsl_matrix_set(m, 1, 0, 0.0);
  gsl_matrix_set(m, 1, 1, 0.0);
  gsl_matrix_set(m, 1, 1, 0.0);
  dfdt[0] = 0.0;
  dfdt[1] = 0.0;
  return GSL_SUCCESS;
  
}


/******************************************************************************
 * Function: simulateParticleAccel
 * Inputs: double, double
 * Returns: int
 * Description: This will run a simulation of a charged particle through a
 * the accelerator region of the fuze experiment.
 ******************************************************************************/

int simulateParticleAccel(double V, double I) {

  double M = MP;
  double Q = QP;
  
  double param[4] = {I, V, M, Q};

  gsl_odeiv2_system system = {equationSystem, jacobian, DIM_SYS, param};

  gsl_odeiv2_driver *driver = gsl_odeiv2_driver_alloc_y_new(&system, gsl_odeiv2_step_rk8pd,
							    1E-6, 1E-6, 0.0);
  int ii;
  double t = 0.0, t1 = 100.0;
  double y[2] = { 1.0, 0.0 };

  for (ii = 1; ii <= 100; ii++) {
    
    double ti = ii * t1 / 100.0;
    int status = gsl_odeiv2_driver_apply(driver, &t, ti, y);

    if (status != GSL_SUCCESS) {
      printf ("error, return value=%d\n", status);
      break;
    }

    printf ("%.5e %.5e %.5e\n", t, y[0], y[1]);
  }

  gsl_odeiv2_driver_free(driver);
  return 0;

}
