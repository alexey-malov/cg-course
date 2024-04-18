#pragma once

struct Vector
{
	double x = 0;
	double y = 0;
};

inline Vector& operator*=(Vector& v, double s) noexcept
{
	v.x *= s;
	v.y *= s;
	return v;
}

inline Vector operator*(Vector v, double s) noexcept
{
	return v *= s;
}

inline Vector& operator+=(Vector& lhs, Vector rhs) noexcept
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

struct Point
{
	double x = 0;
	double y = 0;
};

inline Point& operator+=(Point& p, Vector v) noexcept
{
	p.x += v.x;
	p.y += v.y;
	return p;
}

inline Point operator+(Point p, Vector v) noexcept
{
	return p += v;
}

struct Particle
{
	Point position;
	Vector speed;
};

inline void Update(Particle& particle, double time, Vector acceleration) noexcept
{
	particle.position += particle.speed * time;
	particle.speed += acceleration * time;
}

struct Size
{
	double width = 0;
	double height = 0;
};

inline void Enclose(Particle& particle, Size size) noexcept
{
	if (particle.position.x < 0)
	{
		particle.position.x = 0;
		particle.speed.x = std::abs(particle.speed.x);
	}
	if (particle.position.x > size.width)
	{
		particle.position.x = size.width;
		particle.speed.x = -std::abs(particle.speed.x);
	}
	if (particle.position.y < 0)
	{
		particle.position.y = 0;
		particle.speed.y = std::abs(particle.speed.y);
	}
	if (particle.position.y > size.height)
	{
		particle.position.y = size.height;
		particle.speed.y = -std::abs(particle.speed.y);
	}
}

class World
{
public:
	World(int numParticles, Size size, Vector speedRange)
		: m_size{ size }
	{
		m_particles.reserve(numParticles);

		std::mt19937 gen;
		std::uniform_real_distribution xDist{ 0.0, size.width };
		std::uniform_real_distribution yDist{ 0.0, size.height };
		std::uniform_real_distribution speedXDist{ -speedRange.x, +speedRange.x };
		std::uniform_real_distribution speedYDist{ -speedRange.y, +speedRange.y };

		for (int i = 0; i < numParticles; ++i)
		{
			Point position{ xDist(gen), yDist(gen) };
			Vector speed{ speedXDist(gen), speedXDist(gen) };
			m_particles.emplace_back(position, speed);
		}
	}

	Size GetSize() const noexcept
	{
		return m_size;
	}

	auto GetParticles() const noexcept
	{
		return std::ranges::subrange(m_particles);
	}

	void Update(double time, Vector acceleration)
	{
		for (auto& particle : m_particles)
		{
			::Update(particle, time, acceleration);
			Enclose(particle, m_size);
		}
	}

private:
	std::vector<Particle> m_particles;
	Size m_size;
};