
// Check if the point p is on the left side of the line p0p1
bool PointIsOnTheLeft(vec2 p0, vec2 p1, vec2 p)
{
	vec2 p0p1 = p1 - p0;
	// find the orthogonal vector to p0p1
	vec2 n = vec2(-p0p1.y, p0p1.x);
	
	// Find the dot product between n and (p - p0)
	return dot(p - p0, n) > 0.0;
}

bool PointIsInsideTriangle(vec2 p0, vec2 p1, vec2 p2, vec2 p)
{
	return
		PointIsOnTheLeft(p0, p1, p) && 
		PointIsOnTheLeft(p1, p2, p) && 
		PointIsOnTheLeft(p2, p0, p);
}

void main()
{
	vec2 pos = gl_TexCoord[0].xy;
	
	const vec2 p0 = vec2(1.0, 1.0);
	const vec2 p1 = vec2(3.0, 2.0);
	const vec2 p2 = vec2(2.5, 3.5);
	
	if (PointIsInsideTriangle(p0, p1, p2, pos))
	{
		gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
	else
	{
		// determine whether fraction part of x and y 
		// texture coordinate is greater than 0.5
		vec2 stepXY = step(vec2(0.5, 0.5), fract(pos));

		// determine whether the texture coordinate 
		// is within a black or white check
		gl_FragColor = vec4((stepXY.x != stepXY.y) ? 1.0 : 0.0);
	}
}
