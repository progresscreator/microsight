/*
    Copyright 2008 David House

    This file is part of Microsight.

    Microsight is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Microsight is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Microsight.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _SEGCORE_H
#define _SEGCORE_H

#include "frame.h"
#include "equivalence.h"

//Active Contour 
void CreatePoints(rgbframe *frame);
void SobelGradient(rgbframe *frame);
int IterateSnake(rgbframe *frame);
double MeanDistanceofSnakePoints(rgbframe *frame);
void GetPrevNeighbor(rgbframe *frame, int x, int y, int &prevx, int &prevy);
void GetNextNeighbor(rgbframe *frame, int x, int y, int &nextx, int &nexty);
double FindAngleofRotation(int x, int y);
double FindAngleBetweenPoints(int x1, int y1, int x2, int y2);

// GVF
void LocateCells(rgbframe *);
void GVF(vector<vector<float>>&, float, int, vector<vector<float>>&, vector<vector<float>>&);
void BoundMirrorExpand(vector<vector<float>>& f);
void BoundMirrorShrink(vector<vector<float>>& f);
void BoundMirrorEnsure(vector<vector<float>>& f);
void SobelGradient(vector<vector<float>>& f, vector<vector<float>>& fx, vector<vector<float>>& fy);
void DiscreteLaplacian(vector<vector<float>>& f);

// Basic SegCore Functions
void ResolveContrast(rgbframe *frame);
void Binarize(rgbframe *image);

void SISThreshold(rgbframe *image);
void IterativeThreshold(rgbframe *image);

void SobelEdgeDetection_Scaled(rgbframe *image);
void SobelEdgeDetection(rgbframe *image);

void ComponentLabeling(rgbframe *image);
void OrderComponents(rgbframe *image);
void SizeComponents(rgbframe *image);

void ReduceArtifacts(rgbframe *image, int regionsize);
void listRegions(rgbframe *image, int *newcount);

void BoundaryFollow(rgbframe *image);
void MakeBoundary(rgbframe *image, int sx, int sy);
void BoundaryFill(rgbframe *image);

void ProcessFrame(rgbframe *frame);
void FrameObjects(rgbframe *frame);


// Takes in a rgbframe
// Returns a rgbframe-type binary image that has been thresholded
void IterativeThreshold(rgbframe *image){

	int threshold, u1=0, u2=0;
	int prev1=1, prev2=1, c1=0, c2=0;

	threshold = image->avgintensity;
	
	while(u1!=prev1 || u2!=prev2){
		prev1=u1;
		prev2=u2;
		u1=u2=c1=c2=0;
		for (int i=0; i < image->ydim; i++)
			for (int j=0; j < image->xdim; j++) {
				// mark pixel for group1
				if (image->r[i][j]<=threshold) {
					u1+=image->r[i][j];
					c1++;
				}
				// mark pixel for group2
				else if (image->r[i][j]>threshold) {
					u2+=image->r[i][j];
					c2++;
				}
			}
			// calulate the mean gray values u1 and u2
			if(c1==0) u1=0;
			if (c2==0) u2=0;
			else if (c1!=0&&c2!=0) {
				u1=u1/c1;
				u2=u2/c2;
			}
			threshold=(.5*(u1+u2));

			if(verbose){
			printf("the current mean gray value u1 is %d\n",u1);
			printf("the current mean gray value u2 is %d\n",u2);
			printf("the current threshold is %d\n",threshold);
			}
			
	}

	// Perform thresholding on image
		for (int i=0; i < image->ydim; i++)
			for (int j=0; j < image->xdim; j++) {
				if (image->r[i][j]<=threshold){
					image->r[i][j]=0;
					image->g[i][j]=0;
					image->b[i][j]=0;
				}
				else if (image->r[i][j]>threshold){
					image->r[i][j]=255;
					image->g[i][j]=255;
					image->b[i][j]=255;
				}
			}

}

// Takes in a rgbframe
// Returns a rgbframe with edges highlighted 
void SobelEdgeDetection(rgbframe *image){

	int sx,sy,m;
	double max = 0;
	vector<vector<int>> value;

	// -- Allocate Memory for temporary storage --
	for(int y=0; y<image->ydim; y++)
	  value.push_back(vector <int>());
  
	for(int y=0; y<image->ydim; y++)
		for(int x=0; x<image->xdim; x++)
			value[y].push_back(0);

	for (int i=0; i < image->ydim; i++)
		for (int j=0; j < image->xdim; j++)
			value[i][j] = 0;

	// -- Begin Sobel Edge Detection --
	if(verbose)printf("Running Sobel Edge Detection...");
	// Compute the Convolution
	for (int i=1; i<(image->ydim)-1; i++)
		for (int j=1; j<(image->xdim)-1; j++)
	  {
		sx = (double)(image->r[i-1][j+1]+2*image->r[i][j+1]+image->r[i+1][j+1]) -
	        (image->r[i-1][j-1]+2*image->r[i][j-1]+image->r[i+1][j-1]);
	    sy = (double)(image->r[i-1][j-1]+2*image->r[i-1][j]+image->r[i-1][j+1])-
	        (image->r[i+1][j-1]+2*image->r[i+1][j]+image->r[i+1][j+1]);
		  //m = (int)( sqrt( (double)(sx*sx + sy*sy) )/4.0 );
		m = abs(sx)+abs(sy);
		if(m>255)
			m=255;

		value[i][j] = m;
		if(m>max)
			max=m;
	  }

	// Set pixel values equal to convoluted values
	for (int i=0; i < image->ydim; i++)
		for (int j=0; j < image->xdim; j++){
			image->r[i][j] = value[i][j];
			image->g[i][j] = value[i][j];
			image->b[i][j] = value[i][j];
		}
				
	if(verbose) printf(" Ok.\n");

}

// Takes in a rgbframe
// Returns a rgbframe-type binary image that has been thresholded
void SISThreshold(rgbframe *image){
	
	int threshold;
	double Ix, Iy, stat, statTotal = 0, total = 0;
	if(verbose) printf("Running SIS Thresholding...");

	// -- Statistics Gathering --
	for (int i=1; i<(image->ydim)-1; i++)
		for (int j=1; j<(image->xdim)-1; j++){
			Ix = abs(image->r[i+1][j] - image->r[i-1][j]);
			Iy = abs(image->r[i][j+1] - image->r[i][j-1]);
			if (Ix>Iy)
				stat=Ix;
			else
				stat=Iy;
			statTotal += stat;
			total += stat * image->r[i][j];
		}
	
	// -- Determine threshold --
    if(statTotal == 0) 
		threshold = 0;
    else
		threshold = (total / statTotal);

	if(verbose){printf(" Ok.\n");
	printf("Creating Binary Image...");}
	// -- Perform thresholding on image --
	for (int i=0; i < image->ydim; i++)
		for (int j=0; j < image->xdim; j++) {
			if (image->r[i][j]<=threshold){
					image->r[i][j] = 0;
					image->g[i][j] = 0;
					image->b[i][j] = 0;
			}
			else if (image->r[i][j]>threshold){
					image->r[i][j] = 255;
					image->g[i][j] = 255;
					image->b[i][j] = 255;
		}
	}

	if(verbose) printf(" Ok.\n");

}

// Takes in a rgbframe
// Adjusts contrast & brightness based on preset values
// Needed: Method to determine contrast/brightness adjustment automatically
void ResolveContrast(rgbframe *frame){

	if(verbose) printf("Resolving Contrast.\n");

	// Declare dummy image e;
	vector<vector<float>> e;
	// Contrast Scalar
	float contrast_value=3;
	// Brightness Scalar
	int brightness_value=200;

	// Make dummy image e to match frame dimensions
	for(int y=0; y<frame->ydim; y++)
	  e.push_back(vector <float>());

	for(int y=0; y<frame->ydim; y++)
		for(int x=0; x<frame->xdim; x++)
			e[y].push_back(0);

	// The actual contrast/brightness adjustment
	for(int y=0; y<frame->ydim; y++)
		for(int x=0; x<frame->xdim; x++){
			e[y][x] = (float)frame->r[y][x]/255;
			e[y][x]-=.5;
			e[y][x]*=contrast_value;
			e[y][x]+=.5;
			e[y][x]*=255;
			e[y][x]+=brightness_value;
			
			// Clamp
			if(e[y][x]>255)
				e[y][x]=255;
			if(e[y][x]<0)
				e[y][x]=0;

			// Swap in the adjusted pixel
			frame->r[y][x]=e[y][x];
			frame->g[y][x]=e[y][x];
			frame->b[y][x]=e[y][x];
		}

}

// Takes in a rgbframe-type binary image
// Returns a segmented rgbframe of the binary image
// A pixel's region is defined by its regionID
void ComponentLabeling(rgbframe *image){

	int regions = 1;
	equivalence table;

	// Region Memory Allocation
	vector<vector<int> > v;

	for(int y=0; y<image->ydim; y++)
		  v.push_back(vector <int>());

	for(int y=0; y<image->ydim; y++)
		for(int x=0; x<image->xdim; x++)
			v[y].push_back(0);

	for(int y=0; y<image->ydim; y++)
		for(int x=0; x<image->xdim; x++)
			v[y][x] = 0;

	//Begin Processing
	if(verbose) printf("Starting Component Processing.\n");
	for(int y=1; y<image->ydim-1; y++)
		for(int x=1; x<image->xdim-1; x++)
			if(image->r[y][x] == 255){

				// Check Neighbors
				if(v[y+1][x] != 0){

					if(v[y][x] != 0)
						table.add(v[y+1][x],v[y][x]);

					else if (v[y][x]==0)
						v[y][x] = v[y+1][x];
				}

				if(v[y-1][x] != 0){

					if(v[y][x] != 0)
						table.add(v[y-1][x],v[y][x]);

					else if (v[y][x]==0)
						v[y][x] = v[y-1][x];
				}

				if(v[y][x+1] != 0){

					if(v[y][x] != 0)
						table.add(v[y][x+1],v[y][x]);

					else if (v[y][x]==0)
						v[y][x] = v[y][x+1];
				}

				if(v[y][x-1] != 0){

					if(v[y][x] != 0)
						table.add(v[y][x-1],v[y][x]);

					else if (v[y][x]==0)
						v[y][x] = v[y][x-1];
				}


				if(v[y+1][x+1] != 0){

					if(v[y][x] != 0)
						table.add(v[y+1][x+1],v[y][x]);

					else if (v[y][x]==0)
						v[y][x] = v[y+1][x+1];
				}

				if(v[y-1][x-1] != 0){

					if(v[y][x] != 0)
						table.add(v[y-1][x-1],v[y][x]);

					else if (v[y][x]==0)
						v[y][x] = v[y-1][x-1];
				}

				if(v[y-1][x+1] != 0){

					if(v[y][x] != 0)
						table.add(v[y-1][x+1],v[y][x]);

					else if (v[y][x]==0)
						v[y][x] = v[y-1][x+1];
				}

				if(v[y+1][x-1] != 0){

					if(v[y][x] != 0)
						table.add(v[y+1][x-1],v[y][x]);

					else if (v[y][x]==0)
						v[y][x] = v[y+1][x-1];
				}

				if(v[y][x]==0){
					v[y][x] = regions;
					regions++;
				}

			}

	if(verbose) printf("Solving Equivalence Table [...]\n");

	table.solve_Equivalence();

	if(verbose) printf("Solved.\n");

	for(int y=0; y<image->ydim; y++)
		for(int x=0; x<image->xdim; x++)
			if(v[y][x] != 0)
				v[y][x] = table.lookup_Master(v[y][x]);

	for(int y=0; y<image->ydim; y++)
		for(int x=0; x<image->xdim; x++)
			image->regionID[y][x] = v[y][x];

	image->totalRegions = table.master_regions;

	if(verbose) printf("%d Components in Image\n", image->totalRegions);

}

// Takes in a rgbframe
// Sets all regions smaller than desired equal to regionID 0
// Sets all pixels in discarded regions equal to 0
void ReduceArtifacts(rgbframe *image, int regionsize){

	int workingRegion;
	int workingSize;
	int newcount=image->totalRegions;
	if(verbose) printf("Reducing Artifacts...");

	for(int m=0; m < image->ydim; m++){
			for(int n=0; n < image->xdim; n++){
				workingRegion = image->regionID[m][n];
				workingSize = image->RegionSize[workingRegion];
				if(workingSize<regionsize){
					image->regionID[m][n]=0;
					image->r[m][n] = 0;
					image->g[m][n] = 0;
					image->b[m][n] = 0;
				}
			}
	}

	if(verbose) printf(" Ok.\n");
	listRegions(image, &newcount);
	image->totalRegions = newcount;

}

// Takes in a rgbframe
// Normalizes all N object region labels to be in the range [1-N]
void OrderComponents(rgbframe *image){

	int currentRegion;
	int region;
	bool flag = true;
	vector<int> labels;

	// Disregard 0th label
	labels.push_back(0);

	// Push regionIDs into label array
	for(int y=0; y<image->ydim; y++)
		for(int x=0; x<image->xdim; x++){

			if(image->regionID[y][x]!=0){
				
				for(int z=0; z<labels.size(); z++)
					if(labels[z] == image->regionID[y][x])
						flag = false;

				if(flag==true)
					labels.push_back(image->regionID[y][x]);

			}

			flag=true;
		}

	// Apply new labels
		for(int m=0; m<image->ydim; m++)
			for(int n=0; n<image->xdim; n++)
				if(image->regionID[m][n]!=0){

					for(int y=1; y<labels.size(); y++)
						if(labels[y]==image->regionID[m][n])
							image->regionID[m][n] = y;
				}

		for(int m=0; m<image->ydim; m++)
			for(int n=0; n<image->xdim; n++)
				if(image->regionID[m][n]>image->totalRegions)
					printf("!!! %d", image->regionID[m][n]);

}

// Takes in a rgbframe
// Determines the size of each object in frame
// Stores size calculations into RegionSize map
void SizeComponents(rgbframe *image){

	for(int y=0; y<image->ydim; y++)
		for(int x=0; x<image->xdim; x++)
			if(image->regionID[y][x]!=0)
				image->RegionSize[image->regionID[y][x]]++;

}

// Takes in a rgbframe
// Lists the current number of regions in frame
// Sends the region count back as newcount
void listRegions(rgbframe *image, int *newcount){

	int count=0;
	int currentRegion;
	vector<int> deadRegions;
	
	// -- Memory Allocation --
	for(int y=0; y<image->totalRegions+1; y++)
		deadRegions.push_back(0);

		for(int m=0; m < image->ydim; m++){
			for(int n=0; n < image->xdim; n++){
				if(image->regionID[m][n]!=0 && deadRegions[image->regionID[m][n]]!=1){
					currentRegion = image->regionID[m][n];
					count++;
					deadRegions[image->regionID[m][n]] = 1;
				}
			}
	}

		if(verbose) printf("%d Regions in image.\n", count);
		newcount=(int*)count;
}

// Takes in a rgbframe
// Performs edge detection & thresholding
// Returns a rgbframe-type binary image
void Binarize(rgbframe *image){

	SobelEdgeDetection(image);
	SISThreshold(image);

}

// Input: A segmented and labeled rgbframe
// Output: A rgbframe with boundary pixels marked
// Extraneously, boundary pixels currently set to 255 for debugging
void BoundaryFollow(rgbframe *image){

	int sx=0, sy=0;
	int thisRegion = 0;
	vector<int> deadRegions;

	// -- Memory Allocation --
	for(int i=0;i<image->totalRegions+1; i++)
		deadRegions.push_back(0);

	// -- Prep Image for Boundary Highlighting --
	for(int i=0; i<image->ydim; i++)
		for(int j=0; j<image->xdim; j++){
			image->r[i][j] = 0;
			image->g[i][j] = 0;
			image->b[i][j] = 0;
		}

	if(verbose) printf("Tracing Boundaries...");
	// -- Begin Pixel Selection --
	for(int i=1; i<(image->ydim-1); i++)
		for(int j=1; j<(image->xdim-1); j++){
			thisRegion = image->regionID[i][j];

			if(thisRegion != 0 && deadRegions[thisRegion] != 1){
				if(image->regionID[i-1][j-1]!=thisRegion && image->regionID[i-1][j]!=thisRegion && image->regionID
					[i][j-1]!=thisRegion){
						// We've found a top left pixel...
						sx=i;
						sy=j;
						// Perform Algorithm on chosen pixel
						MakeBoundary(image,sx,sy);
				}
			}

			deadRegions[thisRegion] = 1;
			thisRegion = 0;
		}

		if(verbose) printf(" Ok.\n");
}

void MakeBoundary(rgbframe *image, int sx, int sy){
	
	int cx, cy;
	int n[9];
	int thisRegion = image->regionID[sx][sy];
	int flag = 1;
	int start=1;
	int id = 0;
	int stopcondition = 0;

	cx=sx, cy=sy;

	image->r[cx][cy] = 255;
	image->g[cx][cy] = 255;
	image->b[cx][cy] = 255;
	image->isBoundary[cx][cy] = 1;

	while((cx!=sx || cy!=sy)  || (flag == 1)){
		flag=0;
		stopcondition++;
		
		n[0] = 0;
		n[1] = image->regionID[cx][cy-1];
		n[2] = image->regionID[cx-1][cy-1];
		n[3] = image->regionID[cx-1][cy];
		n[4] = image->regionID[cx-1][cy+1];
		n[5] = image->regionID[cx][cy+1];
		n[6] = image->regionID[cx+1][cy+1];
		n[7] = image->regionID[cx+1][cy];
		n[8] = image->regionID[cx+1][cy-1];

		// Starts at b, loops through all n's clockwise
		// id = which ni we should set c equal to
		if( n[start%9] == thisRegion)
			id = start;
		else if( n[(start+1)%9] == thisRegion)
			id = (start+1)%9;
		else if( n[(start+2)%9] == thisRegion)
			id = (start+2)%9;
		else if( n[(start+3)%9] == thisRegion)
			id = (start+3)%9;
		else if( n[(start+4)%9] == thisRegion)
			id = (start+4)%9;
		else if( n[(start+5)%9] == thisRegion)
			id = (start+5)%9;
		else if( n[(start+6)%9] == thisRegion)
			id = (start+6)%9;
		else if( n[(start+7)%9] == thisRegion)
			id = (start+7)%9;
		else if( n[(start+8)%9] == thisRegion)
			id = (start+8)%9;

		if(id == 1)
		{
			cx=cx;
			cy=cy-1;
			start = 7;
		}

		else if(id == 2)
		{
			cx=cx-1;
			cy=cy-1;
			start = 7;
		}

		else if(id == 3)
		{
			cx=cx-1;
			cy=cy;
			start = 1;
		}

		else if(id == 4)
		{
			cx=cx-1;
			cy=cy+1;
			start = 1;
		}

		else if(id == 5)
		{
			cx=cx;
			cy=cy+1;
			start = 3;
		}

		else if(id == 6)
		{
			cx=cx+1;
			cy=cy+1;
			start = 3;
		}

		else if(id == 7)
		{
			cx=cx+1;
			cy=cy;
			start = 5;
		}

		else if(id == 8)
		{
			cx=cx+1;
			cy=cy-1;
			start = 5;
		}

		image->r[cx][cy] = 255;
		image->g[cx][cy] = 255;
		image->b[cx][cy] = 255;
		image->isBoundary[cx][cy] = 1;
			
		if(stopcondition==20000){
			printf("Error in Boundary Following: Possible Infinite Loop.\n");
			break;
		}

	}

}

// Input: rgbframe with boundaries marked
// Output: rgbframe with regionIDs updated to include interior-object pixels
void BoundaryFill(rgbframe *image){

	int score;
	int maRegion=0;

	if(verbose) printf("Filling Objects...\n");
	for(int i=1; i<(image->ydim-1); i++)
		for(int j=1; j<(image->xdim-1); j++)
			if(image->regionID[i][j]==0)
			{
				score = 0;
				if((image->regionID[i-1][j]!=0)&&(image->isBoundary[i-1][j]==0)){
					score++; maRegion=image->regionID[i-1][j]; }
				if((image->regionID[i+1][j]!=0)&&(image->isBoundary[i+1][j]==0)){
					score++; maRegion=image->regionID[i+1][j]; }
				if((image->regionID[i][j-1]!=0)&&(image->isBoundary[i][j-1]==0)){
					score++; maRegion=image->regionID[i][j-1]; }
				if((image->regionID[i][j+1]!=0)&&(image->isBoundary[i][j+1]==0)){
					score++; maRegion=image->regionID[i][j+1]; }

				if(score>=2)
					image->regionID[i][j]=maRegion;
			}

	if(verbose) printf("Ok.\n");
}

// Input: rgbframe
// Output: original rgbframe with objects identified and labeled
void ProcessFrame(rgbframe *frame){

  // ===-----=== Memory Allocation ===-----=== //
  vector<vector<unsigned char> > Phantom_Original;

  for(int y=0; y<frame->ydim; y++)
	  Phantom_Original.push_back(vector <unsigned char>());

  for(int y=0; y<frame->ydim; y++)
	for(int x=0; x<frame->xdim; x++)
		Phantom_Original[y].push_back(0);
  


  // ===-----=== Image Preprocessing ===-----=== //
  // Contrast Adjustment
 //  ResolveContrast(frame);

  // Store Original Intensity Values
  for(int x=0; x<frame->ydim; x++)
	  for(int y=0; y<frame->xdim; y++)
		  Phantom_Original[x][y] = frame->r[x][y];
  
  
    // ===-----=== Perform Image Processing ===-----=== //
	Binarize(frame);

	ComponentLabeling(frame);
	OrderComponents(frame);
	SizeComponents(frame);

	ReduceArtifacts(frame, 25);

	// Hax Routine
	// BoundaryFollow doesn't function correctly if there are pixels with similar regionIDs
	// that are unconnected in the image. Such pixels pop up in very small groups as artifacts of the
	// labeling process. This hack gets rid of those erroneous pixels by labelling the image a second time.
	// Hey, it works. You might want to rewrite my BoundaryFollow algorithm to rid yourself of this hack.
	// Until then, he's gonna sit right here and help us out.
	//***********************************
	for(int y=0; y<frame->ydim; y++)
		for(int x=0; x<frame->xdim; x++){
			if(frame->regionID[y][x]!=0)
				frame->r[y][x]=frame->g[y][x]=frame->b[y][x]=255;
			else
				frame->r[y][x]=frame->g[y][x]=frame->b[y][x]=0;

			frame->regionID[y][x]=0;
		}

	for(int y=0; y<MAXREGIONS; y++)
		frame->RegionSize[y]=0;

	ComponentLabeling(frame);
	OrderComponents(frame);
	SizeComponents(frame);

	ReduceArtifacts(frame, 250);
	//***********************************

	BoundaryFollow(frame);
	BoundaryFill(frame);



  // ===-----=== Image Post-processing ===-----=== //
  // Restore Original Intensity
  for(int x=0; x<frame->ydim; x++)
	  for(int y=0; y<frame->xdim; y++){
		frame->r[x][y] = Phantom_Original[x][y]; 
		frame->g[x][y] = Phantom_Original[x][y];  
		frame->b[x][y] = Phantom_Original[x][y]; 
	  }

}


// -----===-----===-----===-----===-----===-----===-----===-----= //
// ===-----===-----=== Object-Frame Functions ===-----===-----=== //
// -----===-----===-----===-----===-----===-----===-----===-----= //

// Input: a rgbframe with object regions
// Creates cells from these regions
// Ouput: a rgbframe with corresponding list of cells
void FrameObjects(rgbframe *frame){

	int i,j;
	int thisRegion = 0;
	vector<int> deadRegions;
	int count=0;

	// -- Memory Allocation -- //
	for(i=0; i<frame->totalRegions+1; i++)
		deadRegions.push_back(0);

	if(verbose) printf("Processing Objects...\n");
	// -- Begin Object Selection --
	for(i=0; i<frame->ydim; i++)
		for(j=0; j<frame->xdim; j++){
			thisRegion = frame->regionID[i][j];

			// Find a new object
			if(thisRegion != 0 && deadRegions[thisRegion] != 1){
				
				// We've found a new object...
				count++;
				// Create frame around object
				if(verbose) printf("Framing Region %d\n", thisRegion);
				cell obj(frame, thisRegion);
				if(obj.centroidX!=0)
				frame->cells.push_back(obj);
			}

			deadRegions[thisRegion] = 1;
			thisRegion = 0;
		}

		if(verbose) printf(" Ok.\n");

}

void CreatePoints(rgbframe *frame){

	int least_x = INT_MAX;
	int most_x = INT_MIN;
	int least_y = INT_MAX;
	int most_y = INT_MIN;
	int xdiff = 0;
	int ydiff = 0;
	bool modIt = false;
	int boundary_count = 0;
	// Find min, max boundary points from centroid
	// And count # of boundary pixels
	for(int i=0; i<frame->ydim; i++)
		for(int j=0; j<frame->xdim; j++)
		{
			if(frame->isBoundary[i][j]==1){

				boundary_count++;
				if(i<least_x)
					least_x=i;
				if(i>most_x)
					most_x=i;
				if(j<least_y)
					least_y=j;
				if(j>most_y)
					most_y=j;

			}
		}

	// Set cell extremeties as AC points
		/*
	frame->isACPoint[least_x][least_y] = 1;
	frame->isACPoint[least_x][most_y] = 1;
	frame->isACPoint[most_x][most_y] = 1;
	frame->isACPoint[most_x][least_y] = 1;
	*/

	if(boundary_count > 200)
		modIt = true;

	// If it's a boundary point...
	for(int i=0; i<frame->ydim; i++)
		for(int j=0; j<frame->xdim; j++)
		{
			if(frame->isBoundary[i][j]==1){

				// If the # of points exceeds 200
				if(modIt){

					// Weed out some points
					if(i % 2 == 0 && j % 2 == 0){
						frame->isACPoint[i][j] = 1;
						frame->numPoints++;
					}
				}

				// Else, turn it into an AC Point
				else{
					frame->isACPoint[i][j] = 1;
					frame->numPoints++;
				}

			}

		}

}


void SobelGradient(rgbframe *frame){

	// ===-----=== Memory Allocation ===-----=== //
  vector<vector<unsigned char> > Phantom_Original;

  for(int y=0; y<frame->ydim; y++)
	  Phantom_Original.push_back(vector <unsigned char>());

  for(int y=0; y<frame->ydim; y++)
	for(int x=0; x<frame->xdim; x++)
		Phantom_Original[y].push_back(0);

	// ===-----=== Image Preprocessing ===-----=== //

    // Store Original Intensity Values
    for(int x=0; x<frame->ydim; x++)
	  for(int y=0; y<frame->xdim; y++)
		  Phantom_Original[x][y] = frame->r[x][y];

	// ===-------===-===-------===-===-------=== //


	// Compute Gradient Magnitude
	double gradX, gradY;
		
	for(int i = 0; i < frame->ydim; i++) {
		for(int j = 0; j < frame->xdim; j++) {
			// if on edge of image, set gradient to 0
			if (i == 0 || j == 0 || i == frame->ydim-1 || j == frame->xdim-1) {
					frame->gradientMap[i][j] = 0;
				}
				// otherwise calculate with sobel kernel
				else {

					gradX = 0;
					gradY = 0;

					gradX += Phantom_Original[i-1][j-1];
					gradX += 2*Phantom_Original[i-1][j];
					gradX += Phantom_Original[i-1][j+1];
					gradX -= Phantom_Original[i+1][j-1];
					gradX -= 2*Phantom_Original[i+1][j];
					gradX -= Phantom_Original[i+1][j+1];

					gradY += Phantom_Original[i-1][j-1];
					gradY += 2*Phantom_Original[i][j-1];
					gradY += Phantom_Original[i+1][j-1];
					gradY -= Phantom_Original[i-1][j+1];
					gradY -= 2*Phantom_Original[i][j+1];
					gradY -= Phantom_Original[i+1][j+1];

					frame->gradientMap[i][j] = (int) sqrt((gradX * gradX)+(gradY * gradY));
				}
			}
		}


}

double MeanDistanceofSnakePoints(rgbframe *frame)
{

	// ===-----=== Memory Allocation ===-----=== //
  vector<vector<int> > touched;

  for(int y=0; y<frame->ydim; y++)
	  touched.push_back(vector <int>());

  for(int y=0; y<frame->ydim; y++)
	for(int x=0; x<frame->xdim; x++)
		touched[y].push_back(0);


	double sum = 0.0;
	double distance = 0.0;
	int pointNumber = 0;
	
	if(frame->numPoints==0)
		return -1;


	int short_ed = INT_MAX;
	int indexX = 0;
	int indexY = 0;
	for(int i=0; i<frame->ydim; i++)
		for(int j=0; j<frame->xdim; j++){

			// For every untouched point
			if(touched[i][j]==0 && frame->isACPoint[i][j]==1){

				// Find the neighbor point
				for(int m=0; m<frame->ydim; m++)
					for(int n=0; n<frame->xdim; n++)
						if(frame->isACPoint[m][n]==1 && (m!=i && n!=j)){

							float euclidean_dist = sqrt((float)(m-i)*(m-i) + (n-j)*(n-j));

							if(euclidean_dist < short_ed){
								short_ed = euclidean_dist;
								indexX = n;
								indexY = m;
							}
						}

				// Neighbor Point is now stored in [indexY][indexX]
				// Euclidean dist to neighbor is now stored in short_ed

				sum += short_ed;
				touched[i][j] = 1;

			}



		}

	float avg = sum/frame->numPoints;

	return avg;
}

double FindAngleofRotation(int x, int y)
{
	double a;

	if(!x || !y)
		return 0;

	a=(double)acos(x/sqrt(double(x*x+y*y)));

	if(y<0)
		a=2*PI-a;
	a = a*180/PI;

	return a;
}

double FindAngleBetweenPoints(int x1, int y1, int x2, int y2)
{
	double angle1;
	double angle2;
	double ans;

	angle1=FindAngleofRotation(x1, y1);

	angle2=FindAngleofRotation(x2, y2);

	if(abs(angle2-angle1)>180)
		ans = 360-abs(angle2-angle1);
	else
		ans = fabs(angle2-angle1);

	return ans;
}

int IterateSnake(rgbframe *frame)
{

	// ===-----=== Memory Allocation ===-----=== //
	vector<vector<int> > touched;

	for(int y=0; y<frame->ydim; y++)
	  touched.push_back(vector <int>());

	for(int y=0; y<frame->ydim; y++)
		for(int x=0; x<frame->xdim; x++)
			touched[y].push_back(0);


	int moved_points = 0;
	int x,y,x0,y0,x1,x2,x3,y1,y2,y3;
	int i,j,i1,j1;
	double a,b,r;
	double E[3][3];
	double Eout[3][3];
	double Curve[3][3];
	double maxCurve;
	double minCurve;
	double maxEout;
	double Energy[3][3];
	double maxEnergy;
	double minEnergy;
	double C_j,C_prev,C_next;
    double avg_dist = 0.0;
	double ux,uy,u1x,u1y,u_bar,u1_bar;
	int prevx, prevy, nextx, nexty;


	// Compute Average Distance for all snake points
	avg_dist=MeanDistanceofSnakePoints(frame);

	// For all untouched snake points...
	for(int m=3; m<frame->ydim-3; m++)
		for(int n=3; n<frame->xdim-3; n++){

			if(touched[m][n]==0 && frame->isACPoint[m][n]==1){

				// Store the x,y position for this snake point
				x=m;
				y=n;

				// Set origin point
				x0=x-1;
				y0=y-1;


				// Compute Image Energy
				for (i=0; i<3; i++)
					for (j=0; j<3; j++)
						Energy[i][j]=pow((float)(frame->gradientMap[x0+j][y0+i]),2);

				// Normalize Image Energy
				maxEnergy = Energy[0][0];
				minEnergy = Energy[0][0];


				// Compute Max Energy
				for (i=0; i<3; i++)
					for (j=0; j<3; j++)
					{
						if (Energy[i][j]>maxEnergy)
							maxEnergy = Energy[i][j];                            
					}

				// Compute Min Energy
				for (i=0; i<3; i++)
					for (j=0; j<3; j++)
					{
    					if (Energy[i][j]<minEnergy)
							minEnergy=Energy[i][j];                            
					}

				// Normalize
				for (i=0; i<3; i++)
					for (j=0; j<3; j++)
					{
						if ((minEnergy - maxEnergy)==0)                          
							Energy[i][j]=0;
						else
							Energy[i][j] = (minEnergy-Energy[i][j]) / (minEnergy-maxEnergy);
					}


				// Get Neighbor
				GetPrevNeighbor(frame,x,y,prevx,prevy);
				// Get Neighbor
				GetNextNeighbor(frame,x,y,nextx,nexty);

				// Compute Curve
				for (i=0; i<3; i++)
					for (j=0; j<3; j++)
						Curve[i][j]=pow((float)((prevx)-(2*(x0+j))+(nextx)),2)+pow((float)((prevy)-(2*(y0+i))+(nexty)),2);


				// Normalize
				maxCurve=Energy[0][0];

				for (i=0; i<3; i++)
					for (j=0; j<3; j++)
					{
						if (Curve[i][j]>maxCurve)
							maxCurve=Curve[i][j];
					}

				for (i=0; i<3; i++)
					for (j=0; j<3; j++)
						Curve[i][j]=Curve[i][j]/maxCurve;


				// Compute Eout
				for (i=0; i<3; i++)
					for (j=0; j<3; j++)
						Eout[i][j]=avg_dist-sqrt((float)((x0+j)-prevx)*((x0+j)-prevx)+((y0+i)-prevy)*((y0+i)-prevy));

				for (i=0; i<3; i++)
					for (j=0; j<3; j++)
					{
						if (Eout[i][j]<0)
						Eout[i][j]=0-Eout[i][j];
					}


				// Normalize
				maxEout=Eout[0][0];

				for (i=0; i<3; i++)
					for (j=0; j<3; j++)
					{
						if (Eout[i][j]>maxEout)
							maxEout=Eout[i][j];
					}

				for (i=0; i<3; i++)
					for (j=0; j<3; j++)
						Eout[i][j]=Eout[i][j]/maxEout;


				// Compute Energy Function
				a=frame->alpha;
				b=frame->beta;
				r=frame->gamma;

				for (i=0; i<3; i++)
					for (j=0; j<3; j++)
					{
						E[i][j]=(a*Eout[i][j])+(b*Curve[i][j])+(r*Energy[i][j]);
					}

				// Find the minimum of the Enery function.
				// Move the snake point move to that position.
				minEnergy=E[1][1];
				i1=1;
				j1=1;

				for (i=0; i<3; i++)
					for (j=0; j<3; j++)
					{
						if (E[i][j] < minEnergy)
						{
							minEnergy=E[i][j];
							i1=i;
							j1=j;
						}
					}

					// Move the point
					frame->isACPoint[x0+j1][y0+i1] = 1;
					frame->isACPoint[m][n] = 0;
					touched[x0+j1][y0+i1] = 1;
					touched[m][n] = 1;
					moved_points++;

			}
	}




	// After all points have been touched...
	// Set all points as untouched.
	for(int m=0; m<frame->ydim; m++)
		for(int n=0; n<frame->xdim; n++)
			touched[m][n] = 0;


	// For all untouched snake points...
	for(int m=3; m<frame->ydim-3; m++)
		for(int n=3; n<frame->xdim-3; n++){

			if(touched[m][n]==0 && frame->isACPoint[m][n]==1){

				// Find three snake points in a row: j-1,j,j+1
				GetPrevNeighbor(frame,m,n,prevx,prevy);
				GetNextNeighbor(frame,m,n,nextx,nexty);

				// For the j-1 point:
				x1=prevx;    
				x2=n;
				x3=nextx;
				y1=prevy;    
				y2=m;
				y3=nexty;

				// Calculate Ui
				ux= x2 - x1;
				uy= y2 - y1;

				// Calculate U(i+1)
				u1x = x3 - x2;         
				u1y = y3 - y2;

				// Calculate length of Ui and U(i+1)
				u_bar=sqrt(pow(ux,2)+pow(uy,2));            
				u1_bar=sqrt(pow(u1x,2)+pow(u1y,2));         

				// Calculate curvature of j-1
				C_prev=pow(((ux/u_bar)-(u1x/u1_bar)),2)+pow(((uy/u_bar)-(u1y/u1_bar)),2);    


				// for the jth point:
				x1=prevx;
				x2=n;
				x3=nextx;
				y1=prevy;
				y2=m;
				y3=nexty;

				ux=x2-x1;
				uy=y2-y1;

				u1x=x3-x2;
				u1y=y3-y2;

				u_bar=sqrt(pow(ux,2)+pow(uy,2));
				u1_bar=sqrt(pow(u1x,2)+pow(u1y,2));

				C_j=pow(((ux/u_bar)-(u1x/u1_bar)),2)+pow(((uy/u_bar)-(u1y/u1_bar)),2);     

				/// for the j+1 snake point:               
				x1=prevx;
				x2=x;
				x3=nextx;
				y1=prevy;
				y2=y;
				y3=nexty;

				ux=x2-x1;
				uy=y2-y1;
				u1x=x3-x2;
				u1y=y3-y2;

				u_bar=sqrt(pow(ux,2)+pow(uy,2));
				u1_bar=sqrt(pow(u1x,2)+pow(u1y,2));

				C_next=pow(((ux/u_bar)-(u1x/u1_bar)),2)+pow(((uy/u_bar)-(u1y/u1_bar)),2);

				touched[m][n] = touched[nexty][nextx] = touched[prevy][prevx] = 1;

			}
		}

	return moved_points;
}

void GetPrevNeighbor(rgbframe *frame, int x, int y, int &prevx, int &prevy){

	float short_ed = INT_MAX;
	int indexX = 0;
	int indexY = 0;

	// Find the prev neighbor point
			for(int m=0; m<frame->ydim; m++)
				for(int n=0; n<frame->xdim; n++)
					if(frame->isACPoint[m][n]==1 && (m!=y && n!=x)){

						float euclidean_dist = sqrt((float)(m-y)*(m-y) + (n-x)*(n-x));

						if(euclidean_dist < short_ed){
							short_ed = euclidean_dist;
							indexX = n;
							indexY = m;
							}
						}

			prevx = indexX;
			prevy = indexY;
}

void GetNextNeighbor(rgbframe *frame, int x, int y, int &nextx, int &nexty){

	float short_ed = INT_MAX;
	int indexX = 0;
	int indexY = 0;

	// Find the next neighbor point
			for(int m=0; m<frame->ydim; m++)
				for(int n=0; n<frame->xdim; n++)
					if(frame->isACPoint[m][n]==1 && (m!=y && n!=x)){

						float euclidean_dist = sqrt((float)(m-y)*(m-y) + (n-x)*(n-x));

						if(euclidean_dist < short_ed){
							short_ed = euclidean_dist;
							indexX = n;
							indexY = m;
							}
						}

			nextx = indexX;
			nexty = indexY;
}

void LocateCells(rgbframe *image){

	float mu = 0.2;
	int iter = 1;
	vector<vector<float>> f;
	vector<vector<float>> u;
	vector<vector<float>> v;
	vector<vector<float>> Mag;

	// -- Allocate Temporary Matrix
	for(int y=0; y<image->ydim; y++)
	  f.push_back(vector <float>());
  
	for(int y=0; y<image->ydim; y++)
		for(int x=0; x<image->xdim; x++)
			f[y].push_back(0);

	// -- Save RGB Image into Matrix
	for (int i=0; i <image->ydim; i++)
		for (int j=0; j < image->xdim; j++)
			f[i][j] = image->r[i][j];

	// -- Normalize to [0,1]
	for(int i=0; i <f.size(); i++)
		for (int j=0; j < f[0].size(); j++)
			f[i][j] = ((( (double)(f[i][j]) )/(double)255));

	// -- Compute Negative Edge Map
	for(int i=0; i <f.size(); i++)
		for (int j=0; j < f[0].size(); j++)
			f[i][j] = abs(1 - f[i][j]);

	// -- Compute the Gradient Vector Field of the Edge Map
	GVF(f, mu, iter, u, v);

		
	// -- Compute the Magnitude of the Gradient Vector Field
	Mag = f;
	for(int i=0; i <f.size(); i++)
		for (int j=0; j < f[0].size(); j++)
			Mag[i][j] = sqrt((double)(u[i][j]*u[i][j]) + (double)(v[i][j]*v[i][j]));


	// -- Normalize the External Forces of the Gradient Vector Field
	for(int i=0; i <u.size(); i++)
		for (int j=0; j < u[0].size(); j++){
			u[i][j] = u[i][j] / ( ( (float) ((float)Mag[i][j] + 1e-10)));
			v[i][j] = v[i][j] / ( ( (float) ((float)Mag[i][j] + 1e-10)));
		}


	// -- Scale Image to [0..255]
	for(int i=0; i <f.size(); i++)
		for (int j=0; j < f[0].size(); j++)
			f[i][j] = ((( (double)((u[i][j] + v[i][j])) )*(double)255));



	cout << "Restoring Output Image" << endl;
	// -- Restore RGB Image from Matrix
	for (int i=0; i <image->ydim; i++)
		for (int j=0; j < image->xdim; j++)
			image->r[i][j] = image->g[i][j] = image->b[i][j] = f[i][j];


}


void GVF(vector<vector<float>>& f, float mu, int iter, vector<vector<float>>& u, vector<vector<float>>& v){


	// -- Handle Boundary Condition
	BoundMirrorExpand(f);

	// -- Compute Gradient of Edge Map
	vector<vector<float>> fx;
	vector<vector<float>> fy;
	SobelGradient(f, fx, fy);

	// -- Initialize u, v to Image Gradient
	u = fx;
	v = fy;

	// -- Allocate Squared Magnitude Matrix
	vector<vector<float>> SqMag;
	SqMag = f;

cout << "Computing SqMag of GVF" << endl;
	// -- Compute Squared Magnitude of Gradient Field
	for(int i=0; i <f.size(); i++)
		for (int j=0; j < f[0].size(); j++)
			SqMag[i][j] = (fx[i][j]*fx[i][j]) + (fy[i][j]*fy[i][j]);

cout << "Entering Iterative Solution" << endl;
	// -- Iterative Solution to GVF
	for(int i=0; i<iter; i++){

		cout << "Ensuring Boundary Expansion U" << endl;
		BoundMirrorEnsure(u);
		cout << "Ensuring Boundary Expansion V" << endl;
		BoundMirrorEnsure(v);

		cout << "Copying U,V" << endl;

		// -- Copy U,V
		vector<vector<float>> laplaceU;
		vector<vector<float>> laplaceV;
		vector<vector<float>> differenceU;
		vector<vector<float>> differenceV;
		laplaceU = differenceU = u;
		laplaceV = differenceV = v;

		cout << "Performing Discrete Laplacian" << endl;

		// -- Perform Discrete Laplacian
		DiscreteLaplacian(laplaceU);
		DiscreteLaplacian(laplaceV);

		cout << "Computing Difference of GVF, Gradient" << endl;

		// -- Compute Difference of GVF, Gradient
		for(int j=0; j <fx.size(); j++)
			for (int k=0; k < fx[0].size(); k++){
				differenceU[j][k] = differenceU[j][k] - fx[j][k];
				differenceV[j][k] = differenceV[j][k] - fy[j][k];
			}

		cout << "Computing Square Magnitude * difference" << endl;

		// -- Compute Square Magnitude * (differenceU|V - fx|y)
		for(int j=0; j <fx.size(); j++)
			for (int k=0; k < fx[0].size(); k++){
				differenceU[j][k] = differenceU[j][k] * SqMag[j][k];
				differenceV[j][k] = differenceV[j][k] * SqMag[j][k];
			}

		cout << "Computing u,v" << endl;

		// -- Compute u,v
		for(int j=0; j <u.size(); j++)
			for (int k=0; k < u[0].size(); k++){
				u[j][k] = u[j][k] + (mu * 4 * ((float)laplaceU[j][k])) - differenceU[j][k];
				v[j][k] = v[j][k] + (mu * 4 * ((float)laplaceV[j][k])) - differenceV[j][k];
			}

			cout << "ITER: " << i+1 << " of " << iter << endl;

			laplaceU.clear();
			laplaceV.clear();
			differenceU.clear();
			differenceV.clear();

	}


	// -- Cleanup
	BoundMirrorShrink(f);
	BoundMirrorShrink(u);
	BoundMirrorShrink(v);

}

void BoundMirrorExpand(vector<vector<float>>& f){

	int fxdim = f[0].size();
	int fydim = f.size();
	vector<vector<float>> b;

	// -- Create an empty matrix b of dimension xdim+2, ydim+2
	
	for(int y=0; y<fydim+2; y++)
	  b.push_back(vector <float>());
  
	for(int y=0; y<fydim+2; y++)
		for(int x=0; x<fxdim+2; x++)
			b[y].push_back(0);

	for (int i=0; i < fydim+2; i++)
		for (int j=0; j < fxdim+2; j++)
			b[i][j] = 0;

   // -- Create border of zeros around f; store in b
	for(int yi = 1; yi<fydim+1; yi++)
		for(int xi = 1; xi<fxdim+1; xi++)
			b[yi][xi] = f[yi-1][xi-1];

	// Mirror corners	
	b[0][0] = b[2][2];
	b[0][fxdim+1] = b[2][fxdim-1];
	b[fydim+1][0] = b[fydim-1][2];
	b[fydim+1][fxdim+1] = b[fydim-1][fxdim-1]; 

	// -- Mirror Top
	for(int xi = 1; xi<fxdim+1; xi++)
		b[0][xi] = b[2][xi];

	// -- Mirror Bottom	
	for(int xi = 1; xi<fxdim+1; xi++)
		b[fydim+1][xi] = b[fydim-1][xi];

	// -- Mirror Left
	for(int yi = 1; yi<fydim+1; yi++)
		b[yi][0] = b[yi][2];

	// -- Mirror Right
	for(int yi = 1; yi<fydim+1; yi++)
		b[yi][fxdim+1] = b[yi][fxdim-1];


	// -- Resize f

	// Add Columns
	for(int i=0; i<2; i++)
		f.push_back(vector <float>());

	// Copy every row of b into every row of f
	// while changing row sizes of f accordingly
	for(int i=0; i<f.size(); i++)
		f[i] = b[i];
		
}

void BoundMirrorShrink(vector<vector<float>>& f){
	
	// Delete bottom row
	f.pop_back();

	// Delete rightmost column
	for(int i=0; i<f.size(); i++)
		f[i].pop_back();

	// Delete top row
	vector<vector<float>>::iterator i = f.begin();
		f.erase(i);

	// Delete left row
	for(int i=0; i<f.size(); i++){
		vector<float>::iterator k = f[i].begin();
		f[i].erase(k);
		}
	
}

void BoundMirrorEnsure(vector<vector<float>>& f){

	int fxdim = f[0].size();
	int fydim = f.size();

	// Mirror corners	
	f[0][0] = f[2][2];
	f[0][fxdim-1] = f[2][fxdim-3];
	f[fydim-1][0] = f[fydim-3][2];
	f[fydim-1][fxdim-1] = f[fydim-3][fxdim-3]; 

	// -- Mirror Top
	for(int xi = 1; xi<fxdim-1; xi++)
		f[0][xi] = f[2][xi];

	// -- Mirror Bottom	
	for(int xi = 1; xi<fxdim-1; xi++)
		f[fydim-1][xi] = f[fydim-3][xi];

	// -- Mirror Left
	for(int yi = 1; yi<fydim-1; yi++)
		f[yi][0] = f[yi][2];

	// -- Mirror Right
	for(int yi = 1; yi<fydim-1; yi++)
		f[yi][fxdim-1] = f[yi][fxdim-3];
		
}

void SobelGradient(vector<vector<float>>& f, vector<vector<float>>& fx, vector<vector<float>>& fy){

	fx = fy = f;			

	// Compute the Gradient
	for (int i=1; i<(f.size())-1; i++)
		for (int j=1; j<(f[0].size())-1; j++)
		{
			fx[i][j] = abs((f[i-1][j+1]+2*f[i][j+1]+f[i+1][j+1]) - 
				(f[i-1][j-1]+2*f[i][j-1]+f[i+1][j-1]));
				
	    	fy[i][j] = abs((f[i-1][j-1]+2*f[i-1][j]+f[i-1][j+1])-
	        	(f[i+1][j-1]+2*f[i+1][j]+f[i+1][j+1]));

			if(abs(fx[i][j] + fy[i][j]) > 1)
				fx[i][j] = fy[i][j] = 1;

			if(abs(fx[i][j] + fy[i][j]) < 0)
				fx[i][j] = fy[i][j] = 0;
		

		}		

}

void DiscreteLaplacian(vector<vector<float>>& f){
	
	vector<vector<float>> b;
	b = f;
	
	for (int i=1; i<(f.size())-1; i++)
		for (int j=1; j<(f[0].size())-1; j++)
		{
			float i1 = f[i][j];
			float i2 = f[i+1][j];
			float i3 = f[i-1][j];
			float i4 = f[i][j+1];
			float i5 = f[i][j-1];

			b[i][j] = abs( (-4/255) * i1 + i2 + i3 + i4 + i5);

		}
		
	f = b;
	
}




#endif
