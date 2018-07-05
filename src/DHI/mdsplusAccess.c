#include <DHI/mdsplusAccess.h>

/******************************************************************************
 * Example Usage:
 ******************************************************************************/


/******************************************************************************
 * Function: statusOk
 * Inputs: int
 * Returns: int
 * Description: Returns 1 if OK, 0 otherwise. Status is OK if the LSB is set.
 ******************************************************************************/

static int statusOk( int status ) {

  return ( (status & 1) == 1 );

}


/******************************************************************************
 * Function: getSignalLength
 * Inputs: const char *
 * Returns: int
 * Description: Returns the length of the specified signal if successful, -1
 * if not successfull. Assumes that the user is already connected to the MDSplus
 * server, and that the tree is already open. Uses the tdi intrinsic function
 * SIZE() to get the signal length
******************************************************************************/

static int getSignalLength(const char *signal) {

  int dtype_long = DTYPE_LONG;
  char buf[1024];
  int size;
  int null = 0;
  int idesc = descr(&dtype_long, &size, &null);
  int status;

  /* init buffer */
  memset(buf,0,sizeof(buf));

  /* put SIZE() TDI function around signal name */
  snprintf(buf,sizeof(buf)-1,"SIZE(%s)",signal);

  /* use MdsValue to get the signal length */
  status = MdsValue(buf, &idesc, &null, 0);

  if ( !( (status & 1) == 1 ) ) {
    fprintf(stderr,"Unable to get length of %s.\n",signal);
    return -1;
  }

  /* return signal length */
  return size;

}


/******************************************************************************
 * Function: writeDHIMDSplusImage
 * Inputs: gsl_matrix *, int, char *
 * Returns: int
 * Description: This will write an image to the DHI tree in mdsplus.
 * For this raw data, expression can just be:
 * expression = "$1"
 ******************************************************************************/

int writeDHIMDSplusImage(gsl_matrix* image, char *nodeName, char *expression, int shotNumber,
			 char *treeName) {

  int connectionStatus,
    connectionID,
    dTypeDouble = DTYPE_DOUBLE,
    null = 0,
    sigDescrImage,
    numRows = image->size1,
    numCols = image->size2;
  
  sigDescrImage = descr(&dTypeDouble, image->data, &numRows, &numCols, &null);

  connectionID = MdsConnect("localhost");
  if (connectionID == -1) {
    fprintf(stderr, "Connection Failed\n");
    return -1;
  }

  connectionStatus = MdsOpen(treeName, &shotNumber);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nMdsOpen error. Message: %s.\n", MdsGetMsg(connectionStatus));
    MdsDisconnect();
    return -1;
  }

  connectionStatus = MdsPut(nodeName, "$1", &sigDescrImage, &null);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nMdsPut error. Message: %s.\n", MdsGetMsg(connectionStatus));
    MdsClose(treeName, &shotNumber);
    MdsDisconnect();
    return -1;
  }

  connectionStatus = MdsClose(treeName, &shotNumber);
  
  return 0;

}


/******************************************************************************
 * Function: writeDHIMDSplusVector
 * Inputs: gsl_vector *, char *, char *, int
 * Returns: int
 * Description: This will write an image to the DHI tree in mdsplus.
 * Expression could be:
 * "build_signal(build_with_units($1,'m'),,)";
 ******************************************************************************/

int writeDHIMDSplusVector(gsl_vector *vecIn, char *nodeName, char *expression, int shotNumber,
			  char *treeName) {

  int connectionStatus,
    connectionID,
    dTypeDouble = DTYPE_DOUBLE,
    null = 0,
    sigDescrVector,
    numRows = vecIn->size;
  
  sigDescrVector = descr(&dTypeDouble, vecIn->data, &numRows, &null);

  connectionID = MdsConnect("localhost");
  if (connectionID == -1) {
    fprintf(stderr, "Connection Failed\n");
    return -1;
  }

  connectionStatus = MdsOpen(treeName, &shotNumber);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nMdsOpen error. Message: %s.\n", MdsGetMsg(connectionStatus));
    MdsDisconnect();
    return -1;
  }

  connectionStatus = MdsPut(nodeName, expression, &sigDescrVector, &null);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nMdsPut error. Message: %s.\n", MdsGetMsg(connectionStatus));
    MdsClose(treeName, &shotNumber);
    MdsDisconnect();
    return -1;
  }
   
  connectionStatus = MdsClose(treeName, &shotNumber);
  
  return 0;

}


/******************************************************************************
 * Function: readDHIMDSplusImage
 * Inputs: int, char *
 * Returns: gsl_matrix*
 * Description: 
 ******************************************************************************/

gsl_matrix *readDHIMDSplusImage(int shotNumber, char *nodeName, char *treeName) {

  int connectionStatus,
    connectionID,
    size,
    dtype_float = DTYPE_FLOAT,
    null = 0,
    sigDescrShape,
    sigDescrData,
    numRows,
    numCols,
    ii, jj;
    
  float *shape,
    *data;
  
  char buf[1024];

  gsl_matrix *nullM = 0;
  
  connectionID = MdsConnect("localhost");
  if (connectionID == -1) {
    fprintf(stderr, "Connection Failed\n");
    return nullM;
  }

  connectionStatus = MdsOpen(treeName, &shotNumber);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nMdsOpen error. Message: %s.\n", MdsGetMsg(connectionStatus));
    MdsDisconnect();
    return nullM;
  }

  snprintf(buf,sizeof(buf)-1,"shape(%s)",nodeName);
  size = getSignalLength(buf);
  shape = (float *)malloc(size * sizeof(float));  
  sigDescrShape = descr(&dtype_float, shape, &size, &null);
  connectionStatus = MdsValue(buf, &sigDescrShape, &null, 0);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nMdsValue error. Message: %s.\n", MdsGetMsg(connectionStatus));
    MdsClose(treeName, &shotNumber);
    MdsDisconnect();
    return nullM;

  }
  numRows = (int) shape[0];
  numCols = (int) shape[1];
  shape = (float *)malloc(size * sizeof(float));  
  gsl_matrix *mRet = gsl_matrix_alloc(numRows, numCols);
  size = getSignalLength(nodeName);
  data = (float *)malloc(size * sizeof(float));  
  sigDescrData = descr(&dtype_float, data, &size, &null);
  connectionStatus = MdsValue(nodeName, &sigDescrData, &null, 0);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(connectionStatus));
    MdsClose(treeName, &shotNumber);
    MdsDisconnect();
    return nullM;

  }

 
  connectionStatus = MdsClose(treeName, &shotNumber);

  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      gsl_matrix_set(mRet, ii, jj, data[ii+jj*numRows]);
    }
  }
  
  return mRet;

}


/******************************************************************************
 * Function: readDHIMDSplusVector
 * Inputs: int, char *
 * Returns: gsl_matrix*
 * Description: 
 ******************************************************************************/

gsl_vector *readDHIMDSplusVector(int shotNumber, char *nodeName, char *treeName) {

  int connectionStatus,
    connectionID,
    size,
    dtype_float = DTYPE_FLOAT,
    null = 0,
    sigDescrVector,
    ii;
    
  float *vectorData;
  
  gsl_vector *nullVec = 0;
  
  connectionID = MdsConnect("localhost");
  if (connectionID == -1) {
    fprintf(stderr, "Connection Failed\n");
    return nullVec;
  }
  
  connectionStatus = MdsOpen(treeName, &shotNumber);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(connectionStatus));
    MdsDisconnect();
    return nullVec;
  }

  size = getSignalLength(nodeName);
  vectorData = (float *)malloc(size * sizeof(float));  
  sigDescrVector = descr(&dtype_float, vectorData, &size, &null);
  connectionStatus = MdsValue(nodeName, &sigDescrVector, &null, 0);
  if ( !statusOk(connectionStatus) ) {
    fprintf(stderr,"\nError message: %s.\n", MdsGetMsg(connectionStatus));
    MdsClose(treeName, &shotNumber);
    MdsDisconnect();
    return nullVec;

  }

  connectionStatus = MdsClose(treeName, &shotNumber);
  
  gsl_vector *vRet = gsl_vector_alloc(size);  
  connectionStatus = MdsClose(treeName, &shotNumber);
  for (ii = 0; ii < size; ii++) {
    gsl_vector_set(vRet, ii, vectorData[ii]);
  }
  
  return vRet;

}






/******************************************************************************
 *
 * TESTING SECTION
 *
 ******************************************************************************/

int mdsplusReadTest() {

  int shotNumber = 1;
  int numRows = 5,
    numCols = 10,
    ii, jj;

  gsl_matrix *image = gsl_matrix_alloc(numRows, numCols);
  gsl_vector *rVec = gsl_vector_alloc(numRows);
  gsl_vector *zVec = gsl_vector_alloc(numCols);

  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      gsl_matrix_set(image, ii, jj, (double) (ii*10+jj));
    }
  }
  for (jj = 0; jj < numCols; jj++) {
    gsl_vector_set(zVec, jj, jj/50.0);
  }
  for (ii = 0; ii < numRows; ii++) {
    gsl_vector_set(rVec, ii, ii/20.0);
  }
  
  writeDHIMDSplusImage(image, "DHI:LINE_INT:RAW", "$1", shotNumber, "my_tree");
  writeDHIMDSplusVector(rVec, "DHI:LINE_INT:R", "build_signal(build_with_units($1,'m'),,)",
			shotNumber, "my_tree");
  writeDHIMDSplusVector(zVec, "DHI:LINE_INT:Z", "build_signal(build_with_units($1,'m'),,)",
			shotNumber, "my_tree");
  
  gsl_matrix *imageRead = readDHIMDSplusImage(shotNumber, "DHI:LINE_INT", "my_tree");

  printf("Image:\n");
  for (ii = 0; ii < numRows; ii++) {
    for (jj = 0; jj < numCols; jj++) {
      printf("|%g| ", gsl_matrix_get(imageRead, ii, jj));
    }
    printf("\n------------------\n");
  }

  gsl_vector *rVecRead = readDHIMDSplusVector(shotNumber, "DHI:LINE_INT:R", "my_tree");

  printf("r vector:\n");
  for (ii = 0; ii < (rVecRead->size); ii++) {
    printf("%g\n", gsl_vector_get(rVecRead, ii));
    printf("--\n");
  }

  gsl_vector *zVecRead = readDHIMDSplusVector(shotNumber, "DHI:LINE_INT:Z", "my_tree");

  printf("z vector:\n");
  for (ii = 0; ii < (zVecRead->size); ii++) {
    printf("%g\n", gsl_vector_get(zVecRead, ii));
    printf("--\n");
  }

  return 0;
  
}

/**********************************************************************************
To populate this tree:
webertr@fuze2:~/Documents/my_tree$ export my_tree_path=/home/webertr/Documents/my_tree
webertr@fuze2:~/Documents/my_tree$ mdstcl 
TCL> @script

webertr@fuze2:~/Documents/my_tree$ more script
edit my_tree/new
add node .DHI
add node .DHI:LINE_INT/usage=SIGNAL
add node .DHI:LINE_INT:RAW/usage=NUMERIC
add node .DHI:LINE_INT:R/usage=SIGNAL
add node .DHI:LINE_INT:Z/usage=SIGNAL
add node .DHI:NE/usage=SIGNAL
add node .DHI:NE:RAW/usage=NUMERIC
add node .DHI:NE:ERROR/usage=SIGNAL
add node .DHI:NE:ERROR:RAW/usage=NUMERIC
add node .DHI:T/usage=SIGNAL
add node .DHI:T:RAW/usage=NUMERIC
add node .DHI:T:ERROR/usage=SIGNAL
add node .DHI:T:ERROR:RAW/usage=NUMERIC
add node .DHI:BTHETA/usage=SIGNAL
add node .DHI:BTHETA:RAW/usage=NUMERIC
add node .DHI:BTHETA:ERROR/usage=SIGNAL
add node .DHI:BTHETA:ERROR:RAW/usage=NUMERIC
put DHI:LINE_INT "build_signal(build_with_units(DHI:LINE_INT:RAW,'m^-2'),,)"
put DHI:NE "build_signal(build_with_units(DHI:NE:RAW,'m^-3'),,)"
put DHI:NE:ERROR "build_signal(build_with_units(DHI:NE:ERROR:RAW,'m^-3'),,)"
put DHI:T "build_signal(build_with_units(DHI:T:RAW,'eV'),,)"
put DHI:T:ERROR "build_signal(build_with_units(DHI:T:ERROR:RAW,'eV'),,)"
put DHI:BTHETA "build_signal(build_with_units(DHI:BTHETA:RAW,'T'),,)"
put DHI:BTHETA:ERROR "build_signal(build_with_units(DHI:BTHETA:ERROR:RAW,'T'),,)"
write
close
set tree my_tree
create pulse 12345

Then, to run the server:
webertr@fuze2:~/Documents/mdsplusCTest$ export my_tree_path=/home/webertr/Documents/my_tree
webertr@fuze2:~/Documents/mdsplusCTest$ mdsip -p 8000 -m -h mdsip.hosts

with mdsip.hosts:
/O=Grid/O=National Fusion Collaboratory/OU=MIT/CN=Thomas W. Fredian/Email=twf@psfc.mit.edu | twf
* | MAP_TO_LOCAL
* | webertr


******************************************************************************/
