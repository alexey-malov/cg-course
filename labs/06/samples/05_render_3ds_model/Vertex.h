#pragma once

struct Vector3
{
	float x, y, z;
};

struct Vector2
{
	float x, y;
};

struct Vertex
{
	Vector3 position;
	Vector3 normal;
};

struct TexturedVertex : public Vertex
{
	Vector2 texCoord;
};
