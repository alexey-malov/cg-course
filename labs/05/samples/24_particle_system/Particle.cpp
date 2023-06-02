// Particle.cpp: implementation of the Particle class.
//
//////////////////////////////////////////////////////////////////////

#include "../libgl/StdAfx.h"
#include "Particle.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Particle::Particle()
:m_position(0, 0, 0)
,m_lifeTime(0)
,m_speed(0, 0, 0)
{

}

Particle::~Particle()
{

}

bool Particle::Live(float time)
{
	m_lifeTime -= time;
	if (m_lifeTime <= 0)
	{
		return false;
	}
	return true;
}
