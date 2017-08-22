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

#ifndef _FRAME_H
#define _FRAME_H

#include <cstdio>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <math.h>
#include <list>
#include <vector>

#define verbose false

#define MAXNAME 1000
#define MAXREGIONS 70000
#define PI 3.14159265
using namespace std;

class rgbframe {

    friend class cell;

	public:
	  rgbframe(){}
	  char name[MAXNAME];
	  int xdim;
	  int ydim;
	  int highestvalue;
	  int totalPixels;
	  int avgintensity;
	  int framenumber;
	  vector<vector<unsigned char> > r; 
	  vector<vector<unsigned char> > g; 
	  vector<vector<unsigned char> > b;
	  vector<vector<unsigned int> > regionID;
	  vector<vector<unsigned int> > isBoundary;
	  vector<vector<unsigned int> > isCentroid;
	  vector<vector<unsigned int> > gradientMap;
	  int RegionSize[MAXREGIONS];
	  int totalRegions;
	  int numPoints;
	  float alpha;
	  float beta;
	  float gamma;
	  vector<vector<unsigned int> > isACPoint;
	  list<cell> cells;
};

class cell{
	public:
		cell(){}
		cell(rgbframe *image, int regionID);
		int startx, starty;
		int endx, endy;
		int xdim;
		int ydim;
		int objectID;
		int actCoreID;
		double speed;
		double persistence;
		double meanAngle;
		double angularDeviation;
		double totalPath;
		double netPath;
		int migrateOut;
		int migrateIn;
		int pickedUp;
		int droppedOff;
		int centroidX;
		int centroidY;
		int objArea;
		double circularity;
		double alpha;
		double g;

};

void GetImagePpm(rgbframe *);

void ScanPpm(rgbframe *);

void OutputPpm(rgbframe *);

void FreeFrame(rgbframe *);

#endif

