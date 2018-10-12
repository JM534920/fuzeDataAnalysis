#include "mcnp/readMeshTally.h"

/******************************************************************************
 *
 * This has some functions to read a mesh tally file outpued from MCNP
 *
 ******************************************************************************/

static gsl_vector *getMeshTallyXVector(char *fileName);
static gsl_vector *getMeshTallyYVector(char *fileName);
static gsl_vector *getMeshTallyZVector(char *fileName);
static double *getMeshTallyData(char *fileName, int xSize, int ySize, int zSize);
static double ***getMeshTally3DData(char *fileName);
static int save3DData(double ***data3D, int xSize, int ySize, int zSize, char *fileName);


/******************************************************************************
 * Function: save3DData
 * Inputs: double ***, char *
 * Returns: int
 * Description: Gets the data in a 3D arrray
 ******************************************************************************/

static int save3DData(double ***data3D, int xSize, int ySize, int zSize, char *fileName) {

  FILE *fp;

  fp = fopen(fileName, "wb");
  
  fwrite(data3D, sizeof(double), xSize*ySize*zSize, fp);

  fclose(fp);

  return 0;
  
}
  
/******************************************************************************
 * Function: getMeshTally3DData
 * Inputs: char *fileName, int, int, int, gsl_vector, int, t
 * Returns: double ***
 * Description: Gets the data in a 3D arrray
 ******************************************************************************/

static double ***getMeshTally3DData(char *fileName) {

  gsl_vector *xVec =
    getMeshTallyXVector("/home/webertr/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht");
  gsl_vector *yVec =
    getMeshTallyYVector("/home/webertr/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht");
  gsl_vector *zVec =
    getMeshTallyZVector("/home/webertr/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht");

  int xSize = xVec->size;
  int ySize = yVec->size;
  int zSize = zVec->size;
  
  double *data = 
    getMeshTallyData("/home/webertr/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht",
		     xSize, ySize, zSize);

  double ***data3D = (double ***) malloc(sizeof(double **)*xSize);

  int ii, jj, kk;
  for (ii = 0; ii < xSize; ii++) {
    data3D[ii] = (double **) malloc(sizeof(double *)*ySize);
    for (jj = 0; jj < ySize; jj++) {
      data3D[ii][jj] = (double *) malloc(sizeof(double)*zSize);
      for (kk = 0; kk < zSize; kk++) {
	data3D[ii][jj][kk] = data[ii+jj*xSize+kk*xSize*ySize];
      }
    }
  }

  free(data);
  gsl_vector_free(xVec);
  gsl_vector_free(yVec);
  gsl_vector_free(zVec);

  return data3D;
  
}

/******************************************************************************
 * Function: getMeshTallyData
 * Inputs: char *fileName, int, int, int, gsl_vector, int, int
 * Returns: gsl_vector *
 * Description: Gets the data. ii = x iindex, jj = y-index, kk = z-index
 * data(ii + jj*xSize + kk*xSize*ySize)
 ******************************************************************************/

static double *getMeshTallyData(char *fileName, int xSize, int ySize, int zSize) {


  FILE *fp = fopen(fileName, "r");

  const int BUF_SIZE = 10000;
  char charBuf[BUF_SIZE];

  /* data(ii + jj*xSize + kk*xSize*ySize) */
  double *data = malloc(sizeof(double)*xSize*ySize*zSize);
  double zMin, zMax, xValue, yValue, value;
  int ii, jj, kk;
  int resTest = 0;

  /* Iterating through Z-values */
  for (kk = 0; kk < (zSize-1); kk++) {
    
    resTest = 0;
    while ( resTest != 2 ) {
      fscanf(fp, "%[^\n]\n", charBuf);
      resTest = sscanf(charBuf, "Z bin: %lf - %lf", &zMin, &zMax);
    }

    resTest = 0;
    fscanf(fp, "%[^\n]\n", charBuf);
    while ( strcmp(charBuf, "Tally Results:  X (across) by Y (down)") != 0 ) {
      fscanf(fp, "%[^\n]\n", charBuf);
    }

    /* Geting x values */
    for (ii = 0; ii < (xSize-1); ii++) {
      fscanf(fp, "%lf", &xValue);
    }

    /* Iterating through y-values */
    for (jj = 0; jj < (ySize-1); jj++) {

      /* Getting y value */
      fscanf(fp, "%lf", &yValue);

      /* Getting data value */
      for (ii = 0; ii < (xSize-1); ii++) {
	fscanf(fp, "%lf", &value);
	data[ii + jj*xSize + kk*xSize*ySize] = value;
      }
    }

    /* Dumping the Relative Errors */
    fgets(charBuf, BUF_SIZE, fp);
    fgets(charBuf, BUF_SIZE, fp);
    /* If you want this data, just set it here instead of throwing it away */
    for (jj = 0; jj < (ySize - 1); jj++) {
      fgets(charBuf, BUF_SIZE, fp);
    }
  }
  
  return data;

}

/******************************************************************************
 * Function: getMeshTallyXVector
 * Inputs: char *fileName
 * Returns: gsl_vector *
 * Description: Gets the x values for the 3D array of the mesh tally file
 ******************************************************************************/

static gsl_vector *getMeshTallyXVector(char *fileName) {


  FILE *fp = fopen(fileName, "r");

  const int BUF_SIZE = 10000;
  char charBuf[BUF_SIZE];

  fscanf(fp, "%[^\n]\n", charBuf);
  while ( strcmp(charBuf, "Tally bin boundaries:") != 0 ) {
    fscanf(fp, "%[^\n]\n", charBuf);
  }
  unsigned long position;
  fflush(fp);
  position = ftell(fp);
    
  int count = 0;
  double value;
  fscanf(fp, "X direction:\t%lf", &value);
  while ( fscanf(fp, "%lf", &value) ) {
    count = count + 1;
  }
  count = count + 1;
  
  fseek(fp, position, SEEK_SET);

  gsl_vector *xVec = gsl_vector_alloc(count);
  fscanf(fp, "X direction:\t%lf", &value);
  gsl_vector_set(xVec, 0, value);
  int ii;
  for (ii = 1; ii < count; ii++) {
    fscanf(fp, "%lf", &value);
    gsl_vector_set(xVec, ii, value);
  }

  fclose(fp);
  
  return xVec;

}


/******************************************************************************
 * Function: getMeshTallyYVector
 * Inputs: char *fileName
 * Returns: gsl_vector *
 * Description: Gets the y values for the 3D array of the mesh tally file
 ******************************************************************************/

static gsl_vector *getMeshTallyYVector(char *fileName) {


  FILE *fp = fopen(fileName, "r");

  const int BUF_SIZE = 10000;
  char charBuf[BUF_SIZE];

  fscanf(fp, "%[^\n]\n", charBuf);
  while ( strcmp(charBuf, "Tally bin boundaries:") != 0 ) {
    fscanf(fp, "%[^\n]\n", charBuf);
  }
  fscanf(fp, "%[^\n]\n", charBuf);
  unsigned long position;
  fflush(fp);
  position = ftell(fp);
    
  int count = 0;
  double value;
  fscanf(fp, "Y direction:\t%lf", &value);
  while ( fscanf(fp, "%lf", &value) ) {
    count = count + 1;
  }
  count = count + 1;
  
  fseek(fp, position, SEEK_SET);

  gsl_vector *yVec = gsl_vector_alloc(count);
  fscanf(fp, "Y direction:\t%lf", &value);
  gsl_vector_set(yVec, 0, value);
  int ii;
  for (ii = 1; ii < count; ii++) {
    fscanf(fp, "%lf", &value);
    gsl_vector_set(yVec, ii, value);
  }

  fclose(fp);
  
  return yVec;

}


/******************************************************************************
 * Function: getMeshTallyZVector
 * Inputs: char *fileName
 * Returns: gsl_vector *
 * Description: Gets the z values for the 3D array of the mesh tally file
 ******************************************************************************/

static gsl_vector *getMeshTallyZVector(char *fileName) {


  FILE *fp = fopen(fileName, "r");

  const int BUF_SIZE = 10000;
  char charBuf[BUF_SIZE];

  fscanf(fp, "%[^\n]\n", charBuf);
  while ( strcmp(charBuf, "Tally bin boundaries:") != 0 ) {
    fscanf(fp, "%[^\n]\n", charBuf);
  }
  fscanf(fp, "%[^\n]\n", charBuf);
  fscanf(fp, "%[^\n]\n", charBuf);
  unsigned long position;
  fflush(fp);
  position = ftell(fp);
    
  int count = 0;
  double value;
  fscanf(fp, "Z direction:\t%lf", &value);
  while ( fscanf(fp, "%lf", &value) ) {
    count = count + 1;
  }
  count = count + 1;
  
  fseek(fp, position, SEEK_SET);

  gsl_vector *zVec = gsl_vector_alloc(count);
  fscanf(fp, "Z direction:\t%lf", &value);
  gsl_vector_set(zVec, 0, value);
  int ii;
  for (ii = 1; ii < count; ii++) {
    fscanf(fp, "%lf", &value);
    gsl_vector_set(zVec, ii, value);
  }

  fclose(fp);
  
  return zVec;

}



/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

int testReadMeshTally() {

  char *fileName = "/home/webertr/webertrNAS/mcnpOutputFiles/9_15_Full/inpFullMeshmsht";
  gsl_vector *xVec = getMeshTallyXVector(fileName);
  gsl_vector *yVec = getMeshTallyYVector(fileName);
  gsl_vector *zVec = getMeshTallyZVector(fileName);

  int xSize = xVec->size;
  int ySize = yVec->size;
  int zSize = zVec->size;
  
  double *data = getMeshTallyData(fileName, xSize, ySize, zSize);

  int ii;
  for (ii = 0; ii < (xVec->size); ii++) {
    printf("X-Vec(%d): %g\n", ii, gsl_vector_get(xVec, ii));
  }
  for (ii = 0; ii < (yVec->size); ii++) {
    printf("Y-Vec(%d): %g\n", ii, gsl_vector_get(yVec, ii));
  }
  for (ii = 0; ii < (zVec->size); ii++) {
    printf("Z-Vec(%d): %g\n", ii, gsl_vector_get(zVec, ii));
  }


  double ***data3D = (double ***) malloc(sizeof(double **)*xSize);

  int jj, kk;
  for (ii = 0; ii < xSize; ii++) {
    data3D[ii] = (double **) malloc(sizeof(double *)*ySize);
    for (jj = 0; jj < ySize; jj++) {
      data3D[ii][jj] = (double *) malloc(sizeof(double)*zSize);
      for (kk = 0; kk < zSize; kk++) {
	data3D[ii][jj][kk] = data[ii+jj*xSize+kk*xSize*ySize];
      }
    }
  }

  ii = 2;
  jj = 1;
  kk = 1;
  
  printf("data[0](1.30282E-06): %g\n", data[ii+jj*xSize+kk*xSize*ySize]);
  printf("data[0](1.30282E-06): %g\n", data3D[ii][jj][kk]);

  free(data3D);
  free(data);
  gsl_vector_free(xVec);
  gsl_vector_free(yVec);
  gsl_vector_free(zVec);


  double ***dataTest = getMeshTally3DData(fileName);
  printf("data[0](1.30282E-06): %g\n", dataTest[ii][jj][kk]);
  
  return 0;

}
