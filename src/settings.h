/***************************************************************************
 *   Copyright (C) 2008 by John Stamp <jstamp@users.sourceforge.net>       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

class Settings {
	public:
		Settings();
		void load();
		void defaults();
		void save();

		// Scene
		double fov;
		bool freeCamera;
		double freeCameraSpeed;
		double sizeScale;
		bool cube;
		bool slowMotion;
		bool matrix;
		int aspectRatio;
		int maxFps;
		bool mipmaps;
		// Comets
		int color;
		double createInterval;
		double timeScale;
		int particleDensity;
		bool rotateComet;
		bool splitComet;
		bool curveComet;
		bool blitz;
		// Background
		int bgType;
		QString bgFile;
		QString bgDir;
		int bgSize;
		// Runtime-only
		bool usePointSprites;
};

extern Settings* settings;

#endif
