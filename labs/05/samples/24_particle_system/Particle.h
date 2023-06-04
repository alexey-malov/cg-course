// Particle.h: interface for the Particle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTICLE_H__FCFE6468_3BE1_4C74_B2A1_69AA274B12AE__INCLUDED_)
#define AFX_PARTICLE_H__FCFE6468_3BE1_4C74_B2A1_69AA274B12AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../libgl/Utils.h"

class Particle  
{
public:
	bool Live(float time);
	void SetPosition(Vector3d const& pos)
	{
		m_position = pos;
	}
	void SetSpeed(Vector3d const& spd)
	{
		m_speed = spd;
	}
	void SetAcceleration(Vector3d const& acc)
	{
		m_acceleration = acc;
	}
	
	Vector3d & GetPosition()
	{
		return m_position;
	}
	float GetLifeTime()const
	{
		return m_lifeTime;
	}
	void SetLifeTime(float lifeTime)
	{
		m_lifeTime = lifeTime;
	}
	Vector3d & GetSpeed()
	{
		return m_speed;
	}
	Vector3d & GetAcceleration()
	{
		return m_acceleration;
	}
	Particle (Particle const& other)
		:m_position(other.m_position)
		,m_speed(other.m_speed)
		,m_acceleration(other.m_acceleration)
	{
	}
	Particle();
	virtual ~Particle();
private:
	Vector3d	m_position;
	Vector3d	m_speed;
	Vector3d	m_acceleration;
	float		m_lifeTime;			// оставшееся время жизни частицы
};

#endif // !defined(AFX_PARTICLE_H__FCFE6468_3BE1_4C74_B2A1_69AA274B12AE__INCLUDED_)
