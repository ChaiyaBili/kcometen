/***************************************************************************
 *   Copyright (C) 2005 by Peter Müller                                    *
 *   pmueller@cs.tu-berlin.de                                              *
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
 
#include "cometenscene.h"
#include "settings.h"
#include "pcomet.h"
#include "rotatecomet.h"
#include "curvecomet.h"
#include "lightning.h"

CometenScene::CometenScene(const GLuint* text,
                           double time, double boxWidth, double boxHeight) 
:
textures(text)
{
	resizeBox(boxWidth, boxHeight);
	lastCometProcess = time;
	nextComet = -10e20;
}

CometenScene::~ CometenScene() {
	list<Comet*>::iterator j;
	
	for (j = comets.begin(); j != comets.end(); j++) {
		delete (*j); 
		*j = 0;
	}
}

void CometenScene::resizeBox(double w, double h) {
	boxWidth = w;
	boxHeight = h;
	collisionPlanes.clear();
	collisionPlanes.push_back(Plane(Vec3f::UNIT_X, -boxWidth/2));
	collisionPlanes.push_back(Plane(-Vec3f::UNIT_X, -boxWidth/2));
	collisionPlanes.push_back(Plane(Vec3f::UNIT_Y, -boxHeight/2));
	collisionPlanes.push_back(Plane(-Vec3f::UNIT_Y, -boxHeight/2));
	collisionPlanes.push_back(Plane(Vec3f::UNIT_Z, -boxWidth/2));
	collisionPlanes.push_back(Plane(-Vec3f::UNIT_Z, -boxWidth/2));	
}

void CometenScene::process(double time) {
	list<Comet*>::iterator j, k;
	Comet *comet;
	double deltat = time - lastCometProcess;
	lastCometProcess = time;

	for (j = comets.begin(); j != comets.end();) {
		comet = *j;
		
		if (comet) {
			comet->process(time, deltat);
		}
		
		if (!comet || comet->isDone()) {
			delete comet;
			k = j;
			j++;
			comets.erase(k);
		}
		else {
			j++;
		}	
	}
	
	if (time > nextComet) {
		createComet(time);
		nextComet = time 
			+ frand(settings->createInterval*0.75,
			        settings->createInterval*1.25);
	}
}

void CometenScene::getCometColors(Vec4f& acolor, Vec4f& ecolor) {
	switch (settings->color) {
	case C_RED:
		acolor = Vec4f(1.0, 0.5, 0.25, 1.0);
		ecolor = Vec4f(1.0, 0.0, 0.0, 0.0);
		break;
	case C_BLUE:
		acolor = Vec4f(0.25, 0.5, 1.0, 1.0);
		ecolor = Vec4f(0.0, 0.0, 1.0, 0.0);
		break;
	case C_GREEN:
		acolor = Vec4f(0.5, 1.0, 0.25, 1.0);
		ecolor = Vec4f(0.0, 1.0, 0.0, 0.0);
		break;
	case C_EXTREMBUNT:
		acolor = Vec4f(frand(), frand(), frand(), 1.0);
		ecolor = Vec4f(frand(), frand(), frand(), 0.0);
		break;
	case C_BUNT:
	default:
		switch (rand() % 4) {
		case 2:
			acolor = Vec4f(1.0, 0.5, 0.25, 1.0);
			ecolor = Vec4f(1.0, 0.0, 0.0, 0.0);
			break;
		case 1:
			acolor = Vec4f(0.25, 0.5, 1.0, 1.0);
			ecolor = Vec4f(0.0, 0.0, 1.0, 0.0);
			break;
		case 0:
			acolor = Vec4f(0.5, 1.0, 0.25, 1.0);
			ecolor = Vec4f(0.0, 1.0, 0.0, 0.0);
			break;
		case 3:
		default:
			acolor = Vec4f(1.0, 1.0, 1.0, 1.0);
			ecolor = Vec4f(0.0, 0.0, 0.5, 0.0);
			break;
		}
	}
}

void CometenScene::createComet(double time) {
	Vec4f acolor, ecolor, acolor2, ecolor2;
	Comet * newc = 0;
	
	getCometColors(acolor, ecolor);
	
	double d = boxWidth < boxHeight ? boxWidth : boxHeight;
	Vec3f start = Vec3f::randomUnit() * d/3;
	
	double frandVal = frand();
	// 50%
	if (frandVal <= 0.5) {
		newc = new 
			QComet(this, time, frand(2.5, 10.5), start, 
			       Vec3f::randomUnit() * frand(75.0, 120.0),  false,
			       acolor, ecolor);
		
	}
	// 10%
	else if (frandVal <= 0.6) {
		newc = new 
			QComet(this, time, frand(2.5, 4.5), start, 
			       Vec3f::randomUnit() * frand(200.0, 300.0), false,
			       acolor, ecolor);
	}
	// 10%
	else if (frandVal <= 0.7) {
		if (settings->splitComet) {
			newc = new 
				QComet(this, time, frand(2.5, 4.5), start, 
				       Vec3f::randomUnit() * frand(75.0, 120.0),  true,
				       acolor, ecolor);
		}
		else {
			newc = new 
				QComet(this, time, frand(2.5, 10.5), start, 
				       Vec3f::randomUnit() * frand(50.0, 120.0),  false,
				       acolor, ecolor);
		}
	}
	// 10%
	else if (frandVal < 0.8) {
		if (settings->rotateComet) {
			getCometColors(acolor2, ecolor2);
			newc = new 
				RotateComet(this, time, frand(7.8, 9.5), start, 
				            Vec3f::randomUnit() * frand(80.0, 150.0), 
				            acolor, ecolor, acolor2, ecolor2);
		}
		else {
			newc = new 
				QComet(this, time, frand(2.5, 10.5), start, 
				       Vec3f::randomUnit() * frand(50.0, 120.0),  false,
				       acolor, ecolor);
		}
	} 
	// 17%
	else if (frandVal <= 0.97) {
		if (settings->curveComet) {
			newc = new 
				CurveComet(this, time, frand(1.5, 3.5), start, 
				           acolor, ecolor);
		}
		else {
			newc = new 
				QComet(this, time, frand(2.5, 10.5), start, 
				       Vec3f::randomUnit() * frand(50.0, 120.0),  false,
				       acolor, ecolor);
		}
	} 
	// 3%
	else {
		if (settings->blitz) {
			newc = new Blitz(this, time);
		}
		else {
			newc = new 
				QComet(this, time, frand(2.5, 10.5), start, 
				       Vec3f::randomUnit() * frand(50.0, 120.0),  false,
				       acolor, ecolor);
		}
	}
	
	addComet(newc);
}


void CometenScene::addComet(Comet* comet) {
	if (comet) {
		comets.push_back(comet);
	}
}

void CometenScene::render(const Vec3f& campos) {
	list<Comet*>::iterator i;
	
	renderBackground();
	
	for (i = comets.begin(); i != comets.end(); i++) {
		if (*i) {
			(*i)->render(campos);
		}
	}
}

void CometenScene::renderBackground() {
	double vertex_array[] = {
		// vorne
		boxWidth/2, -boxHeight / 2, boxWidth/2,
		boxWidth/2,  boxHeight / 2, boxWidth/2,
		-boxWidth/2,  boxHeight / 2, boxWidth/2,
		-boxWidth/2, -boxHeight / 2, boxWidth/2,	
		// hinten
		-boxWidth/2, -boxHeight / 2, -boxWidth/2,
		-boxWidth/2,  boxHeight / 2, -boxWidth/2,
		boxWidth/2,  boxHeight / 2, -boxWidth/2,
		boxWidth/2, -boxHeight / 2, -boxWidth/2,
		// links
		-boxWidth/2, -boxHeight / 2, boxWidth/2,
		-boxWidth/2,  boxHeight / 2, boxWidth/2,
		-boxWidth/2,  boxHeight / 2, -boxWidth/2,
		-boxWidth/2, -boxHeight / 2, -boxWidth/2,	
		// rechts
		boxWidth/2, -boxHeight / 2, -boxWidth/2,
		boxWidth/2,  boxHeight / 2, -boxWidth/2,	
		boxWidth/2,  boxHeight / 2, boxWidth/2,
		boxWidth/2, -boxHeight / 2, boxWidth/2,	
		// oben
		-boxWidth/2,  boxHeight / 2, -boxWidth/2,
		-boxWidth/2,  boxHeight / 2, boxWidth/2,	
		boxWidth/2,  boxHeight / 2, boxWidth/2,
		boxWidth/2,  boxHeight / 2, -boxWidth/2,	
		// unten
		-boxWidth/2, -boxHeight / 2, boxWidth/2,
		-boxWidth/2, -boxHeight / 2, -boxWidth/2,
		boxWidth/2, -boxHeight / 2, -boxWidth/2,
		boxWidth/2, -boxHeight / 2, boxWidth/2,
	};	
	
	static const double texcoords_array[] = {
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,
	};
	
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);		
	glTexCoordPointer(2, GL_DOUBLE, 0, texcoords_array);
	glVertexPointer(3, GL_DOUBLE, 0, vertex_array);
	glEnable(GL_CULL_FACE);
	glColor4f(0.7, 0.7, 0.7, 1.0);
	glBindTexture(GL_TEXTURE_2D, textures[T_BACKGROUND]);
	glDisable(GL_BLEND);	
	glDrawArrays(GL_QUADS, 0, 24);
	
	glEnable(GL_CULL_FACE);
	glTexCoordPointer(2, GL_DOUBLE, 0, texcoords_array);
	glColor4f(1.0, 1.0, 1.0, 0.99);
	glBindTexture(GL_TEXTURE_2D, textures[T_LIGHTMAP]);
	glEnable(GL_BLEND);	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_QUADS, 0, 24);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


	

	
	
	
	
	
	
	
	
