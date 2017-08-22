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

#ifndef _ACTCORE_H
#define _ACTCORE_H

#include <fstream>
#include <iomanip>
#include "time.h"
#include "segCore.h"

class actCore{
public:

	// Object Recording
	actCore();
	actCore(int st, int end);
	bool NextFrame(rgbframe *);
	bool LastFrame();
	void RegisterCells(rgbframe *);
	void ChangeOneID(rgbframe *frame, int changeFrom, int changeTo);

	// C1
	void MatchSimilarity();
	void EncapsulatePaths();
	void FindMigration();
	void MatchMigration();
	void FindLostCells();
	void MatchLostCells(int);
	int LinkPaths(int,int);
	void AssociateCellsC1();
	void MakePath(int, cell *, cell *);
	void WritePath(int, cell, cell);

	// C2
	void AssociateCellsC2();
	void AuctionC2();
	void NextSet();
	void WritePathC2(int, cell, cell);

	// C3
	void AssociateCellsC3();
	void AuctionC3();
	void ChangeWeights();
	void NextCellSet();
	void WritePathC3(int, cell, cell);
	int UpdatePaths(int,int, cell, cell);

	// Output
	void OutputPath();
	void OutputPath_File(char* name, char* celltype, char* comp);
	void OutputPath_VisualizationactCoreing();
	void OutputPath_VisualizationSpeed();
	void WriteactCores(rgbframe *);

	
	// Measurements
	void CalculateSpeed();
	void CalculateAngles();
	void CalculatetotalPath();
	double Speed(int,int,int,int);
	double Dispersion(int,int,int,int,int,int);

	int first, last;
	int totalframes;
	int frameNumber;
	float time;
	float timeinterval;
	string directory;
	int actCores;
	int totalcells;

	vector<vector<cell> > record; 
	vector<vector<cell> > path;
	vector<int> type;
	vector<int> connector;

};

actCore::actCore(){

	frameNumber=0;
	time=0;
	actCores=0;
	timeinterval=15;

}

actCore::actCore(int st, int end){

	frameNumber=st-1;
	time=0;
	timeinterval=15;
	first=st;
	last=end;
	totalframes=(last-(first-1));
	directory = "";
	actCores=0;
	totalcells=0;

	for(int i=0; i<(totalframes+1); i++)
		record.push_back(vector <cell>());
		
}

bool actCore::LastFrame(){

	if(frameNumber==last)
		return true;

	return false;

}

bool actCore::NextFrame(rgbframe *frame){

		if(frameNumber==last)
		return false;
	  
	// -- Frame Input --
	// Replace As Needed
	int dig = 2;

	if(frameNumber<9){
		frame->name[0]='0';
		frame->name[1]=(char)frameNumber+1+'0';
		dig=2;
	}

	else if(frameNumber<99){
		frame->name[0]=(char)((frameNumber+1)/10)+'0';
		frame->name[1]=(char)((frameNumber+1)%10)+'0';
		dig=2;
	}

	else if(frameNumber<999){
		frame->name[0]=(char)((frameNumber+1)/100)+'0';
		frame->name[1]=(char)(((frameNumber+1)/10)%10)+'0';
		frame->name[2]=(char)((frameNumber+1)%10)+'0';
		dig=3;
	}

	else if(frameNumber<9999){
		frame->name[0]=(char)((frameNumber+1)/1000)+'0';
		frame->name[1]=(char)(((frameNumber+1)/100)%10)+'0';
		frame->name[2]=(char)(((frameNumber+1)/10)%10)+'0';
		frame->name[3]=(char)((frameNumber+1)%10)+'0';
		dig=4;
	}


	frame->name[dig]='.';
	frame->name[dig+1]='p';
	frame->name[dig+2]='p';
	frame->name[dig+3]='m';
	frame->name[dig+4]='\0';

	ScanPpm(frame);
	// -- End --

	frameNumber++;
	frame->framenumber = frameNumber;
	if(frameNumber!=1)
		time=(frameNumber-1)*timeinterval;

	return true;
}

void actCore::RegisterCells(rgbframe *frame){

	int count = frame->cells.size();
	list<cell> cells = frame->cells;

	for(int i=0; i<count; i++){
		record[(frame->framenumber-first)+1].push_back(cells.back());
		cells.pop_back();
	}

}


// -----===-----===-----===-----===-----===-----===-----=== //
// ===-----===-----=== Output Functions ===-----===-----=== //
// -----===-----===-----===-----===-----===-----===-----=== //

void actCore::OutputPath(){

	float total_speed = 0;
	int count = 0;

	for(int i=0; i< path.size(); i++)
		if(path[i].size()>1 || path[i][0].centroidX!=0){
			printf("\n[Path #%d]\n", i+1);
			for(int j=0; j<totalframes; j++){
				printf("Frame %d: ", (j+1+first)-1);
				if(path[i][j].centroidX==0){
					printf("Spacer\n");
					total_speed = INT_MIN;
				}
				else{
					printf("(%d, %d)  {speed: %.2f micrometers/hour}\n", path[i][j].centroidX, path[i][j].centroidY, path[i][j].speed);
					total_speed+=path[i][j].speed;
					if(path[i][j].speed!=0)
						count++;
				}
			}

			printf("[End Of track]\n");

			total_speed = total_speed / count;

			if(total_speed>0){
				printf("{Avg Speed: %.2f micrometers/hour}\n", total_speed);
			}
			
			count=0;
			total_speed=0;
		}


}

void actCore::OutputPath_File(char* name, char* celltype, char* comp){

	string outputFile = directory;
	outputFile.append("\\Tracking Output.txt");

	fstream Output(outputFile.c_str(), ios::out);

	float total_speed = 0;
	float mean_angle=0;
	float angular_deviation=0;
	float path_length=0;
	float net_path=0;
	int count = 0;

	char date[15];
	_strdate(date);

	Output << "Experiment: " << name << endl;
	Output << "Date: " << date << endl;
	Output << "Cell Type: " << celltype << endl;
	Output << "Image Composition: " << comp << endl;
	Output << "Time Interval (min): " << timeinterval << " Minutes" << endl;
	Output << "Frames Processed: " << totalframes << endl;
	Output << "Notes: " << endl;
	Output << endl << endl;

	for(int i=0; i< path.size(); i++)
		if(path[i].size()>1 || path[i][0].centroidX!=0)
			if(path[i][0].objArea!=-2){
			Output << "\n[Cell #" << i+1 << "]\n";
			mean_angle = path[i][0].meanAngle;
			angular_deviation = path[i][0].angularDeviation;
			path_length = path[i][0].totalPath;
			net_path = path[i][0].netPath;
			for(int j=0; j<totalframes; j++){
				if(path[i][j].migrateIn==1)
					Output << "Cell #" << i+1 << " migrates in at frame " << (j+1+first)-1 << endl;
				if(path[i][j].pickedUp==1)
					Output << "Cell #" << i+1 << " is picked up at frame " << (j+1+first)-1 << endl;
				Output << "Frame " << (j+1+first)-1;
				Output << " " << setfill('0') << setw(2) << (int)(j*timeinterval)/60 << ":" << setw(2) << (int)(j*timeinterval)-((int)(j*timeinterval)/60)*60;
				Output << ":" << setprecision(2) << fixed << setw(2) << (int)(10*((((float)(j*timeinterval)-((int)(j*timeinterval)/60)*60)-((int)(j*timeinterval)-((int)(j*timeinterval)/60)*60))*6));
				Output << " : ";
				if(path[i][j].centroidX==0){
					Output << "No Data\n";
					total_speed = INT_MIN;
				}
				else{
					Output << "(" << path[i][j].centroidX << "," << path[i][j].centroidY << ")   {speed: ";
					Output << setprecision(2) << fixed << path[i][j].speed << " micrometers/hour}";
					Output << " {orientation: " << setprecision(2) << fixed << path[i][j].alpha*180/PI << " degrees}\n";
					if(path[i][j].migrateOut==1)
						Output << "Cell #" << i+1 << " migrates out at frame " << (j+1+first)-1 << endl;
					if(path[i][j].droppedOff==1)
						Output << "Cell #" << i+1 << " is dropped off at frame " << (j+1+first)-1 << endl;
					total_speed+=path[i][j].speed;
					if(path[i][j].speed!=0)
						count++;
				}
			}

			Output << "[End Of Track]\n";
			Output << "Mean Angle over Track: " << mean_angle << " degrees." << endl;
			Output << "Angular Deviation over Track: " << angular_deviation << " degrees." << endl;
			Output << "Total Path Length: " << path_length << " micrometers." << endl;
			Output << "Net Path Length: " << net_path << " micrometers." << endl;
			if(path_length!=0) Output << "Tactic Index: " << net_path/path_length << endl;

			total_speed = total_speed / count;

			if(total_speed>0){
				Output << "{Avg Speed: " << setprecision(2) << fixed << total_speed << " micrometers/hour}\n";
			}
			
			count=0;
			total_speed=0;
		}

	Output.close();


}

void actCore::OutputPath_VisualizationactCoreing(){

	string outputFile = directory;
	outputFile.append("\\Visual_Tracking_Output.txt");

	fstream Output(outputFile.c_str(), ios::out);

	float total_speed = 0;
	int count = 0;

	Output << "(Total number of cells: " << totalcells << ")" << endl;

	for(int i=0; i< path.size(); i++)
		if(path[i].size()>1 || path[i][0].centroidX!=0)
			if(path[i][0].objArea!=-2){
			Output << "\n[Cell #" << i+1 << "]\n";
			Output << "{Start frame: " << first << "}" << endl;
			Output << "{Start time: " << (first-1)*timeinterval << " Minutes}" << endl;
			Output << "{End frame: " << last << "}" << endl;
			Output << "{End  time: " << (last-1)*timeinterval << " Minutes}" << endl;

			for(int j=0; j<totalframes; j++){

				if(path[i][j].centroidX==0){
					Output << "Spacer\n";
					total_speed = INT_MIN;
				}
				else{
					Output << "(" << path[i][j].centroidX << "," << path[i][j].centroidY << ")\n";//   {speed: ";
					//Output << setprecision(2) << fixed << path[i][j].speed << " micrometers/hour}";
					//Output << " {orientation: " << setprecision(2) << fixed << path[i][j].alpha*180/PI << " degrees}\n";
					total_speed+=path[i][j].speed;
					if(path[i][j].speed!=0)
						count++;
				}
			}

			Output << "[End Of Track]\n";

			total_speed = total_speed / count;
			
			count=0;
			total_speed=0;
		}

	Output.close();


}

void actCore::OutputPath_VisualizationSpeed(){

	string outputFile = directory;
	outputFile.append("\\Visual_Speed_Output.txt");

	fstream Output(outputFile.c_str(), ios::out);

	float total_speed = 0;
	int count = 0;

	Output << "(Total number of cells: " << totalcells << ")" << endl;

	for(int i=0; i< path.size(); i++)
		if(path[i].size()>1 || path[i][0].centroidX!=0)
			if(path[i][0].objArea!=-2){
			Output << "\n[Cell #" << i+1 << "]\n";
			Output << "{Start frame: " << first << "}" << endl;
			Output << "{Start time: " << (first-1)*timeinterval << " Minutes}" << endl;
			Output << "{End frame: " << last << "}" << endl;
			Output << "{End  time: " << (last-1)*timeinterval << " Minutes}" << endl;

			for(int j=0; j<totalframes; j++){

				if(path[i][j].centroidX==0){
					Output << "Spacer\n";
					total_speed = INT_MIN;
				}
				else{
					Output << "(" << path[i][j].centroidX << "," << path[i][j].centroidY << ") ";
					Output << setprecision(2) << fixed << path[i][j].speed << endl;
					total_speed+=path[i][j].speed;
					if(path[i][j].speed!=0)
						count++;
				}
			}

			Output << "[End Of Track]\n";

			total_speed = total_speed / count;
			
			count=0;
			total_speed=0;
		}

	Output.close();


}

void actCore::WriteactCores(rgbframe *frame){

	list<cell>::iterator obj = frame->cells.begin();
	list<cell>::iterator last_obj = frame->cells.end();

	for( ; obj!=last_obj; ++obj)
		for(int i=0; i<path.size(); i++)
			if((frameNumber-first)<path[i].size() && (frameNumber-first)>=0)
				if(path[i][frameNumber-first].centroidX==obj->centroidX && obj->centroidY==path[i][frameNumber-first].centroidY){
				obj->actCoreID = i+1;
				break;
			}

}

// Change one ID in a frame
void actCore::ChangeOneID(rgbframe *frame, int changeFrom, int changeTo){

	list<cell>::iterator obj = frame->cells.begin();
	list<cell>::iterator last_obj = frame->cells.end();

	for( ; obj!=last_obj; ++obj){
				if(obj->actCoreID == changeFrom)
					obj->actCoreID = changeTo;
			}

}


void actCore::CalculateSpeed(){

	for(int i=0; i<path.size(); i++)
		for(int j=1; j<path[i].size(); j++)
			if(path[i][j-1].centroidX!=0 && path[i][j].centroidX!=0)
				path[i][j].speed = Speed(path[i][j-1].centroidX, path[i][j-1].centroidY,
										 path[i][j].centroidX, path[i][j].centroidY);



}

void actCore::CalculateAngles(){

	
	// For every cell
	for(int i=0; i<path.size(); i++){

		vector<int> angles;
		int n = 0;
		for(int a=0; a<361; a++)
			angles.push_back(0);

		// Calculate directional angle for each cell movement
		// Store into histogram
		for(int j=1; j<path[i].size(); j++)
			if(path[i][j-1].centroidX!=0 && path[i][j].centroidX!=0){
				
				int chg_x = path[i][j].centroidX - path[i][j-1].centroidX;
				int chg_y = path[i][j].centroidY - path[i][j-1].centroidY;
				
				int whole_angle_in_degrees = atan((double)chg_y/chg_x)*180/PI;
				while(whole_angle_in_degrees>360)
					whole_angle_in_degrees-=360;
				while(whole_angle_in_degrees<0)
					whole_angle_in_degrees+=360;

				angles[whole_angle_in_degrees]++;
				n++;

			}

			double X=0, Y=0;

			for(int k=0; k<angles.size(); k++)
				if(angles[k]!=0){
					X+=angles[k]*sin(k*PI/180);
					Y+=angles[k]*cos(k*PI/180);
				}

			X/=n;
			Y/=n;

			double r = sqrt((X*X) + (Y*Y));
			double mean_angle= X/r;

			mean_angle = acos(mean_angle)*180/PI;
			double angular_deviation = 180/PI*sqrt(2*(1-r));

			path[i][0].meanAngle = mean_angle;
			path[i][0].angularDeviation = angular_deviation;

	}

}

void actCore::CalculatetotalPath(){

	double total_distance = 0;

	for(int i=0; i<path.size(); i++){

		for(int j=0; j<path[i].size()-1; j++)
			if(path[i][j].centroidX!=0 && path[i][j+1].centroidX!=0){


				double x1 = path[i][j].centroidX;
				double x2 = path[i][j+1].centroidX;
				double y1 = path[i][j].centroidY;
				double y2 = path[i][j+1].centroidY;

				total_distance+=sqrt((double)(((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2))));
			}

			path[i][0].totalPath = total_distance*(0.6615);
			total_distance = 0;
	}


	double net_distance = 0;

	for(int i=0; i<path.size(); i++){

		int j1=INT_MAX, j2=INT_MIN;
		for(int j=0; j<path[i].size(); j++)
			if(path[i][j].centroidX!=0){
				j1 = min(j, j1);
				j2 = max(j, j2);
			}

		if(j1!=INT_MAX && j2!=INT_MIN){
			double x1 = path[i][j1].centroidX;
			double x2 = path[i][j2].centroidX;
			double y1 = path[i][j1].centroidY;
			double y2 = path[i][j2].centroidY;

			net_distance = sqrt((double)(((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2))));
		}
		
		path[i][0].netPath = net_distance*(0.6615);
		net_distance = 0;

	}


}

// Calculates speed given 2 point coordinates based on timeinterval
// Returns speed value in micrometers
double actCore::Speed(int x1, int y1, int x2, int y2){

	if(x1+y1==0 || x2+y2==0)
		return 0;

	// Calculate Distance
	double dist = sqrt((double)(((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2))));

	// Convert Distance to Micrometers
	dist=dist*(0.6615);

	// Calculate Speed: Displacement / Time elapsed (hrs)
	double speed = dist/(timeinterval/60);

	return speed;

}

// -- Cost Function C1 -- //
void actCore::MatchSimilarity(){

	for(int i=1; i<totalframes; i++){
		int j = i+1;

		if(verbose) printf("Matching frames %d & %d\n", (i+first)-1, (j+first)-1);
		
		// Remember number of objects in both frames
		int m = record[i].size();
		int n = record[j].size();

		// Create U
		vector<vector<double>> U;
		for(int z=0; z<m; z++)
			U.push_back(vector<double>());

		for(int z=0; z<m; z++)
			for(int q=0; q<n; q++)
				U[z].push_back(0.0);

		// Build U
		for(int x=0; x<record[i].size(); x++)
			for(int z=0; z<record[j].size(); z++){
				int x1 = record[i][x].centroidX;
				int y1 = record[i][x].centroidY;
				int x2 = record[j][z].centroidX;
				int y2 = record[j][z].centroidY;

				double sim = (x1*x2)+(y1*y2);
				double t = sqrt((double)(x1*x1)+(y1*y1)) * sqrt((double)(x2*x2)+(y2*y2));
				sim = sim/t;

				double dist = sqrt((double)(((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2))));

				U[x][z] = sim/dist;

			}

		vector<int> ItoJ;
		vector<int> JtoI;

		for(int x=0; x<record[i].size(); x++)
			ItoJ.push_back(0);

		for(int x=0; x<record[j].size(); x++)
			JtoI.push_back(0);

		double TsFavorite = INT_MIN;
		double T1sFavorite = INT_MIN;
		int TFavPos, T1FavPos;

		// Solve U: Find favorites i->j
		for(int y=0; y<U.size(); y++){
			for(int x=0; x<U[y].size(); x++){
				
				if(TsFavorite<U[y][x]){
					TsFavorite=U[y][x];
					TFavPos = x;
				}
			}

			ItoJ[y] = TFavPos;
			TFavPos=0;
			TsFavorite = INT_MIN;
		}

		// Solve U: Find favorites j->i
		for(int x=0; x<record[j].size(); x++){
			for(int y=0; y<record[i].size(); y++){
				
				if(T1sFavorite<U[y][x]){
					T1sFavorite=U[y][x];
					T1FavPos = y;
				}
			}

			JtoI[x] = T1FavPos;
			T1FavPos=0;
			T1sFavorite = INT_MIN;
		}

		// Match Favorites
		for(int q=0; q<ItoJ.size(); q++){
			int match = ItoJ[q];
			if(JtoI[match]==q){
				MakePath(i, &record[i][q], &record[j][match]);
				WritePath(i, record[i][q], record[j][match]);
				for(int z=0; z<record[j].size(); z++)
					U[q][z]=0;
				for(int z=0; z<record[i].size(); z++)
					U[z][match]=0;
			}
		}

	}

}

void actCore::MakePath(int srcframe, cell *source, cell *destination){

	// If object has never been handled before,
	// Create a space for it and give it a actCoreID.
	if(source->actCoreID==-1){
		path.push_back(vector <cell>());
		destination->actCoreID = source->actCoreID = actCores;
		actCores++;
		totalcells++;
	}

	// Ensure the heritage of actCoreIDs
	destination->actCoreID = source->actCoreID;

}

void actCore::WritePath(int srcframe, cell source, cell destination){

	cell spacer;
	spacer.centroidX=spacer.centroidY=0;
	spacer.actCoreID=source.actCoreID;

	// Handle Spacers
	while(srcframe>path[source.actCoreID].size() && srcframe!=first)
		path[source.actCoreID].push_back(spacer);

	// Insert objects into path
	if(srcframe==first){
		path[source.actCoreID].push_back(source);
		path[source.actCoreID].push_back(destination);
	}

	else{
		path[source.actCoreID].push_back(destination);

		int sz = path[source.actCoreID].size();
		if(path[source.actCoreID][sz-2].centroidX==0){
			path[source.actCoreID].pop_back();
			path[source.actCoreID].pop_back();
			path[source.actCoreID].push_back(source);
			path[source.actCoreID].push_back(destination);
		}
	}

}

void actCore::EncapsulatePaths(){

	// Encapsulate all prematurely terminating paths by adding a spacer to their end
	cell spacer;
	spacer.centroidX=spacer.centroidY=0;
	
	for(int i=0; i<path.size(); i++)
		while(path[i].size() < totalframes){
			int j = path[i].size()-1;
			spacer.actCoreID = path[i][j].actCoreID;
			path[i].push_back(spacer);
		}

}

int actCore::LinkPaths(int id1, int id2){

	if(id1==id2 || id1<0 || id2<0 || id1>path.size() || id2>path.size())
		return 1;

	int id1_first = INT_MAX;
	int id2_first = INT_MAX;

	cell spacer, nullspacer;
	spacer.centroidX=spacer.centroidY=0;
	nullspacer.centroidX=nullspacer.centroidY=0;
	nullspacer.objArea=-2;

	// Determine which path exists first
	for(int i=0; i<path[id1].size(); i++)
		if(path[id1][i].centroidX!=0)
			if(i<id1_first)
				id1_first = i;

	for(int i=0; i<path[id2].size(); i++)
		if(path[id2][i].centroidX!=0)
			if(i<id2_first)
				id2_first = i;

	// Swap so that id1 is leading path
	if(id2_first<id1_first){
		int temp=id1;
		id1=id2;
		id2=temp;
	}

	if(verbose) printf("%d is leading, %d is following\n", id1+1, id2+1);

	int id1_connect=-1;
	int id2_connect=-1;

	// Find id1's trailing spacer
	for(int i=0; i<path[id1].size(); i++)
		if(path[id1][i].centroidX==0)
			id1_connect=i;

	if(id1_connect==-1)
		return 1;

	// Find id2's connecting spacer
	for(int i=0; i<path[id2].size()-1; i++)
		if(path[id2][i].centroidX==0)
			if(path[id2][i+1].centroidX!=0)
				id2_connect=i;

	if(id2_connect==-1)
		return 1;


	if(verbose) printf("%d connects at %d, %d connects at %d\n", id1+1, id1_connect, id2+1, id2_connect);

	// Make id1 big enough to hold id2
	int sz = path[id2].size();
	while(path[id1].size()<sz){
		spacer.actCoreID=path[id1][0].actCoreID;
		path[id1].push_back(spacer);
	}

	// Copy non-spacer values from id2 to id1
	for(int i=0; i<sz; i++){
		if(path[id1][i].centroidX!=0 && path[id2][i].centroidX!=0)
			printf("Collision in tracker #%d\n", id1+1);
		if(path[id1][i].centroidX==0)
			if(path[id2][i].centroidX!=0){
				path[id1][i].centroidX = path[id2][i].centroidX;
				path[id1][i].centroidY = path[id2][i].centroidY;
				path[id1][i].alpha = path[id2][i].alpha;
				path[id1][i].g = path[id2][i].g;
				path[id1][i].speed = path[id2][i].speed;
				path[id1][i].persistence = path[id2][i].persistence;
				path[id1][i].actCoreID = id1;
				path[id1][i].circularity = path[id2][i].circularity;
				path[id1][i].endx = path[id2][i].endx;
				path[id1][i].endx = path[id2][i].endy;
				path[id1][i].objArea = path[id2][i].objArea;
				swap(path[id1][i], path[id2][i]);
			}
	}

	// Delete id2's actCore
	path[id2].clear();

	// Mark as dead actCore
	path[id2].push_back(nullspacer);

	totalcells--;

	// Fill up with spacers
		while(path[id2].size() < totalframes)
			path[id2].push_back(spacer);

	return 0;

}


// Cost Function C1
void actCore::AssociateCellsC1(){

	MatchSimilarity();
	EncapsulatePaths();
	FindMigration();
	FindLostCells();
	MatchMigration();
	MatchLostCells(1);
	MatchLostCells(2);
	MatchLostCells(10);
	MatchLostCells(80);

	CalculateSpeed();
	CalculateAngles();
	CalculatetotalPath();

}


// Cost Function C1
void actCore::FindMigration(){

	// Find paths that may end with cell migration out of frame
	for(int x=0; x<path.size(); x++)
		for(int y=0; y<path[x].size()-1; y++)
			if(path[x][y].centroidX!=0 && path[x][y+1].centroidX==0)
				if(path[x][y].centroidX<30 || path[x][y].centroidX>1260 || path[x][y].centroidY<30 || path[x][y].centroidY>1000)
					path[x][y].migrateOut=1;

	// Find paths that may begin with cell migration into frame
	for(int x=0; x<path.size(); x++)
		for(int y=1; y<path[x].size(); y++)
			if(path[x][y].centroidX!=0 && path[x][y-1].centroidX==0)
				if(path[x][y].centroidX<30 || path[x][y].centroidX>1260 || path[x][y].centroidY<30 || path[x][y].centroidY>1000)
					path[x][y].migrateIn=1;

}

// Cost Function C1
void actCore::FindLostCells(){

	// Find paths in which cells drop off
	for(int x=0; x<path.size(); x++)
		for(int y=0; y<path[x].size()-1; y++)
			if(path[x][y].centroidX!=0 && path[x][y+1].centroidX==0)
				if(path[x][y].migrateOut!=1)
					path[x][y].droppedOff=1;

	// Find paths in which cells are picked up
	for(int x=0; x<path.size(); x++)
		for(int y=1; y<path[x].size(); y++)
			if(path[x][y].centroidX!=0 && path[x][y-1].centroidX==0)
				if(path[x][y].migrateIn!=1)
					path[x][y].pickedUp=1;

}

void actCore::MatchMigration(){

	for(int x=0; x<path.size(); x++)
		for(int y=0; y<path[x].size(); y++)
			if(path[x][y].migrateOut==1){

				//cout << "Matching Path " << x+1 << endl;
				double mindist = INT_MAX;
				int pos = -1;
				for(int m=0; m<path.size(); m++)
					for(int n=0; n<path[m].size(); n++)
						if(path[m][n].migrateIn==1){

							//cout << "Possible Found: Path " << m+1 << endl;

							bool fail = false;
							for(int z=0; z<min(path[m].size(), path[x].size()); z++)
								if(path[x][z].centroidX!=0 && path[m][z].centroidX!=0){
									//cout << "Path " << m+1 << " fails data test" << endl;
									fail=true;
								}
							if(!fail){
								double x1 = path[x][y].centroidX;
								double y1 = path[x][y].centroidY;
								double x2 = path[m][n].centroidX;
								double y2 = path[m][n].centroidY;

								double dist = sqrt((double)(((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2))));

							//	cout << "Path distance is " << dist << endl;
								if(dist<mindist){
									mindist = dist;
									pos = m;
								//	cout << "Accepted as minimum distance!" << endl;
								}

							}// cout << endl << endl; system("pause");
						}

					if(pos!=-1 && mindist<200){
					//	cout << "Matching Path " << x+1 << " with Path " << pos+1 << endl;
					LinkPaths(x,pos);
					}

			}

}

void actCore::MatchLostCells(int threshold){

	for(int x=0; x<path.size(); x++)
		for(int y=0; y<path[x].size(); y++)
			if(path[x][y].droppedOff==1){

				//cout << "Matching Path " << x+1 << endl;
				double mindist = INT_MAX;
				int pos = -1;
				for(int m=0; m<path.size(); m++)
					for(int n=0; n<path[m].size(); n++)
						if(path[m][n].pickedUp==1 && abs(y-n)<=threshold){

							//cout << "Possible Found: Path " << m+1 << endl;

							bool fail = false;
							for(int z=0; z<min(path[m].size(), path[x].size()); z++)
								if(path[x][z].centroidX!=0 && path[m][z].centroidX!=0){
								//	cout << "Path " << m+1 << " fails data test" << endl;
									fail=true;
								}
							if(!fail){
								double x1 = path[x][y].centroidX;
								double y1 = path[x][y].centroidY;
								double x2 = path[m][n].centroidX;
								double y2 = path[m][n].centroidY;

								double dist = sqrt((double)(((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2))));

							//	cout << "Path distance is " << dist << endl;
								if(dist<mindist){
									mindist = dist;
									pos = m;
									//cout << "Accepted as minimum distance!" << endl;
								}

							} //cout << endl << endl; system("pause");
						}

					if(pos!=-1 && mindist<200){
						//cout << "Matching Path " << x+1 << " with Path " << pos+1 << endl;
						LinkPaths(x,pos);
					}

			}



}



#endif
