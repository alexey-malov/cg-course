// ParticleSystem.cpp: implementation of the ParticleSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "../libgl/StdAfx.h"
#include "ParticleSystem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define MAX_LIFE_TIME 5.0f
#define MIN_LIFE_TIME 2.0f

ParticleSystem::ParticleSystem(size_t maxParticles)
:m_maxParticles(maxParticles)
,m_particleSize(0.15f)
{
	m_particles.resize(maxParticles);
	for (size_t index = 0; index < maxParticles; ++index)
	{
		InitParticle(index);
	}

}

ParticleSystem::~ParticleSystem()
{

}

void ParticleSystem::Process(float time)
{
	for (size_t index = 0; index < m_maxParticles; ++index)
	{
		Particle &particle = m_particles[index];
		if (!particle.Live(time))
		{
			InitParticle(index);
		}
		particle.GetPosition() += particle.GetSpeed() * time;

		if (
			(particle.GetSpeed().y < 0) &&
			(particle.GetPosition().y < m_particleSize*0.5f)
			)
		{
			particle.GetPosition().y = m_particleSize*0.5f;
			particle.GetSpeed().y *= -0.8f;
		}
		particle.GetSpeed() += particle.GetAcceleration() * time;

	}
}

void ParticleSystem::InitParticle(size_t index)
{
	Particle &particle = m_particles[index];
	particle.SetLifeTime(GetRandomFloat(MIN_LIFE_TIME, MAX_LIFE_TIME));
	particle.SetPosition(Vector3d(GetRandomFloat(-0.3f, 0.3f), 0, GetRandomFloat(-0.3f, 0.3f)));
	particle.SetSpeed(Vector3d(GetRandomFloat(-0.3f, 0.3f), GetRandomFloat(0.2f, 4), GetRandomFloat(-0.3f, 0.3f)));
	particle.SetAcceleration(Vector3d(0, -2.8f, 0));
}

void ParticleSystem::Draw()
{
	float modelview[4][4];
	glGetFloatv(GL_MODELVIEW_MATRIX, &modelview[0][0]);
	
	float sizeX2 = m_particleSize * 0.5f;
	float sizeY2 = m_particleSize * 0.5f;
	// извлекаем направление координатных осей из матрицы моделирования-вида 
	// и умножаем jcb x и y на половину размера бильборда
	Vector3d xAxis(modelview[0][0]*sizeX2, modelview[1][0]*sizeX2, modelview[2][0]*sizeX2);
	Vector3d yAxis(modelview[0][1]*sizeY2, modelview[1][1]*sizeY2, modelview[2][1]*sizeY2);
	Vector3d zAxis(modelview[0][2], modelview[1][2], modelview[2][2]);
	
	// вектора смещения четырех вершин бильборда относительно его центра
	Vector3d p0(-xAxis.x + yAxis.x, -xAxis.y + yAxis.y, -xAxis.z + yAxis.z);
	Vector3d p1(-xAxis.x - yAxis.x, -xAxis.y - yAxis.y, -xAxis.z - yAxis.z);
	Vector3d p2(+xAxis.x - yAxis.x, +xAxis.y - yAxis.y, +xAxis.z - yAxis.z);
	Vector3d p3(+xAxis.x + yAxis.x, +xAxis.y + yAxis.y, +xAxis.z + yAxis.z);

	Vector3d v0, v1, v2, v3;
	glBegin(GL_QUADS);
	{
		Vector2d t0(0, 0);
		Vector2d t1(0, 1);
		Vector2d t2(1, 1);
		Vector2d t3(1, 0);

		for (size_t index = 0; index < m_maxParticles; ++index)
		{
			Particle &particle = m_particles[index];
			Vector3d &pos = particle.GetPosition();
			Vector3d v0 = pos + p0;
			Vector3d v1 = pos + p1;
			Vector3d v2 = pos + p2;
			Vector3d v3 = pos + p3;

			float bright = (particle.GetLifeTime() + MIN_LIFE_TIME * 0.5f) / MAX_LIFE_TIME;
			Color4d particleColor = {bright, bright, bright, 1};
			particleColor.SetOpenGLColor();

			t0.DeclareTextureCoord();
			v0.DeclareOpenGLVertex();

			t1.DeclareTextureCoord();
			v1.DeclareOpenGLVertex();

			t2.DeclareTextureCoord();
			v2.DeclareOpenGLVertex();

			t3.DeclareTextureCoord();
			v3.DeclareOpenGLVertex();
		}
	}
	glEnd();
}
