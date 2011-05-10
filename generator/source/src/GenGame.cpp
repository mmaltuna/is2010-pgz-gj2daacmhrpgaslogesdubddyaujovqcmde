#include "GenGame.h"

GenGame::GenGame(){}

void GenGame::genGame(DBManager* myDB){
	/* ---- Decidator obtiene los datos para los generadores ---- */
	// la GUI guardar� el archivo que posteriormente leer� decidator para obtener la informaci�n
	decidator = new Decidator(myDB, "./file.txt");
	int wSize = 15;//decidator->getWorldSize();
	int numDungeons = decidator->getNumDungeons();
	/*
	int numZones = decidator->getNumZones();*/
	int numZones = 4;
	int diff = decidator->getDifficulty();
	int ratioDungeon = decidator->getRatio();
	int numSafeZones = decidator->getNumSafeZones();
	int numEnemies = decidator->getNumEnemies();

	zones = new vector<GenZone*>();
	ow = new Overworld(wSize, diff, numZones, numDungeons, numSafeZones);
	GenDungeon* genDungeon = new GenDungeon();
	Decorator* decorator = new Decorator();
	int numDungeon = 1; 
	int idTool = 1;   //params para la dungeon
	vector<SafeZoneInfo>* safeZones = NULL;//new vector<SafeZoneInfo>();
	string zoneTheme;
	int zoneNumber = 1;
	string zone, lastzone = "";
	while(zones->size() < numZones)
	{
		while(strcmp(zone.c_str(),lastzone.c_str()) == 0)
			zone = myDB->getZone();
		if(strcmp(zone.c_str(), "Forest") == 0)
		{
				zoneTheme = "Forest";
				GenZone* myGenZone = new GenForestZone(zoneTheme, zoneNumber, NULL, ow, numEnemies, genDungeon, /*numDungeon*/zoneNumber, idTool, ratioDungeon, safeZones, decorator, myDB);
				zoneNumber++;
				zones->push_back(myGenZone);
		}
		else if(strcmp(zone.c_str(), "Prairie") == 0)
		{
				zoneTheme = "Prairie";
				GenZone* myGenZone = new GenFieldZone(zoneTheme, zoneNumber, NULL, ow, numEnemies, genDungeon, /*numDungeon*/zoneNumber, idTool, ratioDungeon, safeZones, decorator, myDB);
				zoneNumber++;
				zones->push_back(myGenZone);
		}
		else if(strcmp(zone.c_str(), "Lake") == 0)
		{
				zoneTheme = "Lake";  //pantano
				//GenZone* myGenZone = new GenWormZone("theme-default", zoneTheme, zoneNumber, NULL, ow, numEnemies, genDungeon, /*numDungeon*/zoneNumber, idTool, ratioDungeon, safeZones, decorator, myDB);
				GenZone* myGenZone = new GenLagoonZone(zoneTheme, zoneNumber, NULL, ow, numEnemies, genDungeon, /*numDungeon*/zoneNumber, idTool, ratioDungeon, safeZones, decorator, myDB);
				zoneNumber++;
				zones->push_back(myGenZone);
		}
		else if(strcmp(zone.c_str(), "Desert") == 0)
		{
				zoneTheme = "Desert";  
				//GenZone* myGenZone = new GenWormZone("theme-default", zoneTheme, zoneNumber, NULL, ow, numEnemies, genDungeon, /*numDungeon*/zoneNumber, idTool, ratioDungeon, safeZones, decorator, myDB);
				GenZone* myGenZone = new GenDesertZone(zoneTheme, zoneNumber, NULL, ow, numEnemies, genDungeon, /*numDungeon*/zoneNumber, idTool, ratioDungeon, safeZones, decorator, myDB);
				zoneNumber++;
				zones->push_back(myGenZone);
		}
		lastzone = zone;
	}
	
	// Decidator obtiene de la base de dator el generador de mundo a utilizar
	/*switch (decidator->getWorldGen()){   Lo comento de momento porque la consulta no devuelve na y estalla esto x.x
	case 1:
		genOw = new GenVoroWorld(ow, zones, myDB);	
		break;
	}*/
	genOw = new GenVoroWorld(ow, zones, myDB);	

	world = new World(diff, genOw, myDB);

	world->buildOverworld();
	ow->save(); //ahora aqu� se hace el guardado
	for (int i = 0; i < genDungeon->getNumDungeons(); i++) //guardamos todas las dungeons
		genDungeon->getDungeon(i)->save();

	// Decidator guarda la informaci�n que necesita el int�rprete (como n�mero de piezas de coraz�n, etc...)
	decidator->save();

	delete genDungeon; 
	genDungeon = NULL;

	delete decorator;
	decorator = NULL;
}

GenGame::~GenGame(){

	vector<GenZone*>::iterator it;
    for(it = zones->begin(); it != zones->end(); it++)
        if ((*it) != NULL)
        {
			delete (*it);
			(*it) = NULL;
        }
	delete zones;
	zones = NULL;
	
	delete ow;			// SOLO LO BORRA ESTE PORQUE ES EL CLIENTE DICTADOR
	ow = NULL;

	delete genOw;		// As� nos aseguramos que llama al delete del GenVoronoiOW
	genOw = NULL;

	delete world; 
	world = NULL;

	delete decidator;
	decidator = NULL;
}
