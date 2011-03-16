#include "Zone.h"

// Constructora.
Zone::Zone(int zoneTypeId, vector<Point>* zoneShape){
	// Asignamos parametros a los atributos.
	typeId = zoneTypeId;
	shape = zoneShape;
}

// Destructora.
Zone::~Zone(){;}

// Devuelve el tipo de zona en forma de int.
int Zone::getTypeId(){
	return typeId;
}

// Permite modificar el tipo asociado a una zona.
void Zone::setTypeId(int tId){
	typeId = tId;
}

// Devuelve el conjunto de puntos delimitador de zona.
vector<Point>* Zone::getShape(){
	return shape;
}

// Permite modificar el delimitador de zona.
void Zone::setShape(vector<Point>* s){
	shape = s;
}

// Coloca una mazmorra. Ricky: al final no recuerdo qu decidimos si les pasabamos tanta informacion o no.
void Zone::placeDungeon(vector<int>* idTools,int dungNumber, int gameDiff,int typeId, vector<int>* keyObjects, int dungSize, int ratio,
										vector<int>* idBosses, vector<int>* idEnemies, Point pos, vector<int>* idMiniBosses)
{
	cout << "Ejecutando funci�n <>Zone::placeDungeon()>" << endl;
}

// Por decidir, de primeras coloca la entrada a una zona segura. (Ricky: esto tendra tela)
void Zone::placeSafeZone(int idZone,Point* pos){
	cout << "Ejecutando funci�n <>Zone::placeSafeZone()>" << endl;
}

void Zone::genScreens(){
    int screensH = /*worldSizeH*/ 100 / 12;
    int screensW = /*worldSizeW*/ 100 / 16;
    int screens = screensH * screensW;

    int screenNumber = 0;
    int iniTileRow;
    int iniTile;

    for (int row = 0; row < screensH; row++){
        iniTileRow = screenWidth*screenHeight*screenNumber;
        for (int col = 0; col < screensW; col++){
            iniTile = col*screenWidth + iniTileRow;
            OwScreen* screen = new OwScreen(iniTile,new vector<MapTile>()/*Aqu� deber�a ir la mapTileMatrix*/, 
											screenNumber, new vector<MapTile>(), new vector <MapTile>());
            screen->placeDetails();
            screen->placeEnemies();
            string screenpath = screen->createScreenFiles(); //Habr� que meter estos strings en alg�n sitio, no???
            screenNumber++;
        }
    }
}


// Devuelve el n�mero de orden de la mazmorra que se encuentra en la zona
int Zone::getDungeonNumber(){
	return dungeonNumber;
}

// Establece un nuevo n�mero de orden de la mazmorra de la zona
void Zone::setDungeonNumber(int dunNum){
	dungeonNumber = dunNum;
}