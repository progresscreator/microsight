/**********************************************************************/
/*      Margrit Betke, Image and Video Computing, Spring 2006         */
/*		Revisions by David House, IVC, Summer 2008	      */
/**********************************************************************/


#include "frame.h"

/**********************************************************************/
/*   			OUTPUT FUNCTIONS               	              */
/**********************************************************************/

// Output a color image:
void OutputPpm(rgbframe* image)
{
  int x, y;
  FILE *ofp;  /* ofp = output file pointer  */
    
  ofp = fopen(image->name, "w"); 
  
  //* pnm label for rgb ascii images: pgm */
  fprintf(ofp, "P3\n");  
  fprintf(ofp, "# %s\n", image->name);
 // printf("Output of %s  xdim: %d ydim: %d\n", image->name, 
	// image->xdim, image->ydim);
  fprintf(ofp, "%d %d\n", image->xdim, image->ydim);
  fprintf(ofp, "%d\n", image->highestvalue);
  
  for(y = 0; y < image->ydim; y++) {
    for(x= 0; x < image->xdim; x++) {
      fprintf(ofp,"%d ", image->r[y][x]);
      fprintf(ofp,"%d ", image->g[y][x]);
      fprintf(ofp,"%d ", image->b[y][x]);
    }  
    fprintf(ofp,"\n");
  }  
  fclose(ofp);  
}

/**********************************************************************/
/*   			SCANNING FUNCTIONS                         */
/**********************************************************************/

// GetNextSignificantNumber scans in next number and skips white space
// and comments
double GetNextSignificantNumber(FILE *fp)
{
  
  int ch;
  double num;

  while ((ch = fgetc(fp)) == '\t' || (ch == ' ')
	 || (ch == '\n') || (ch == '#')) {
    if (ch == '#') {
      while ((ch = fgetc(fp)) != '\n')
	;
    }
  }

  ungetc(ch, fp);
  fscanf(fp, "%lf", &num);

  
  return (num);
}

// Input a color image:
void ScanPpm(rgbframe *image)
{
  FILE *inputfp;
  int x, y;
  int i,j;
  int luma=0;
  char ch;
  char buff[300];

  inputfp = fopen(image->name, "rb");
  if(!inputfp) {
    printf("Could not open file %s\n",image->name);
			system("pause");
    exit(1);
  }


  fscanf(inputfp, "%s\n", buff);
  if ((strcmp(buff, "P3") != 0) && (strcmp(buff, "P6") != 0)) {  
    printf("File %s does not start with header P3 or P6\n", image->name);
    printf("Not a Ppm file!\n");
  }
  
  /* Read the dimensions and the highest pixel value of the image: */
  image->xdim =  (int) GetNextSignificantNumber(inputfp);
  image->ydim =  (int) GetNextSignificantNumber(inputfp);
  image->highestvalue =  (int) GetNextSignificantNumber(inputfp);
  image->totalRegions = 0;
  image->totalPixels = (image->xdim * image->ydim);
  image->framenumber = 0;
  image->numPoints = 0;
  image->alpha = 0;
  image->beta = 0;
  image->gamma = 0;

  // Allocate memory space for color pixel values:
  GetImagePpm(image);

 // printf("Scan of %s, xdim = %d, ydim = %d highest = %d\n",
	// image->name, image->xdim, image->ydim,
	// image->highestvalue);

  /* Read the pixel values: */
  if (strcmp(buff, "P3") == 0) {
    for (y = 0; y < image->ydim; y++)
      for(x = 0; x < image->xdim; x++) {
        image->r[y][x] = (unsigned char) GetNextSignificantNumber(inputfp);
        image->g[y][x] = (unsigned char) GetNextSignificantNumber(inputfp);
        image->b[y][x] = (unsigned char) GetNextSignificantNumber(inputfp);
		//image->regionID[x][y] = 0;
		image->isBoundary[x][y] = 0;
		//image->isCentroid[x][y] = 0;
      }

    fscanf(inputfp, "\n");   

  }
  else if (strcmp(buff, "P6") == 0) {
    if ((ch = fgetc(inputfp)) != '\n')  
      ungetc(ch, inputfp);
    for (y = 0; y < image->ydim; y++)
      for(x = 0; x < image->xdim; x++) {
        image->r[y][x] = (unsigned char) fgetc(inputfp);
        image->g[y][x] = (unsigned char) fgetc(inputfp);
        image->b[y][x] = (unsigned char) fgetc(inputfp);
      }  
  }
  image->avgintensity=0;
	for (i=0; i < image->ydim; i++)
		for (j=0; j < image->xdim; j++)
			image->avgintensity += image->r[i][j];

	// Converts all pixels to grayscale (intensity) only
	for (i=0; i < image->ydim; i++)
		for (j=0; j < image->xdim; j++)
		{
			luma = 0.3*image->r[i][j] + 0.59*image->g[i][j] + 0.11*image->b[i][j];
			image->r[i][j]=luma;
			image->g[i][j]=luma;
			image->b[i][j]=luma;
		}

	image->avgintensity = image->avgintensity/(image->ydim*image->xdim);

  fclose(inputfp);
}

// Create an Object Frame
cell::cell(rgbframe *image, int regionID){

	int i,j;
	double area=0.000000;
	int meanX = 0;
	int meanY = 0;
	int localcentroidx=0;
	int localcentroidy=0;
	float localcirc=0.00000;

	int chgx=0;
	int xsmall=INT_MAX;
	int xlarge=INT_MIN;

	int chgy=0;
	int ysmall=INT_MAX;
	int ylarge=INT_MIN;

	// -- Compute Area -- //
	for (i=0; i < image->ydim; i++)
		for (j=0; j < image->xdim; j++)
			if(image->regionID[i][j]==regionID){
				meanY+=i;
				meanX+=j;
				area++;

				if(i<ysmall)
					ysmall=i;
				if(i>ylarge)
					ylarge=i;

				if(j<xsmall)
					xsmall=j;
				if(j>xlarge)
					xlarge=j;
			}

	if(area!=0){
	localcentroidx=(meanX/area);
	localcentroidy=(meanY/area);
	}
	else
		printf("Error: Zero Area in Obj Constructor\n");
	// -- End Area-- //

	// 7-2
	if(sqrt((double)(((localcentroidx-922)*(localcentroidx-922)) + ((localcentroidy-353)*(localcentroidy-353)))) < 30)
		localcentroidx=0;
	if(sqrt((double)(((localcentroidx-1173)*(localcentroidx-1173)) + ((localcentroidy-886)*(localcentroidy-886)))) < 30)
		localcentroidx=0;
	if(sqrt((double)(((localcentroidx-426)*(localcentroidx-426)) + ((localcentroidy-905)*(localcentroidy-905)))) < 30)
		localcentroidx=0;
	if(sqrt((double)(((localcentroidx-462)*(localcentroidx-462)) + ((localcentroidy-936)*(localcentroidy-936)))) < 30)
		localcentroidx=0;
	if(sqrt((double)(((localcentroidx-178)*(localcentroidx-178)) + ((localcentroidy-542)*(localcentroidy-542)))) < 30)
		localcentroidx=0;
	if(sqrt((double)(((localcentroidx-1040)*(localcentroidx-1040)) + ((localcentroidy-64)*(localcentroidy-64)))) < 15)
		localcentroidx=0;
	if(sqrt((double)(((localcentroidx-849)*(localcentroidx-849)) + ((localcentroidy-117)*(localcentroidy-117)))) < 15)
		localcentroidx=0;

	// -- Create Bounding Box -- //
	chgx=(xlarge-xsmall);
	chgy=(ylarge-ysmall);

	startx = localcentroidx - chgx;
	starty = localcentroidy - chgy;

	endx = localcentroidx + chgx;
	endy = localcentroidy + chgy;

	// Check for out-of-bounds
	if(startx<0)
		startx=1;
	if(starty<0)
		starty=1;
	if(endx<0)
		endx=1;
	if(endy<0)
		endy=1;
	if(endx>image->xdim)
		endx=image->xdim-2;
	if(endy>image->ydim)
		endy=image->ydim-2;
	if(startx>image->xdim)
		startx=image->xdim-2;
	if(starty>image->ydim)
		starty=image->ydim-2;

	objectID = regionID;
	xdim = endx - startx;
	ydim = endy - starty;
	// -- End Bounding Box -- //


	// -- Compute Circularity --//
	double r = sqrt(area/PI);
	double t = 0.00000;

	for (i=0; i < image->ydim; i++)
		for (j=0; j < image->xdim; j++)
			if(image->regionID[i][j]==regionID){
				float tmp = (j-localcentroidx)*(j-localcentroidx)+(i-localcentroidy)*(i-localcentroidy);
				if((sqrt(tmp))<r){
					t=t+1;
				}
			}

	if(area!=0)
		localcirc=(t/area);
	
	else
		printf("Error: Zero Area in Obj\n");
	// -- End Circularity --//

	
	// -- Compute Orientation -- //
	double a=0, b=0, c=0;
	for (i = 0; i < image->ydim; i++) {
		for (j = 0; j < image->xdim; j++) {
			if (image->regionID[i][j] == regionID) {
				a += pow((double)(j - localcentroidx), 2);
				b += (j - localcentroidx) * (i - localcentroidy);
				c += pow((double)(i - localcentroidy), 2);
			}
		}
	}
	b = b * 2;

	// Calculate the angle.
	double localalpha = 0;
	if (c - a == 0) {
		printf("Divide by zero error.\n");
	}
	else {
		localalpha = 0.5 * atan(b/(c - a));  //c-a
	}

	g=-localcentroidx*sin(localalpha)+localcentroidy*cos(localalpha);

	localalpha=localalpha*180/PI;
	if(b<0 && localalpha<0)
		localalpha+=-90;
	else if(b>0 && localalpha>0)
		localalpha+=90;
	localalpha=localalpha*PI/180;

	// Save object measurements
	centroidX = localcentroidx;
	centroidY = localcentroidy;
	objArea = area;
	circularity = localcirc;

	alpha = localalpha;

	// Initialize class variables
	actCoreID = -1;
	speed = 0.0;
	persistence = 0.0;
	meanAngle = 0.0;
	angularDeviation = 0.0;
	totalPath = 0.0;
	netPath = 0.0;
	migrateOut = 0;
	migrateIn = 0;
	pickedUp = 0;
	droppedOff = 0;

	// Record global centroid position
	image->isCentroid[centroidY][centroidX] = 1;

	// Test Output
	// cout << endl << "Cx: " << centroidX << "\tCy: " << centroidY << "\tarea: " << area << "\tCirc: " << circularity << endl;
	// cout << "alpha: " << alpha << "\tg: " << g << endl;
	// system("pause");
	// cout << x1 << " : " << y1 << endl;

}


/**********************************************************************/
/*   		MEMORY ALLOCATION FUNCTIONS                         */
/**********************************************************************/

// Allocate memory space for color pixel values:
void GetImagePpm(rgbframe *image)
{

  int x, y;
 
  // Allocate Rows
  for(y=0; y<image->ydim; y++)
	  image->r.push_back(vector <unsigned char>());

  for(y=0; y<image->ydim; y++)
	  image->g.push_back(vector <unsigned char>());

  for(y=0; y<image->ydim; y++)
	  image->b.push_back(vector <unsigned char>());

  for(y=0; y<image->ydim; y++)
	  image->regionID.push_back(vector <unsigned int>());

  for(y=0; y<image->ydim; y++)
	  image->isCentroid.push_back(vector <unsigned int>());

  for(y=0; y<image->ydim; y++)
	  image->isBoundary.push_back(vector <unsigned int>());

  for(y=0; y<image->ydim; y++)
	  image->isACPoint.push_back(vector <unsigned int>());

  for(y=0; y<image->ydim; y++)
	  image->gradientMap.push_back(vector <unsigned int>());

  // Allocate Columns
  for(y=0; y<image->ydim; y++)
  {
	  for(x=0; x<image->xdim; x++){
		image->r[y].push_back(0);
	  }
  }

  for(y=0; y<image->ydim; y++)
	for(x=0; x<image->xdim; x++)
		image->g[y].push_back(0);
	
  for(y=0; y<image->ydim; y++)
	for(x=0; x<image->xdim; x++)
		image->b[y].push_back(0);

  for(y=0; y<image->ydim; y++)
	for(x=0; x<image->xdim; x++)
		image->regionID[y].push_back(0);
	
  for(y=0; y<image->ydim; y++)
	for(x=0; x<image->xdim; x++)
		image->isBoundary[y].push_back(0);
	
  for(y=0; y<image->ydim; y++)
	for(x=0; x<image->xdim; x++)
		image->isCentroid[y].push_back(0);

  for(y=0; y<image->ydim; y++)
	for(x=0; x<image->xdim; x++)
		image->isACPoint[y].push_back(0);

  for(y=0; y<image->ydim; y++)
	for(x=0; x<image->xdim; x++)
		image->gradientMap[y].push_back(0);


 // Initialize Vector
 for(y=0; y<image->ydim; y++)
	for(x=0; x<image->xdim; x++)
		image->r[y][x] = 0;

 for(y=0; y<image->ydim; y++)
	for(x=0; x<image->xdim; x++)
		image->g[y][x] = 0;

 for(y=0; y<image->ydim; y++)
	for(x=0; x<image->xdim; x++)
		image->b[y][x] = 0;

 for(y=0; y<image->ydim; y++)
	for(x=0; x<image->xdim; x++)
		image->regionID[y][x] = 0;

 for(y=0; y<image->ydim; y++)
	for(x=0; x<image->xdim; x++)
		image->isBoundary[y][x] = 0;

 for(y=0; y<image->ydim; y++)
	for(x=0; x<image->xdim; x++)
		image->isCentroid[y][x] = 0;

  for(y=0; y<image->ydim; y++)
	for(x=0; x<image->xdim; x++)
		image->isACPoint[y][x] = 0;

  for(y=0; y<image->ydim; y++)
	for(x=0; x<image->xdim; x++)
		image->gradientMap[y][x] = 0;

}

void FreeFrame(rgbframe *frame){

	for(int y=0; y<frame->ydim; y++)
		for(int x=0; x<frame->xdim; x++){

			frame->isBoundary[y][x] = 0;
			frame->isCentroid[y][x] = 0;
			frame->regionID[y][x] = 0;
			frame->r[y][x] = 0;
			frame->g[y][x] = 0;
			frame->b[y][x] = 0;

			}

	frame->cells.clear();
	frame->totalRegions=0;
	frame->avgintensity=0;
	frame->highestvalue=0;
	frame->totalPixels=0;

}
