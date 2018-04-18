#include "analysis.h"

/******************************************************************************
 *
 * This is the source file to analysis data from the FuZE experiment
 *
 ******************************************************************************/


/******************************************************************************
 * Function: plotPostShotData
 * Inputs: int
 * Returns: int
 * Description: Pass it a shot number, and it will plot data to view after
 * each pulse.
 ******************************************************************************/

int plotPostShotData(int shotNumber) {

  int sigSize = getSignalLengthMDSplus("\\b_n45_000_sm", shotNumber);
  
  gsl_vector *data1 = gsl_vector_alloc(sigSize),
    *data2 = gsl_vector_alloc(sigSize),
    *time = gsl_vector_alloc(sigSize);

  initializeMagneticDataAndTime(shotNumber, "\\b_n95_000_sm", data1, time);
  initializeMagneticData(shotNumber, "\\b_n10_000_sm", data2);

  plot2VectorData(time, data1, data2, "set xrange[0:50E-6]");

  gsl_vector_free(data1);
  gsl_vector_free(data2);
  gsl_vector_free(time);

  return 0;

}


/******************************************************************************
 * Function: plotIPApril2018Talk
 * Inputs: int
 * Returns: int
 * Description: This will use gnu plot to plot IP for a talk that I'm giving
 * on April 28th, 2018
 ******************************************************************************/

int plotIPApril2018Talk(int shotNumber) {

  int sigSize = getSignalLengthMDSplus("\\b_n45_000_sm", shotNumber);
  
  /* Declaring variables */
  gsl_vector *iP = gsl_vector_alloc(sigSize),
    *cap1 = gsl_vector_alloc(sigSize),
    *timeDetaq = gsl_vector_alloc(sigSize),
    *timeCAMAC = gsl_vector_alloc(sigSize);

  int status;
  
  char *gnuPlotFile = "script/temp.sh",
    *ipFile = "data/ip.txt",
    *cap1File = "data/cap1.txt";
  
  /* Geting Data */
  initializeMagneticDataAndTime(shotNumber, "\\I_P", iP, timeDetaq);
  initializeMagneticDataAndTime(shotNumber, "\\I_TBM_01_CAP", cap1, timeCAMAC);


  /* Saving data */
  save2VectorData(timeDetaq, iP, ipFile);
  save2VectorData(timeCAMAC, cap1, cap1File);



  /* Creating gnuplot file */

  if (remove(gnuPlotFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(gnuPlotFile, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fp, "#!/usr/bin/env gnuplot\n");
  fprintf(fp, "set xrange[-15E-6:600E-6]\n");
  fprintf(fp, "plot '%s' using 1:($2/1E3) title 'IP',\\\n", ipFile);
  fprintf(fp, "     '%s' using ($1-16E-6):(12*$2/1E3) title 'Cap 1'\n", cap1File);
  fprintf(fp, "pause -1\n");
  
  fclose(fp);

  chmod(gnuPlotFile, S_IRWXG);
  chmod(gnuPlotFile, S_IRWXO);
  chmod(gnuPlotFile, S_IRWXU);


  

  /* Creating child process to run script */
  FILE *gnuplot = popen(gnuPlotFile, "r");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }
  
  fflush(gnuplot);

  /* Pausing so user can look at plot */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  

  
  /* Freeing vectors */
  gsl_vector_free(iP);
  gsl_vector_free(cap1);
  gsl_vector_free(timeDetaq);
  gsl_vector_free(timeCAMAC);

  return 0;

}


/******************************************************************************
 * Function: plotAccelApril2018Talk
 * Inputs: int
 * Returns: int
 * Description: This will use gnu plot to plot for acceleration data 
 * for a talk that I'm giving on April 28th, 2018
 ******************************************************************************/

int plotAccelApril2018Talk() {

  int shotNumber = 180223035,
    status,
    sigSize = getSignalLengthMDSplus("\\b_n45_000_sm", shotNumber);

  /* Declaring variables */
  gsl_vector *n45 = gsl_vector_alloc(sigSize),
    *n35 = gsl_vector_alloc(sigSize),
    *n25 = gsl_vector_alloc(sigSize),
    *n15 = gsl_vector_alloc(sigSize),
    *timeDetaq = gsl_vector_alloc(sigSize);

  
  char *gnuPlotFile = "script/temp.sh",
    *accelFile = "data/accel.txt";
  
  /* Geting Data */
  initializeMagneticDataAndTime(shotNumber, "\\b_n45_000_sm", n45, timeDetaq);
  initializeMagneticData(shotNumber, "\\b_n35_000_sm", n35);
  initializeMagneticData(shotNumber, "\\b_n25_000_sm", n25);
  initializeMagneticData(shotNumber, "\\b_n15_000_sm", n15);


  /* Saving data */
  save5VectorData(timeDetaq, n45, n35, n25, n15, accelFile);


  /* Creating gnuplot file */

  if (remove(gnuPlotFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(gnuPlotFile, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fp, "#!/usr/bin/env gnuplot\n");
  //fprintf(fp, "set terminal png\n");
  //fprintf(fp, "set output 'data/accel180223035'\n");
  fprintf(fp, "set xrange[0:15]\n");
  fprintf(fp, "set key left top\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'Acceleration Region for Pulse #%d' font '0,18'\n", shotNumber);
  fprintf(fp, "set xlabel 'time ({/Symbol m}sec)' font ',16' offset 0,0\n");
  fprintf(fp, "set ylabel 'B_{/Symbol q} (Tesla)' font ',16' offset 0,0\n");
  fprintf(fp, "plot '%s' using (($1+14E-6)*1E6):($2) with line lw 3 lc rgb 'black' \
title 'z = -45 cm',\\\n", accelFile);
  fprintf(fp, "     '%s' using (($1+14E-6)*1E6):($3) with line lw 3 lc rgb 'red' \
title 'z = -35 cm',\\\n", accelFile);
  fprintf(fp, "     '%s' using (($1+14E-6)*1E6):($4) with line lw 3 lc rgb 'blue' \
title 'z = -25 cm',\\\n", accelFile);
  fprintf(fp, "     '%s' using (($1+14E-6)*1E6):($5) with line lw 3 lc rgb 'yellow' \
title 'z = -15 cm'\n", accelFile);
  fprintf(fp, "pause -1\n");
  
  fclose(fp);

  chmod(gnuPlotFile, S_IRWXG);
  chmod(gnuPlotFile, S_IRWXO);
  chmod(gnuPlotFile, S_IRWXU);


  

  /* Creating child process to run script */
  FILE *gnuplot = popen(gnuPlotFile, "r");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }
  
  fflush(gnuplot);

  /* Pausing so user can look at plot */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  

  
  /* Freeing vectors */
  gsl_vector_free(n45);
  gsl_vector_free(n35);
  gsl_vector_free(n25);
  gsl_vector_free(n15);
  gsl_vector_free(timeDetaq);

  return 0;

}


/******************************************************************************
 * Function: plotModeApril2018Talk
 * Inputs: int
 * Returns: int
 * Description: This will use gnu plot to plot for mode data 
 * for a talk that I'm giving on April 28th, 2018
 ******************************************************************************/

int plotModeApril2018Talk() {

  int shotNumber = 180222040,
    status;

  char *gnuPlotFile = "script/temp.sh",
    *modeFile = "data/mode.txt";

  
  
  /* Getting data */
  gsl_matrix *azimuthArray = getAzimuthalArray(shotNumber, "\\b_p15_000_sm");
  getAzimuthalArrayModes(azimuthArray);

  

  /* Saving data */
  saveMatrixData(azimuthArray, modeFile);


  
  /* Creating gnuplot file */
  if (remove(gnuPlotFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(gnuPlotFile, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files gnuplot file!\n");
    exit(1);

  }

  fprintf(fp, "#!/usr/bin/env gnuplot\n");
  fprintf(fp, "set terminal png\n");
  fprintf(fp, "set output 'data/modeData.png'\n");
  fprintf(fp, "set xrange[15:45]\n");
  fprintf(fp, "set yrange[0:1]\n");
  fprintf(fp, "set y2range[0:]\n");
  fprintf(fp, "set tics font 'Times Bold, 14'\n");
  fprintf(fp, "set key right top\n");
  fprintf(fp, "set arrow from 15,0.2 to 45,0.2 nohead dt 3 lw 2 lc rgb 'green'\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set title 'Normalized modes at z=15 cm for pulse #%d' font '0,14'\n", shotNumber);
  fprintf(fp, "set xlabel 'Time ({/Symbol m}sec)' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "set ylabel 'Normalized modes' font 'Times Bold,18' offset 0,0\n");
    fprintf(fp, "set y2tics nomirror tc lt 2\n");
  fprintf(fp, "set y2label 'Pinch Current (kA)' font 'Times Bold,18' offset 0,0\n");
  fprintf(fp, "plot '%s' using (($1+15.2E-6)*1E6):($3) with line dt 2 lw 3 lc rgb 'red' \
title 'm=1',\\\n", modeFile);
  fprintf(fp, "     '%s' using (($1+15.2E-6)*1E6):($4) with line dt 3 lw 3 lc rgb 'blue' \
title 'm=2',\\\n", modeFile);
  fprintf(fp, "     '%s' using (($1+15.2E-6)*1E6):($2/0.002) with line lw 3 lc rgb 'black' \
title 'Pinch Current' axes x1y2\n", modeFile);
  fprintf(fp, "pause -1\n");
  
  fclose(fp);

  chmod(gnuPlotFile, S_IRWXG);
  chmod(gnuPlotFile, S_IRWXO);
  chmod(gnuPlotFile, S_IRWXU);


  

  /* Creating child process to run script */
  FILE *gnuplot = popen(gnuPlotFile, "r");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }
  
  fflush(gnuplot);

  /* Pausing so user can look at plot */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  

  
  /* Freeing vectors */
  gsl_matrix_free(azimuthArray);

  
  return 0;


}
