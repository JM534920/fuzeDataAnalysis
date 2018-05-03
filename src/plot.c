#include "plot.h"

/******************************************************************************
 * Example Usage:
 * This is used to plot data using gnuplot
 ******************************************************************************/

/******************************************************************************
 * Function: plot1DVectorData
 * Inputs: gsl_vector*
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot1DVectorData (gsl_vector *vecIn, char *plotOptions) {

  int ii, status;
  
  FILE *gnuplot = popen("gnuplot", "w");

  if (!gnuplot) {
    fprintf (stderr,
	     "incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }

  fprintf(gnuplot, "%s\n", plotOptions);
  
  fprintf(gnuplot, "plot '-'\n");

  for (ii = 0; ii < vecIn->size; ii++) {

    fprintf(gnuplot, "%d %g\n", ii, gsl_vector_get(vecIn, ii));

  }

  fprintf(gnuplot, "e\n");

  fflush(gnuplot);

  /* Pausing */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  return 0;

}


/******************************************************************************
 * Function: plotVectorData
 * Inputs: gsl_vector*
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plotVectorData (gsl_vector *xVecIn, gsl_vector *yVecIn, char *plotOptions) {

  int ii, status;
  
  FILE *gnuplot = popen("gnuplot", "w");

  if (!gnuplot) {
    fprintf (stderr,
	     "incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }

  fprintf(gnuplot, "%s\n", plotOptions);
  
  fprintf(gnuplot, "plot '-'\n");

  for (ii = 0; ii < xVecIn->size; ii++) {

    fprintf(gnuplot, "%g %g\n", gsl_vector_get(xVecIn, ii), gsl_vector_get(yVecIn, ii));

  }

  fprintf(gnuplot, "e\n");

  fflush(gnuplot);

  /* Pausing */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  return 0;

}


/******************************************************************************
 * Function: plotVectorData
 * Inputs: gsl_vector*
 * Returns: int
 * Description: This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plot2VectorData (gsl_vector *xVecIn, gsl_vector *yVec1In, gsl_vector *yVec2In,
		     char *plotOptions) {

  int ii, status;

  char *tempFile = "data/temp.txt";


  
  /* Writing file to hold data */
  
  if (remove(tempFile) != 0) {
    printf("Unable to delete the file");
  }

  FILE *fp = fopen(tempFile, "w");
  
  if ( (fp == NULL) ) {

    printf("Error opening files!\n");
    exit(1);

  }

  for (ii = 0; ii < xVecIn->size; ii++) {

    fprintf(fp, "%g\t%g\t%g\n", gsl_vector_get(xVecIn, ii), 
	    gsl_vector_get(yVec1In, ii), gsl_vector_get(yVec2In, ii));

  }

  fclose(fp);






  /* 
   * Creating child process to then call "gnuplot" in shell, and then to pipe the standard output
   * to it.
   */
  
  FILE *gnuplot = popen("gnuplot", "w");

  if (!gnuplot) {
    fprintf(stderr,"incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }

  fprintf(gnuplot, "%s\n", plotOptions);
  
  fprintf(gnuplot, "plot 'data/temp.txt' using 1:2 title 'Vector 1',\
                    'data/temp.txt' using 1:3 title 'Vector 2'\n");
  
  fflush(gnuplot);

  /* Pausing so user can look at plot */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  return 0;

}


/******************************************************************************
 * Function: plotImageData
 * Inputs: gsl_matrix *
 * Returns: int
 * Description: This will save the data as binary floats, then use popen
 * which calls gnuplot in the shell in a seperate process, then pipes that
 * back to the parent process, and in gnuplot, it will open this binary file.
 ******************************************************************************/

int plotImageData (gsl_matrix *mInput, char *plotOptions) {

  int numRows = mInput->size1;
  int numCols = mInput->size2;

  /* Allocating the matrix to save */  
  gsl_matrix_float* temp = gsl_matrix_float_alloc(numRows+1, numCols+1);

  /* Set number of columns to 0,0 elements */
  gsl_matrix_float_set(temp,0,0,(float) numCols);

  int ii,jj;
  /* Setting y vector values to indices*/
  for (ii = 1; ii < numRows+1; ii++) {
    gsl_matrix_float_set(temp, ii, 0,
			 (float) ii);
  }
  /* Setting x vector values to indices*/
  for (ii = 1; ii < numCols+1; ii++) {
    gsl_matrix_float_set(temp, 0, ii,
			 (float) ii);
  }
  /* Setting matrix values */
  for (ii = 1; ii < numRows+1; ii++) {
    for (jj = 1; jj < numCols + 1; jj++) {

      gsl_matrix_float_set(temp, ii, jj,
			   (float) gsl_matrix_get(mInput,ii-1, jj-1));

    }
  }

  /* Writting temp matrix to a file */
  FILE *fp2;
  fp2 = fopen("data/temp.dat", "wb");
  gsl_matrix_float_fwrite (fp2, temp);
  fclose(fp2);

  gsl_matrix_float_free(temp);

  int status;
  
  FILE *gnuplot = popen("gnuplot", "w");

  if (!gnuplot) {
    fprintf (stderr,
	     "incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }

  fprintf(gnuplot, "%s\n", plotOptions);

  fprintf(gnuplot, "set palette rgb 33,13,10\n");
  fprintf(gnuplot, "plot 'data/temp.dat' binary matrix with image title ''\n");

  fflush(gnuplot);

  /* Pausing before kill process that has gnuplot */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  remove("data/temp.dat");

  return 0;

}


/******************************************************************************
 * Function: plotImageDataFile
 * Inputs: char *
 * Returns: int
 * Description: This will plot an image from a passed file. It uses popen
 * which calls gnuplot in the shell in a seperate process, then pipes that
 * back to the parent process, and in gnuplot, it will open this binary file.
 ******************************************************************************/

int plotImageDataFile(char *fileName, char *plotOptions) {

  int status;
  
  FILE *gnuplot = popen("gnuplot", "w");

  if (!gnuplot) {
    fprintf (stderr,
	     "incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }

  fprintf(gnuplot, "set palette rgb 33,13,10\n");
  fprintf(gnuplot, "%s\n", plotOptions);
  fprintf(gnuplot, "plot '%s' binary matrix with image title ''\n", fileName);

  fflush(gnuplot);

  /* Pausing before kill process that has gnuplot */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  remove("data/temp.dat");

  return 0;

}


/******************************************************************************
 * Function: plotMatrixDataFile
 * Inputs: char *
 * Returns: int
 * Description: You pass this text file with columns and it will plot 1 of 
 * the column. This will use popen to fork a process, execute a shell command
 * then attach a pipe between that shell command and a stream
 ******************************************************************************/

int plotMatrixColDataFile (char *fileName, int colNum, char *plotOptions) {

  int status;
  
  FILE *gnuplot = popen("gnuplot", "w");

  if (!gnuplot) {
    fprintf (stderr,
	     "incorrect parameters or too many files.\n");
    return EXIT_FAILURE;
  }

  fprintf(gnuplot, "%s\n", plotOptions);
  
  fprintf(gnuplot, "plot '%s' using 0:%d with points title '1'\n", fileName, colNum);
  
  fflush(gnuplot);

  /* Pausing so user can look at plot */
  getchar();

  status = pclose(gnuplot);

  if (status == -1) {
    printf("Error reported bp close");
  }

  return 0;

}
