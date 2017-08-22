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

#include "actCore.h"
#include "direct.h"
#include "image_output.h"
#include "windows.h"

using namespace std;

int main(int argc, char *argv[]) {

	int a,b,e;
	float c;
	char dummy;
	char trialName[300];
	char cellType[100];
	char composition[100];

	char path[_MAX_PATH];
	getcwd(path,_MAX_PATH);

	system("cls");
	cout << "\n\n\n\n\n";
	cout << setw(67) << "===-----===-----===-----===-----===-----===-----===----=\n";
	cout << setw(67) << "=---===---===---=== Microsight Tracker ===-----===-----=\n";
	cout << setw(67) << "===---===---===---===---===---===---===---===---===---==\n";
	cout << setw(67) << "=---===---===---===---= ACT v0.70 =---===---===---===---\n";
	cout << setw(67) << "===-----===-----===-----===-----===-----===-----===----=\n";
	cout << endl << endl << endl;
	cout << setw(65) << "        Choose an output format:             \n";
	cout << setw(65) << "[1] High-Contrast           [2] Color        \n";
	cout << setw(8) << "\t\t   " << ">";
	cin >> e;
	if(e<1 || e>2)
		e=2;

	if(e==1)
		e = 6;

	if(e==2)
		e = 5;
	

	system("cls");

	cout << "**************************************************\n";
	cout << "*              Cell Tracker Console              *\n";
	cout << "**************************************************\n";

	cout << "Start Frame:";
	cin >> a;
	cout << "End Frame:";
	cin >> b;
	cout << "Time Interval:";
	cin >> c;
	cout << "Trial Label:";
	cin.get(dummy);
	cin.get(trialName,100);
	cout << "Cell Type:";
	cin.get(dummy);
	cin.get(cellType,100);
	cout << "Image Composition:";
	cin.get(dummy);
	cin.get(composition,100);

	// Make output dir //
	string outputDirectory = path;
	outputDirectory.append("\\");
	outputDirectory.append(trialName);
	outputDirectory.append(" Output");
	CreateDirectory(outputDirectory.c_str(), NULL);
	

	// Start tracking system //
	cout << "Let's go!\n";
	actCore one(a,b);
	one.timeinterval = c;
	one.directory = outputDirectory;


	// ==-----===-----===-----===-----===-----===-----=== //
	// --==--==Segmentation & Cell-Object Storage==--==-- //
	// ==-----===-----===-----===-----===-----===-----=== //
	while(!one.LastFrame()){
		rgbframe frame;
		one.NextFrame(&frame);
		printf("|-------- Frame #%d Processing Underway. --------|\n", frame.framenumber);
		printf("Time: %02d : %02d : %02.0f\n", (int)one.time/60, (int)one.time-((int)one.time/60)*60, 
			10*((((float)one.time-((int)one.time/60)*60)-((int)one.time-((int)one.time/60)*60))*6));
		printf("Segmenting frame #%d....", one.frameNumber);
		ProcessFrame(&frame);
		printf("Ok.\n");

		printf("Locating Cells...");
		FrameObjects(&frame);
		printf("Ok.\n");

		printf("Registering Cells with Tracker....");
		one.RegisterCells(&frame);
		printf("Ok.\n");

		printf("[%d Cell-Objects Found.]\n", frame.cells.size());
		printf("|-------- Frame #%d Processing Complete. --------|\n\n", frame.framenumber);

		FreeFrame(&frame);
	}


	one.OutputPath_File(trialName, cellType, composition);

	// --==--==Data Association==--==-- //
	printf("Associating Cells....");
	one.AssociateCellsC1();
	printf("Ok.\n");


	// --==--==Data Output==--==-- //
	one.OutputPath_File(trialName, cellType, composition);


	// --==--==Frame Output==--==-- //
	// Write processed image files here
	one.frameNumber=a-1;
	while(!one.LastFrame()){
		rgbframe frame;
		one.NextFrame(&frame);
		printf("Writing tracking Frame #%d....", one.frameNumber);
		ProcessFrame(&frame);
		FrameObjects(&frame);
		one.WriteactCores(&frame);
		OutputFrame(&frame, e, trialName, outputDirectory);
		printf("Ok.\n");
	}

	system("pause");

	return 0;

}
