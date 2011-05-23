#pragma once

#ifndef _WORLDAUTOTILER_H_
#define _WORLDAUTOTILER_H_

#include "AutoTiler.h"

class WorldAutoTiler : public AutoTiler
{
	protected:

		// cargamos las decoraciones propias del mundo
		FILE* loadWorldConfig(FILE* f);

		// cargamos cada decoraci�n
		bool loadWorldDeco(FILE* f);

	public:
		// Constructora	
		WorldAutoTiler(std::string tileSetPath);

		// Destructora
		~WorldAutoTiler();

		// Devuelve una decoraci�n de entrada de mazmorra con tama�o = 6 tiles
		Decoration* getDecoDunEntrance(int floorId);
};

#endif