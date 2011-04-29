#include "DunScreen.h"

DunScreen::DunScreen(short posX, short posY, short puzzle, short n_enemies, short boss, short miniboss, short tool, string zone, string theme, DBManager* db, short mapNumber) 
	: Screen(mapNumber, posX, posY, n_enemies, zone, theme, db) {
	this->boss = boss;
	this->miniboss = miniboss;
	this->tool = tool;
	this->puzzle = puzzle;
	keyObj = -1;

	wall_size = 2;
	// Posici�n inicial del personaje en la pantalla
	posIniX = wall_size;
	posIniY = wall_size;

	// Id de la m�sica de fondo
	idMusic = 0;

	key = false;
	boss_key = false;
	empty_room = false;

	// Inicializa puertas, bloqueos, tiles y s�lidos
	for (int i = 0; i < 4; i++) {
		door[i] = false;
		lock[i] = false;
		boss_lock[i] = false;
		lockId[i] = -1;
	}
}

DunScreen::~DunScreen() {
	// Todo es est�tico de momento
}

void DunScreen::generate() {
	n_puzzles = 0;		// Mientras no tengamos puzzles
	n_tilesFG = 0;		// Mientras no tengamos tiles de foreground
	n_entities = 0;		// Se ir� incrementando seg�n se vayan a�adiendo

	decorate();			// Decora la mazmorra con s�lidos
	placePuzzle();		// Coloca las entidades asociadas al puzzle de la pantalla (de haberlo)
	placeEntities();	// Coloca las entidades de la pantalla
	placeEnemies();		// Coloca los enemigos de la pantalla
}

void DunScreen::print_screen() {
	for (int j = 0; j < SCREEN_HEIGHT; j++) {
		for (int i = 0; i < SCREEN_WIDTH; i++) {
			if (solids[i][j] == 0) printf(".");
			if (solids[i][j] == 1) printf("#");
		}
		printf("\n");
	}
}

void DunScreen::setDoor(short i) {
	if (i >= 0 && i < 4)
		door[i] = true;
}

void DunScreen::setLock(short i) {
	if (i >= 0 && i < 4)
		lock[i] = true;
}

void DunScreen::setLock(short d, short id) {
	if (d >= 0 && d < 4) {
		lock[d] = true;
		lockId[d] = id;
	}
}

void DunScreen::unSetDoor(short i) {
	if (i >= 0 && i < 4)
		door[i] = false;
}

void DunScreen::unSetLock(short i) {
	if (i >= 0 && i < 4){
		lock[i] = false;
		lockId[i] = -1;
	}
}

void DunScreen::setBoss_lock(short d) {
	if (d >= 0 && d < 4)
		boss_lock[d] = true;
}

void DunScreen::setBoss_lock(short d, short id) {
	if (d >= 0 && d < 4) {
		boss_lock[d] = true;
		lockId[d] = id;
	}
}

bool DunScreen::has_lock() {
	return (lock[0] || lock[1] || lock[2] || lock[3]);
}

bool DunScreen::has_one_door() {
	return ((door[0] && !door[1] && !door[2] && !door[3]) ||
		(!door[0] && door[1] && !door[2] && !door[3]) ||
		(!door[0] && !door[1] && door[2] && !door[3]) ||
		(!door[0] && !door[1] && !door[2] && door[3]));
}

void DunScreen::placePuzzle() {}

void DunScreen::placeWalls() {
	// De momento los tiles que coloca son 0 (suelo) o 1 (s�lido)
	// Arriba
	for (int j = 0; j < SCREEN_WIDTH; j++)
		for (int k = 0; k < wall_size; k++)
			if (!(j == SCREEN_WIDTH / 2 || j == (SCREEN_WIDTH - 1) / 2) || !door[UP]) {
				solids[j][k] = 1;
				tiles[j][k] = 1;
			}

	// Abajo
	for (int j = 0; j < SCREEN_WIDTH; j++)
		for (int k = SCREEN_HEIGHT - wall_size; k < SCREEN_HEIGHT; k++)
			if (!(j == SCREEN_WIDTH / 2 || j == (SCREEN_WIDTH - 1) / 2) || !door[DOWN]) {
				solids[j][k] = 1;
				tiles[j][k] = 1;
			}

	// Izquierda
	for (int j = 0; j < wall_size; j++)
		for (int k = 0; k < SCREEN_HEIGHT; k++)
			if (!(k == SCREEN_HEIGHT / 2 || k == (SCREEN_HEIGHT - 1) / 2) || !door[LEFT]) {
				solids[j][k] = 1;
				tiles[j][k] = 1;
			}
			
	// Derecha
	for (int j = SCREEN_WIDTH - wall_size; j < SCREEN_WIDTH; j++)
		for (int k = 0; k < SCREEN_HEIGHT; k++)
			if (!(k == SCREEN_HEIGHT / 2 || k == (SCREEN_HEIGHT - 1) / 2) || !door[RIGHT]) {
				solids[j][k] = 1;
				tiles[j][k] = 1;
			}

}

void DunScreen::decorate() {
	// Construye las paredes y puertas de la mazmorra
	placeWalls();
	// Si la habitaci�n es vac�a no hacemos nada m�s
	if (!empty_room) {
		// Primero elige un tipo de simetr�a
		sym_type = rand() % 3 + 1;	// No queremos sin simetr�a, que queda muy feo
		// Coloca elementos en los cuadrantes apropiados
		genQuadrants();
		genSymmetry();
	}
}

void DunScreen::genQuadrants() {
    switch (sym_type) {
        case NO_SYM:
            genQuadrant(0);
            genQuadrant(1);
            genQuadrant(2);
            genQuadrant(3);
            break;
        case HOR_SYM:
            genQuadrant(0);
            genQuadrant(1);
            break;
        case VER_SYM:
            genQuadrant(0);
            genQuadrant(2);
            break;
        case TOT_SYM:
            genQuadrant(0);
            break;
    }
}

void DunScreen::genQuadrant(short q) {
    // Cuadrantes:
    // 0 - superior izquierda
    // 1 - superior derecha
    // 2 - inferior izquierda
    // 3 - inferior derecha
    short qW = (SCREEN_WIDTH - wall_size*2) / 2;    // Ancho del cuadrante
    short qH = (SCREEN_HEIGHT - wall_size*2) / 2;   // Alto del cuadrante
    short iniX = wall_size + qW * (q % 2);          // Posici�n en la que comienza el cuadrante q (X)
    short iniY = wall_size + qH * (q / 2);          // Posici�n en la que comienza el cuadrante q (Y)
    short total_tiles = qW * qH;                    // Area en tiles disponible en el cuadrante
    short n_solids = rand() % (total_tiles / 4);    // Cantidad de s�lidos a colocar (de momento la cuarta parte de la superficie, m�ximo)

    // Coloca los s�lidos en el cuadrante
    for (int i = 0; i < n_solids; i++) {
        // Escoge una localizaci�n v�lida en el cuadrante
        int x, y, s;
        do {
            x = rand() % qW;
            y = rand() % qH;
            s = solids[iniX+x][iniY+y];
        } while (s != 0 || blocksDoor(iniX+x, iniY+y));

        // Coloca el s�lido en dicha habitaci�n
        solids[iniX+x][iniY+y] = 1;
		tiles[iniX+x][iniY+y] = 1;
    }
}

void DunScreen::genSymmetry() {
    switch (sym_type) {
        case HOR_SYM:
            genHorSymmetry();
            break;
        case VER_SYM:
            genVerSymmetry();
            break;
        case TOT_SYM:
            genHorSymmetry();
            genVerSymmetry();
            break;
    }
}

void DunScreen::genHorSymmetry() {
    int k = SCREEN_HEIGHT - wall_size - 1; // �ndice sim�trico a j

    for (int i = wall_size; i < (SCREEN_WIDTH - wall_size); i++) {
        for (int j = wall_size; j < (SCREEN_HEIGHT / 2); j++) {
            solids[i][k] = solids[i][j];
            tiles[i][k] = tiles[i][j];
            k--;    // k retrocede una posici�n a la vez que j avanza una
        }
        // Restaura el valor original de k en cada iteraci�n
        k = SCREEN_HEIGHT - wall_size - 1;
    }
}

void DunScreen::genVerSymmetry() {
    int k = SCREEN_WIDTH - wall_size - 1; // �ndice sim�trico a i

    for (int i = wall_size; i < (SCREEN_WIDTH / 2); i++) {
        for (int j = wall_size; j < (SCREEN_HEIGHT - wall_size); j++) {
            solids[k][j] = solids[i][j];
            tiles[k][j] = tiles[i][j];
        }
        // k retrocede una posici�n a la vez que i avanza una
        k--;
    }
}

bool DunScreen::blocksDoor(short x, short y) {
    // �Bloquea la puerta de arriba?
    if ((x == SCREEN_WIDTH / 2 - 1 || x == SCREEN_WIDTH / 2) && y == wall_size && door[UP]) return true;
    // �Bloquea la puerta de abajo?
    if ((x == SCREEN_WIDTH / 2 - 1 || x == SCREEN_WIDTH / 2) && y == SCREEN_HEIGHT - wall_size - 1 && door[DOWN]) return true;
    // �Bloquea la puerta de la izquierda?
    if (x == wall_size && (y == SCREEN_HEIGHT / 2 - 1 || y == SCREEN_HEIGHT / 2) && door[LEFT]) return true;
    // �Bloquea la puerta de la derecha?
    if (x == SCREEN_WIDTH - wall_size - 1 && (y == SCREEN_HEIGHT / 2 - 1 || y == SCREEN_HEIGHT / 2) && door[RIGHT]) return true;

    return false;
}

void DunScreen::placeEntities() {
	// Colocamos los bloqueos
	int x, y, s;
	for (int d = 0; d < 4; d++) {
		switch (d) {
			case UP:
				x = SCREEN_WIDTH / 2 - 1;
				y = wall_size - 1;
				break;
			case DOWN:
				x = SCREEN_WIDTH / 2 - 1;
				y = SCREEN_HEIGHT - wall_size;
				break;
			case LEFT:
				x = wall_size - 1;
				y = SCREEN_HEIGHT / 2 - 1;
				break;
			case RIGHT:
				x = SCREEN_WIDTH - wall_size;
				y = SCREEN_HEIGHT / 2 - 1;
				break;
		}
		if (lock[d]) {
			EntityDoor* e = new EntityDoor(DOOR, x, y, lockId[d], -1, idTileSet);
			entities->push_back(e);	// A�adimos la nueva entidad al vector de entidades
			n_entities++;
		}
		if (boss_lock[d]) {
			EntityBossDoor* e = new EntityBossDoor(BOSSDOOR, x, y, lockId[d], -1, idTileSet);
			entities->push_back(e);
			n_entities++;
		}
	}

	// Los idCollectables del resto de entidades se asignan a nivel de mazmorra
	// Colocamos la llave o la llave del jefe, si la hay
	if (key || boss_key) {
		EntityItem* e = new EntityItem(ITEM, x, y, -1, -1, -1, -1, 1);
		do {
			x = (rand() % (SCREEN_WIDTH - wall_size*2)) + wall_size;
			y = (rand() % (SCREEN_HEIGHT - wall_size*2)) + wall_size;
            s = solids[x][y];
        } while (s != 0 || blocksDoor(x, y));
		e->x = x; e->y = y;
		if (key) {
			e->effect = KEY;
			e->gfxId = db->getKeyGfxId();
		}
		if (boss_key) {
			e->effect = BOSSKEY;
			e->gfxId = db->getBossKeyGfxId();
		}
		entities->push_back(e);
		n_entities++;
	}

	// Colocamos la herramienta de la mazmorra
	if (tool >= 0) {
		do {
			x = (rand() % (SCREEN_WIDTH - wall_size*2)) + wall_size;
			y = (rand() % (SCREEN_HEIGHT - wall_size*2)) + wall_size;
            s = solids[x][y];
        } while (s != 0 || blocksDoor(x, y));
		EntityTool* e = new EntityTool(TOOL, x, y, -1, -1, tool);
		entities->push_back(e);
		n_entities++;
	}

	// Colocamos el objeto clave y el power up (y un teletransporte a la entrada de la mazmorra?)
	if (keyObj >= 0) {

		EntityItem* e1 = new EntityItem(ITEM, SCREEN_WIDTH / 2 - 2, SCREEN_HEIGHT / 2 - 1, -1, -1, db->getGfxId("KeyItems", keyObj), KEY, 1); 
		entities->push_back(e1);
		n_entities++;

		short idPowUp = db->getPowUp(theme);
		// Comprobamos que en la base de datos hab�a alg�n power up v�lido
		if (idPowUp >= 0) {
			EntityItem* e2 = new EntityItem(ITEM, SCREEN_WIDTH / 2 + 1, SCREEN_HEIGHT / 2 - 1, -1, -1, db->getGfxId("PowUps", idPowUp), db->getPowUpEffect(idPowUp), 1);
			entities->push_back(e2);
			n_entities++;
		}
	}
}

void DunScreen::placeEnemies() {
	// Coloca los s�lidos en el cuadrante
	short e = -1;
    for (int i = 0; i < n_enemies; i++) {
		// Pide un enemigo v�lido a la interfaz con la base de datos
		e = db->getEnemy(zone, theme);
        // Escoge una localizaci�n v�lida en la habitaci�n
        short x, y, s;
        do {
			x = (rand() % (SCREEN_WIDTH - wall_size*2)) + wall_size;
			y = (rand() % (SCREEN_HEIGHT - wall_size*2)) + wall_size;
            s = solids[x][y];
        } while (s != 0 || blocksDoor(x, y));
		// Al salir del bucle, x e y representan una posici�n v�lida para el enemigo e
		enemy en;
		en.id = e;
		en.posX = x;
		en.posY = y;
		// Guardamos el nuevo enemigo en el vector de enemigos
		enemies->push_back(en);
	}
	// Si no hab�a ning�n enemigo v�lido, deshacemos lo hecho
	if (e == -1) {
		delete enemies;
		enemies = new vector<enemy>();
		n_enemies = 0;
	}
}

void DunScreen::placeTeleporter(short idMap, short screenX, short screenY, short tileX, short tileY) {
	EntityTeleporter* e = new EntityTeleporter(TELEPORTATOR, -1, -1, -1, -1, idMap, screenX, screenY, tileX, tileY);
	// Buscamos una posici�n v�lida en la habitaci�n
	short x, y, s;
    do {
		x = (rand() % (SCREEN_WIDTH - wall_size*2)) + wall_size;
		y = (rand() % (SCREEN_HEIGHT - wall_size*2)) + wall_size;
        s = solids[x][y];
	} while (s != 0 || blocksDoor(x, y));
	// Colocamos el teletransporte en dicha habitaci�n
	e->x = x;
	e->y = y;
	// A�adimos el teletransporte al vector de entidades
	entities->push_back(e);
}

short DunScreen::getSym_type() { return sym_type; }
short DunScreen::getWall_size() { return wall_size; }
bool DunScreen::getDoor(short d) { return door[d]; }
bool DunScreen::getLock(short l) { return lock[l]; }
short DunScreen::getPuzzle() { return puzzle; }
short DunScreen::getBoss() { return boss; }
short DunScreen::getMiniBoss() { return miniboss; }
short DunScreen::getTool() { return tool; }

void DunScreen::setWall_size(short wall_size) { this->wall_size = wall_size; }
void DunScreen::setKey() { key = true; }
void DunScreen::setBoss_key() { boss_key = true; }
void DunScreen::setBoss(short boss) { this->boss = boss; }
void DunScreen::setTool(short tool) { this->tool = tool; }
void DunScreen::setEmpty_room(short empty_room) { this->empty_room = empty_room; }
void DunScreen::setKeyObj(short keyObj) { this->keyObj = keyObj; }