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

#ifndef _EQUIVALENCE_H
#define _EQUIVALENCE_H

#include <vector>
using namespace std;

#define tablesize 70000

class equivalence{
public:
	equivalence();

	vector<vector<int> > e;

	int master_regions;

	int ar[tablesize];

	int add(int e1, int e2);

	void solve_Equivalence();

	void display_Equivalence();

	int lookup_Master(int e2);

	int lookup_Master_Graph(int e2);

};

equivalence::equivalence(){

	master_regions = 0;

	for(int y=0; y<tablesize; y++)
		  e.push_back(vector <int>());

	for(int y=0; y<tablesize; y++)
			e[y].push_back(0);
		
	for(int y=0; y<tablesize; y++)
		for(int x=0; x<e[y].size(); x++)
			e[y][x] = 0;

	for(int x=0; x<tablesize; x++)
		ar[x]=0;

}

int equivalence::add(int e1, int e2){

	if(e1==e2)
		return 1;

	for(int x=0; x<e[e1].size(); x++){

		if(e[e1][x] != 0){
			if(e[e1][x]==e2)
				return 1;
		}

		else if(e[e1][x]==0){
			e[e1][x] = e2;
			e[e1].push_back(0);
			return 0;
		}
	}
	return 0;
}

void equivalence::display_Equivalence(){

	for(int y=0; y<e.size(); y++)
		if(e[y][0] != 0){
			printf("|%d| ", y);
			for(int x=0; x<e[y].size(); x++)
				if(e[y][x] != 0)
					printf("%d ", e[y][x]);

			printf("\n");
		}

	system("pause");
}

void equivalence::solve_Equivalence(){

	int v=-1;

	for(int y=1; y<e.size(); y++)//tsize
		for(int x=0; x<e[y].size(); x++)
			if(e[y][x]!=0){
				
				v=e[y][x];

				for(int i=0; i<e[v].size(); i++)
					if(e[v][i]!=0){

						if(e[v][i] == y)
							e[v][i]=0;

						else{
							for(int j=0; j<e[y].size(); j++)
								if(e[y][j]==0){
									e[y][j] = e[v][i];
									e[y].push_back(0);
									ar[e[v][i]] = y;
									e[v][i] = 0;
									break;
								}
						}
					}
			}


	for(int y=1; y<e.size(); y++)
		if(e[y][0]!=0)
			master_regions++;
}

int equivalence::lookup_Master(int e2){

	if(e[e2][0]!=0)
		return e2;

	else
		return(ar[e2]);

	return e2;
}

int equivalence::lookup_Master_Graph(int e2){

	if(e[e2][0]!=0)
		return e2;

	else{
		for(int y=1; y<e.size(); y++)//tsize
			for(int x=0; x<e[y].size(); x++)
				if(e[y][x] == e2)
					return y;
	}
	return e2;
}

#endif
