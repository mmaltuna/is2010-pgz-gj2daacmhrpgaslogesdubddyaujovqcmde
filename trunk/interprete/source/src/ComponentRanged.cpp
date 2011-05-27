#include "ComponentRanged.h"

// Esto tendra que recibirlo de algun lado
#define IMG_WIDTH 16    // ancho de la imagen
#define IMG_HEIGHT 16    // alto de la imagen

ComponentRanged::ComponentRanged(Game* game, Controller* cont) : Component()
{
	this->cont = cont;
	this->game = game;
};

ComponentRanged::~ComponentRanged()
{
};

void ComponentRanged::onCInit(Enemy* e)
{
	// Comenzamos en una direccion random y estado Normal
	e->dir = (Direction) ((rand() % 4) +1);
	state = Standing;
	resting = false;

	// Cambiamos la configuraci�n por defecto de los flags que nos interesan
	e->solid = false;

	// Creamos un EnemyTool
	eToolKameha = new EnemyTool(e->x, e->y, game, e->world);
	eToolKameha->init(e, 0, "data/basic-gfx/enemyTool-rock.png");

	eToolKameha->setCoolDown(20);
	eToolKameha->setRange(0); // distancia infinita, no hace falta ponerlo
	eToolKameha->setTravelSpeed(3);
};

void ComponentRanged::onCStep(Enemy* e)
{
	int xtemp, ytemp;

	Direction dir = NONE;

	int nx = 0; int ny = 0;
	Player* p = NULL;
		int tmpx, tmpy;
	bool out;
	switch (state)
	{
		/* ********************** Standing ************************* */
		case Standing:
			if(!resting){
				e->setTimer(5, rand()%15 + 15);
				resting = true;
			}
			break;

		/* ********************** Walking ************************* */
		case Walking:
			if (rand()%100 < turnRatio){
				e->dir = getDifDir(e->dir);
			}
			moveInDir(e, moveSpeed);

			for (int i = 0; i < cont->getNumPlayers(); i++)
			{
				p = cont->getPlayer(i);
				// comprueba si est� cerca y en qu� direcci�n, y un 50% de las veces le dispara
				if (checkPlayerNear(p, e, searchDist, dir) && (rand()%2< 1)){
					e->dir = dir;
					state = Attacking;
					e->setTimer(3,10);
				}
			}
			break;

		/* ********************** Damaged ************************* */
		case ReceivingDamage:
			xtemp = e->x; 
			ytemp = e->y;

			// Bounce del enemy 
			if (e->getLastHitDirection() == UP) ytemp += e->getTimer(1)/2;
			else if (e->getLastHitDirection() == DOWN) ytemp -= e->getTimer(1)/2;
			else if (e->getLastHitDirection() == LEFT) xtemp += e->getTimer(1)/2;
			else if (e->getLastHitDirection() == RIGHT) xtemp -= e->getTimer(1)/2;
			else if (e->getLastHitDirection() == UPLEFT) ytemp += e->getTimer(1)/2, xtemp += e->getTimer(1)/2;
			else if (e->getLastHitDirection() == UPRIGHT) ytemp += e->getTimer(1)/2, xtemp -= e->getTimer(1)/2;
			else if (e->getLastHitDirection() == DOWNLEFT) ytemp -= e->getTimer(1)/2, xtemp += e->getTimer(1)/2;
			else if (e->getLastHitDirection() == DOWNRIGHT) ytemp -= e->getTimer(1)/2, xtemp -= e->getTimer(1)/2;

			out = true;
			tmpx = e->mask->x; 
			tmpy = e->mask->y;

			e->mask->y = ytemp; 
			e->mask->x = xtemp;

			// Miramos a ver si seguimos en territorio pantallil
			cont->getScreenMap()->relative_position(e,out);
        

			e->mask->x = tmpx; 
			e->mask->y = tmpy;

			// Y corregimos apropiadamente
			if (out)
			{
				e->setTimer(1,0);
			}
			else
			{
				// Actualizamos posici�n
				if (e->world->place_free(e->x, ytemp, e))
					e->y = ytemp; 
				else
					e->world->moveToContact(e->x,ytemp, e);

				if (e->world->place_free(xtemp, e->y, e))
					e->x = xtemp; 
				else
					e->world->moveToContact(xtemp,e->y, e);
			}
			break;

		/* ********************** Attacking ************************* */
		case Attacking:
			eToolKameha->activate();
			break;
	};
	
	e->graphic->setColor(Color::White);
	e->graphic->setAlpha(1);

	// Graphic settings
	switch (state)
	{
	case Standing:
		e->currentAnim = STAND;
		break;
	case Walking:
		e->currentAnim = WALK;
		break;
	case ReceivingDamage:
		e->currentAnim = DAMAGED;
		break;
	case Attacking:
		e->currentAnim = ATKRANGED;
	}
};

void ComponentRanged::onCCollision(Enemy* enemy, CollisionPair other, Entity* e)
{
	if (other.b == "player")
	{
		enemy->damagePlayer((Player*) e, enemy->strength, 255);
		/*((Player*) e)->setLastHitDirection(((Player*) e)->computeHitDirection(enemy, e));
		((Player*) e)->onDamage(5, 0x1);*/
	}
	
	else if (other.b == "tool")
	{
		if (state != ReceivingDamage)
		{
			// Este da�o lo har� el arma que nos pega
			state = ReceivingDamage;
			enemy->setTimer(1, 10);
		}
	}
	else if (other.b == "enemy")
	{
		switch(enemy->dir)
		{
			case UP:
				enemy->y += moveSpeed;
				break;
			case DOWN:
				enemy->y -= moveSpeed;
				break;
			case LEFT:
				enemy->x += moveSpeed;
				break;
			case RIGHT:
				enemy->x -= moveSpeed;
				break;
		}
	}
};

void ComponentRanged::onCTimer(Enemy* e, int timer)
{
	// timer de bounce al ser damageado
	if (timer == 1)
	{
		if (state == ReceivingDamage)
			if (!e->dead)
				state = Standing;
	};

	// timer de desaparecer
	if (timer == 2)
		e->instance_destroy();

	// timer de la animacion al colisionar con player
	if (timer == 3)
	{
		state = Standing;
		resting = false;
	}

	// timer de estar walking
	if (timer == 5){
		if (rand()%4 < 1)
		// random attack
		{
			state = Attacking;
			e->setTimer(3,10);
		}
		else
		{
			state = Walking;
			e->setTimer(6, rand()%25 + 65);		
		}
	}

	// timer de estar standing
	if (timer == 6){
		state = Standing;
		resting = false;
	}
};

bool ComponentRanged::checkPlayerNear(Player* p, Enemy* e, int dist, Direction & dir)
{
	// Solo comprobamos si estamos mirando hacia el player nos ahorramos sqrt

	//V�rtices entre los que se efectuar� la distancia

		// enemy
		int ex = 0, ey = 0;
		// player
		int px = 0, py = 0;


		// DISTANCIA HORIZONTAL
		// e a la izquierda de p
		if (p->x > e->x)
		{
			// enemy y player aproximadamente a distancia horizontal 0 (player dentro de enemy si ignoramos la y)
			if (p->x + p->mask->width < e->x + e->mask->width)
			{
				// no se computan
				ex = 0;
				px = 0;
			}
			else
			{
				// suponemos v�rtice derecho del enemy m�s cercano al izquierdo del player
				ex = e->x + e->mask->width;
				px = p->x;
			}
			dir = RIGHT;
		}
		// p a la izquierda de e
		else
		{
			// enemy y player aproximadamente a distancia horizontal 0 (enemy dentro de player si ignoramos la y)
			if (p->x + p->mask->width > e->x + e->mask->width)
			{
				// no se computan
				ex = 0;
				px = 0;
			}
			else
			{
				// v�rtice izquierdo del enemy m�s cerca del derecho del player
				ex = e->x;
				px = p->x + p->mask->width;
			}
			dir = LEFT;
		}

		// DISTANCIA VERTICAL

		// e encima de p
		if (p->y > e->y)
		{
			// enemy y player aproximadamente a distancia vertical 0 (player dentro de enemy si ignoramos la x)
			if (p->y + p->mask->height < e->y + e->mask->height)
			{
				// no se computan
				ey = 0;
				py = 0;
			}
			else
			{
				// suponemos v�rtice inferior del enemy m�s cercano al superior del player
				ey = e->y + e->mask->height;
				py = p->y;
			}

			// comprobaci�n cutre y r�pida de en qu� dir est� m�s cerca
			if (abs(e->x - p->x) > (abs(e->y - p->y)))
				dir = DOWN;
		}
		// p encima de e
		else
		{
			// enemy y player aproximadamente a distancia vertical 0 (enemy dentro de player si ignoramos la x)
			if (p->y + p->mask->height > e->y + e->mask->height)
			{
				// no se computan
				ey = 0;
				py = 0;
			}
			else
			{
				// v�rtice superior del enemy m�s cerca del inferior del player
				ey = e->y;
				py = p->y + p->mask->height;
			}
			dir = UP;

			if (abs(e->x - p->x) > (abs(e->y - p->y)))
				dir = UP;
		}

	switch (e->dir)
	{
		// Le sumo mask height|width para el ancho y el alto del player
		case UP:
			if (p->y <= e->y)
				return getDistance(ex, ey, px, py) < dist;
			break;
		case DOWN:
			if (p->y + p->mask->height >= e->y + e->mask->height)
				return getDistance(ex, ey, px, py) < dist;
			break;
		case LEFT:
			if (p->x <= e->x)
				return getDistance(ex, ey, px, py) < dist;
			break;
		case RIGHT:
			if (p->x + p->mask->width >= e->x + e->mask->width)
				return getDistance(ex, ey, px, py) < dist;
			break;
	}
	return false;
};

int ComponentRanged::getDistance(int x1, int y1, int x2, int y2)
{
	int sqr1, sqr2;
	sqr1 = (x2-x1)*(x2-x1);
	sqr2 = (y2-y1)*(y2-y1);
	return (int)sqrt((double)(sqr1+sqr2));
};

// Mueve al enemigo en la direccion en la que este mirando, devuelve si se choc� con algo
bool ComponentRanged::moveInDir(Enemy* e, int speed){
	int xtemp = e->x;
	int ytemp = e->y;
	bool outOfScreen = true, collided = false;

    // Miramos a ver si seguimos en territorio pantallil
	Direction dir = cont->getScreenMap()->relative_position(e,outOfScreen);
        
    // Y corregimos apropiadamente
	if (outOfScreen)
		if (dir == RIGHT){
			e->x -= speed;
			e->dir = LEFT;
		}
		else if(dir == LEFT){
			e->x += speed;
			e->dir = RIGHT;
		}
		else if(dir == UP){
			e->y += speed;
			e->dir = DOWN;
		}
		else if(dir == DOWN){
			e->y -= speed;
			e->dir = UP;
		}
	
	// Coord donde intentaremos movernos
	if (e->dir == RIGHT) xtemp += speed;
	if (e->dir == LEFT) xtemp -= speed;
	if (e->dir == UP) ytemp -= speed;
	if (e->dir == DOWN) ytemp += speed;

	Player* player;
	for (int i= 0; i<cont->getNumPlayers(); i++){
		player = cont->getPlayer(i);
		player->solid = false;
		player->collidable = false;
	}

	// Nos intentamos mover (el orden da igual), probamos placeFree si falla hacemos moveToContact
	// Primero vertical
	e->world->place_free(e->x, ytemp, e) ? e->y = ytemp : e->world->moveToContact(e->x, ytemp, e), collided = true;
	// Luego horizontal
	e->world->place_free(xtemp, e->y, e) ? e->x = xtemp : e->world->moveToContact(xtemp, e->y, e), collided = true; 
	
	for (int i= 0; i<cont->getNumPlayers(); i++){
		player = cont->getPlayer(i);
		player->solid = true;
		player->collidable = true;
	}


	return collided;
};

Direction ComponentRanged::getDifDir(Direction direc){
	Direction newDir = (Direction) ((rand() % 4) +1);
	while (direc == newDir)
		newDir = (Direction) ((rand() % 4) +1);
	return newDir;
};