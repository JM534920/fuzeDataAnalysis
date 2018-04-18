#ifndef HOLOPARAM_H
#define HOLOPARAM_H


/* 
 * A structure to pass around the necessary parameters for hologram reconstruction and inversion
 */
struct holographyparameters { 
  double res;                 // Resolution of Camera CCD Pixels (3.85E-6 m)
  double lambda;              // Wavelength of laser (532E-9 m)
  double d;                   // Reconstruction distance (0.5 m)
  double deltaN;              // Density offset delta (5E18)
  int numRows;                // Number of rows of image (6016)
  int numCols;                // Number of columns of image (4000)
  int xLL;                    // X/Column number of far left pixel defining first order image
                              // to extract from the holographic reconstruction
  int yLL;                    // Y/Row number of bottom most pixel defining first order image
                              // to extract from the holographic reconstruction
  int xUR;                    // X/Column number of far right pixel defining first order image
                              // to extract from the holographic reconstruction
  int yUR;                    // Y/Row number of top most pixel defining first order image
                              // to extract from the holographic reconstruction
  int hyperbolicWin;          // The fitting parameter for the hyperbolic smooth parameters (8)
  int sampleInterval;         // Sampling interval of line-integrated density (10)
  int centroidNum;            // number of centroids to vary +/- around maximum (10)
  int offsetIter;             // Number of offset iterations (15)
  int boxCarSmoothWidth;      // Box car smoothing width to apply to unwrapped phase (8)
  double unwrapThresh;        // Threshold to trigger the phase unwrapping
  int signTwin;               // Sign to density conversion +/-1. Depends on laser setup (-1)
  double deltaX;              // The distance between column pixels for the current 2d image
  double deltaY;              // The distance between row pixels for the current 2d image
  int debugPhase;             // 1 means save and plot a col profile of the phase and unwrapped phase
  int debugPhaseColNum;       // Col number to save for the phase and unwrapped phase plot
  int debugPhaseRowNum;       // Row number to save for the phase and unwrapped phase plot
  int saveHologram;           // 1 means to save the hologram before extracting twin image
  char fileHologram[100];     // The name of the file for the hologram data
  int plotRadialProfile;      // 1 means to plot the inverted radial profile and slice through
                              // the line integrated image (at plotColNum)
  int invertImage;            // 1 means to perform an inversion
  int plotColNum;             // The column number to plot for the inverted radial profile and
                              // slice of the line integrated image.
  int saveLineIntPos;         // 1 means to save the line integrated data with position info
  char fileLineIntPos[100];   // The name of the file for the line integrated data with positon info
  double R_electrode;         // value of the radius of the inner electrode
  int rotateImage;            // 1 means to rotate the image by 90 degrees CW
  int flipImageRows;          // 1 means to flip the image rows
  int flipImageCols;          // 1 means to flip the images cols
  int refSubtract;            // 1 means to subtract the reference image
  double zPosition;           // The z position of the hologram at the center of the image
  char filePlasma[100];       // The name of the baseline image
  char fileRef[100];          // The name of the plasma image
};

typedef struct holographyparameters holographyParameters;

/*
 * This is the struct that will hold the default values,
 * static means it should only be visible to the source file
 * that includes it, so the compiler won't complain when linking, I think?
 */
static const holographyParameters HOLOGRAPHY_PARAMETERS_DEFAULT = {
  .res = 3.85E-6*100, 
  .lambda = 532E-9*100,
  .d = 0.45*100,
  .deltaN = 1E23/(100*100*100.0),
  .zPosition = 0.145*100,
  .numRows = 4000,
  .numCols = 6016,
  .xLL = 2531,
  .yLL = 2437,
  .xUR = 3356,
  .yUR = 4349,
  .hyperbolicWin = 8,
  .sampleInterval = 10,
  .centroidNum = 10,
  .offsetIter = 10,
  .boxCarSmoothWidth = 10,
  .unwrapThresh = 1.0*M_PI,
  .signTwin = 1,
  .deltaX = 0.000115,
  .deltaY = 0.000115,
  .debugPhase = 0,
  .debugPhaseColNum = 10,
  .debugPhaseRowNum = 61,
  .saveHologram = 0,
  .fileHologram = "data/hologram.dat",
  .plotRadialProfile = 0,
  .invertImage = 0,
  .plotColNum = 100,
  .saveLineIntPos = 1,
  .fileLineIntPos = "data/lineIntegratedPosition.dat",
  .R_electrode = 0.100838,
  .rotateImage = 1,
  .flipImageRows = 0,
  .flipImageCols = 0,
  .refSubtract = 1,
  .fileRef = "/home/webertr/DHI_Images/180215/180215011.JPG",
  .filePlasma = "/home/webertr/DHI_Images/180215/180215012.JPG"
};

/******************************************************************************
 * Example Usage:
 *
 * holographyParameters param = HOLOGRAPHY_PARAMETERS_DEFAULT;
 *
 * param.res = 3.85E-6;             // CCD Resolution
 * param.lambda = 532E-9;           // Wavelength of laser
 * param.d = 0.37;                  // Reconstruction distance
 * param.deltaN = 1E23;             // Density offset delta for inversion
 * param.hyperbolicWin = 8;         // Hyperbolic window parameter
 * param.sampleInterval = 10;       // Sampling interval of line-integrated density 
 * param.centroidNum = 10;          // number of centroids to vary +/- around maximum (10)
 * param.offsetIter = 10;           // Number of offset iterations (15)
 * param.boxCarSmoothWidth = 10;    // Width of box car smoothing on phase
 * param.unwrapThresh = 1.0*M_PI;   // Threshold to trigger the phase unwrapping
 * param.signTwin = 1;              // Sign to density conversion +/-1. Depends on laser setup (-1)
 * param.debugPhase = 0;            // 1 means save and plot a col profile of phase 
 * param.debugPhaseColNum = 10;     // Col number to save for the phase and unwrapped phase
 * param.debugPhaseRowNum = 61;     // Row number to save for the phase and unwrapped phase
 * param.hologramPreview = 0;       // 1 means to preview the hologram before extracting twin image
 * param.invertImage = 0;           // 1 means to invert the image.
 * param.plotRadialProfile = 1;     // 1 means to plot the inverted radial profile and slice throu
 * param.plotColNum = 20;          // Column number to plot for the inverted radial profile and a 
 * param.plotLineIntegrated = 1;    // 1 means to plot the line integrated data
 * param.plotRawHologram = 0;       // 1 means it will plot the raw hologram
 * param.plotRawHologramRow = 100;  // 1 means it will plot a row of the raw hologram
 * param.plotRawHologramCol = 100;  // 1 means it will plot a column of the raw hologram
 * param.plotTwinImage = 0;         // 1 means it will plot a column of the twin image  
 * param.rotateImage = 1;           // 1 means to rotate the image by 90 degrees CW
 * param.flipImageRows = 0;         // 1 means to flip the rows 0 <-> end index
 * param.flipImageCols = 0;         // 1 means to flip the cols 0 <-> end index
 * param.refSubtract = 1;           // 1 means to subtract the reference image
 * param.zPosition = .145;          // Z position of the hologram at the center of the image
 * char *filenameRef = "/home/fuze/DHI_Images/Calibration/DSC_0087.JPG";
 * char *filenamePlasma = "/home/fuze/DHI_Images/Calibration/DSC_0088.JPG";
 *
 * param.xLL = 306;          // Lower left x pixel value of phase reconstruction
 * param.yLL = 1686;          // Lower left y pixel value of phase reconstruction
 * param.xUR = 1300;          // Upper right x pixel value of phase reconstruction
 * param.yUR = 3506;          // Upper right y pixel value of phase reconstruction
 ************************************************************************************************/

#endif
