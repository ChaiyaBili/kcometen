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
#include "particlesystem.h"
#include "settings.h"

static const GLfloat texCoords[] = {
	0.0, 1.0,
	1.0, 1.0,
	1.0, 0.0,
	0.0, 0.0
};

GLfloat* ParticleSystem::textureArr = 0;

ParticleSystem::ParticleSystem(double time, GLuint _texture,
                               const Vec3f & _emPos1,
                               const Vec3f & _emPos2,
                               int _maxParticle,
                               int numCreateAtStart,
                               double _minLifeTime,
                               double _maxLifeTime,
                               bool _createNewParticle, 
                               double _newParticlePerSec, 
                               double _minVel, double _maxVel,
                               bool _slowdown,
                               double _particleSize, 
                               const Vec4f & _startColor, 
                               const Vec4f & _endColor) :
emitterPos1(_emPos1),
emitterPos2(_emPos2),
minVelocity(_minVel),
maxVelocity(_maxVel),
slowdown(_slowdown),
particleSize(_particleSize),
minLifeTime(_minLifeTime),
maxLifeTime(_maxLifeTime),
createNewParticle(_createNewParticle),
newParticlePerSec(_newParticlePerSec),
startColor(_startColor),
endColor(_endColor),
maxParticle(_maxParticle),
texture(_texture)
{
	maxParticle = maxParticle * settings->particleDensity/100.0+0.5;
	newParticlePerSec = newParticlePerSec * settings->particleDensity/100.0+0.5;

	if ( !settings->usePointSprites )
	{
		colorArr = new Vec4f[maxParticle*4];
		vertArr = new Vec3f[maxParticle*4];
		if ( !textureArr )
			setupTextureCoordArray();
	}

    /* Intel cards seem to have memory problems with non-integer point sizes.
     * Since point antialiasing is disabled anyway, we can convert to int
     * without affecting the appearance.
     */
	double pointRange[2];
	glGetDoublev(GL_SMOOTH_POINT_SIZE_RANGE, pointRange);
	particleSize += 0.5;
	particleSize = clamp<int>(particleSize, pointRange[0], pointRange[1]);

	particleArr = new Particle[maxParticle];
	indexArr = new int[maxParticle];
	
	numCreateAtStart = min<int>(maxParticle, numCreateAtStart);
	numNewParticle = (double) numCreateAtStart;
	numActiveParticle = 0;
		
	for (int i = 0; i < maxParticle; i++) {
		particleArr[i].active = false;
	}
	
	lastUpdateTime = time;
}

ParticleSystem::~ParticleSystem() {
	delete[] particleArr;
	delete[] indexArr;

	if ( !settings->usePointSprites )
	{
		delete[] colorArr;
		delete[] vertArr;
	}
}

void ParticleSystem::setupTextureCoordArray()
{
	int stSize = sizeof(texCoords);
	int m = 1;
	textureArr = new GLfloat[800*8];
	GLfloat *tp = textureArr;
	
	memcpy( tp, texCoords, stSize );
	tp += 8;
	while (m < 512)
	{
		memcpy( tp, textureArr, stSize*m );
		tp += m*8;
		m *= 2;
	}
	memcpy( tp, textureArr, stSize*288 );
}

void ParticleSystem::process(double time) {
	Vec3f dir, up, right;
	Particle *p;
	double t;
	int j = 0;
	int k = 0;
	double deltat = time - lastUpdateTime;
	lastUpdateTime = time;

	if ( !settings->usePointSprites )
	{
		// billboarding
		dir = campos;
		dir.normalize();
		Vec3f::generateOrthonormalBasis(dir, up, right);
		right *= 0.5 * particleSize;
		up *= 0.5 * particleSize;
	}
	
	if (createNewParticle)
		numNewParticle += newParticlePerSec * deltat;
	
	for (int i = 0; i < maxParticle; i++) {
		p = particleArr + i;

		// particle die
		if ( (p->active) && (p->startTime + p->lifeTime < time) ) {
			p->active = false;
		}
		
		if (!p->active && numNewParticle < 1.0) {
			continue;
		}
		
		if (!p->active) {
			double q = frand();
			p->position = (emitterPos1 * q) + (emitterPos2 * (1-q));
			p->velocity = Vec3f::randomUnit() 
				* frand(minVelocity, maxVelocity);
			p->startTime = time;
			p->lifeTime = frand(minLifeTime, maxLifeTime);
			// This misbehaves when compiled with gcc-4.2:
			//p->lifeTime = max<double>(0.001, p->lifeTime);
			// so use this instead:
			p->lifeTime = 0.001 > p->lifeTime ? 0.001 : p->lifeTime;
			p->invLifeTime = 1/ p->lifeTime;
			p->active = true;
			numNewParticle -= 1.0;
			t = (time - p->startTime) * p->invLifeTime;
		} else {
			t = (time - p->startTime) * p->invLifeTime;
			if (slowdown) 
				p->position += p->velocity * deltat * (1-t);
			else
				p->position += p->velocity * deltat;
		}

		p->color = (startColor * (1-t)) + (endColor * t);

		if ( !settings->usePointSprites )
		{
			vertArr[k] = p->position + right + up;
			colorArr[k] = p->color;
			k++;

			vertArr[k] = p->position - right + up;
			colorArr[k] = p->color;
			k++;

			vertArr[k] = p->position - right - up;
			colorArr[k] = p->color;
			k++;

			vertArr[k] = p->position + right - up;
			colorArr[k] = p->color;
			k++;
		}

		indexArr[j] = i;
		j++;
	}
	
	numActiveParticle = j;
}

void ParticleSystem::render(const Vec3f & _campos) {
	campos = _campos;
	if ( settings->usePointSprites )
		renderPoint();
	else
		renderQuad();
}

void ParticleSystem::renderPoint() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
	
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glColorPointer(4, GL_FLOAT, sizeof(Particle), &particleArr[0].color);
	glVertexPointer(3, GL_FLOAT, sizeof(Particle), &particleArr[0].position);
	
	// pointparameter in cometenscene->init()

	glEnable(GL_POINT_SPRITE_ARB);
	glPointSize(particleSize);

	glDrawElements(GL_POINTS, numActiveParticle, GL_UNSIGNED_INT, indexArr);
	
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glDisable(GL_POINT_SPRITE_ARB);
	glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_FALSE);
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void ParticleSystem::renderQuad() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glColorPointer(4, GL_FLOAT, 0, colorArr);
	glVertexPointer(3, GL_FLOAT, 0, vertArr);
	glTexCoordPointer(2, GL_FLOAT, 0, textureArr);

	glDrawArrays(GL_QUADS, 0, numActiveParticle*4);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisable(GL_TEXTURE_2D);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void ParticleSystem::stop() {
	createNewParticle = false;
	numNewParticle = 0.0;
}

bool ParticleSystem::done() {
	return (!createNewParticle) && (numActiveParticle == 0);
}
