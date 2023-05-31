// ParticleSystem.h: interface for the ParticleSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTICLESYSTEM_H__F523B15C_5395_434F_81D6_624B379ADB61__INCLUDED_)
#define AFX_PARTICLESYSTEM_H__F523B15C_5395_434F_81D6_624B379ADB61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Particle.h"

class ParticleSystem  
{
public:
	void Draw();
	void InitParticle(size_t index);
	void Process(float time);
	ParticleSystem(size_t maxParticles);
	virtual ~ParticleSystem();
private:
	size_t	m_maxParticles;
	std::vector<Particle>	m_particles;
	float	m_particleSize;

};

#endif // !defined(AFX_PARTICLESYSTEM_H__F523B15C_5395_434F_81D6_624B379ADB61__INCLUDED_)
