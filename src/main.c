#include "dataAnalysis.h"


/******************************************************************************
 * Function: main
 * Inputs: int, char**
 * Returns: None
 * Description: 
 ******************************************************************************/

int main(int argc, char *argv[]) {

  /* 
   * struct containing all the holography parameters.
   * Setting to default values.
   */
  dataVector *data = initializeMagneticData(170817005, "\\b_n95_000_sm");

  printf("Data: %d, Delta T: %f\n", data->length, data->deltaT);

  printf("Node Name: %s\n", data->nodeName);

  printf("20th Element: %f\n", data->getElement(data, 20));

  data->saveData(data);

  system("script/plot_test.sh");

  return 0;

}
