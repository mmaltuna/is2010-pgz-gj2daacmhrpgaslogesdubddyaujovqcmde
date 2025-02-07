/*----------------------------------------------------------------------------------*/
//This code is part of a larger project whose main purpose is to entretain either	//
//by helping making it better or by playing it in its actual state.					//
//																					//
//Copyright (C) 2011 Three Legged Studio											//
//																					//
//    This program is free software; you can redistribute it and/or modify			//
//    it under the terms of the GNU General Public License as published by			//
//    the Free Software Foundation; either version 2, or (at your option)			//
//    any later version.															//
//																					//
//    This program is distributed in the hope that it will be useful,				//
//    but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the					//
//    GNU General Public License for more details.									//
//																					//
//    You should have received a copy of the GNU General Public License				//
//    along with this program; if not, write to the Free Software Foundation,		//
//    Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.			//
//																					//
//	  You can contact us at projectpgz.blogspot.com									//
/*----------------------------------------------------------------------------------*/

#pragma once

#ifndef _MAIN_
#define _MAIN_

#define _CRTDBG_MAP_ALLOC
//#define SIMULACION_OW 
//#define NORMAL
#define DEBUG_GAME_GENERATOR
//#define DBMANAGER 

#include <stdlib.h>
#include <crtdbg.h>
#include <stdio.h>
#include <iostream>

#include "Dungeon.h"
#include "DungeonJ.h"
#include "DungeonM.h"
#include "GenGame.h"

using namespace std;


// Este main act�a como cliente de la factor�a (como el futuro "GenDungeon")
int main(int argc, char** argv) {
	

	DBManager* db = new DBManager();
	double time_max = -1;
	//DungeonJ* dj;
	//DungeonM* dm;
	DungeonPos dp = {0,0,0,0};
	#ifdef	DBMANAGER
		std::string s = "Zelda";
		db->getBossKey(s);
		for(int i = 0; i < 10; i++){
					//db->getBlock(s,"Forest",);
					db->getTool(s);
					db->getEnemy("Forest",s);
					db->getItem("Forest");
		}
		//db->getExchange(s);
		db->getBossKeyGfxId();
		db->getKey(s);
		db->getKeyGfxId();
		db->getNPC("Forest",s);
		db->getPlayer(s);
		db->getPowUp(s);
		db->getWorldGen(s);
		db->getZone(s);
	#endif
	
	#ifdef SIMULACION_OW

		for(int i = 0; i < 1000; i++)
			for(int j = 0; j < 26; j++){
				clock_t t1 = clock();
				dj = new DungeonJ("Forest", "Zelda", 2, j, 100, 3, 0, dp, db);
				dj->generate();
				//dm = new DungeonM("Forest", "Zelda", 2, j, 100, 3, 0, dp, db);
				//dm->generate();
				delete dj;
				//delete dm;
				clock_t t2 = clock();
				cout<<"Tiempo empleado: " << double(t2-t1)/CLOCKS_PER_SEC<<" segundos."<<endl;
				if (time_max < double(t2-t1)/CLOCKS_PER_SEC)
					time_max = double(t2-t1)/CLOCKS_PER_SEC;
			}	

		printf("\ntiempo_max: %f\nEnter",time_max);
		getchar();
	#endif

	#ifdef NORMAL
		dj = new DungeonJ("Forest", "Zelda", 2, 0, 100, 3, 0, dp,db);

		dj->generate();
	
		dj->save();

		delete dj;
	#endif

	#ifdef DEBUG_GAME_GENERATOR
		srand((unsigned int)time(NULL));
		GenGame* myGame;
		for(int i = 0; i < 1; i++){
			clock_t t1 = clock();
			myGame = new GenGame();
			myGame->genGame(db);
			delete myGame;
			clock_t t2 = clock();
			cout<<"Time taken: " << double(t2-t1)/CLOCKS_PER_SEC <<" segundos."<<endl;
			if(time_max < double(t2-t1)/CLOCKS_PER_SEC )
				time_max = double(t2-t1)/CLOCKS_PER_SEC;
			
		}
		cout << "Generation Finished!\n";
	#endif

	delete db;

	//_CrtDumpMemoryLeaks();

	return 0;
}

#endif