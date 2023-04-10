typedef float4 point; // x, y, z – the w is unused
typedef float4 vector; // vx, vy, vz – the w is unused
typedef float4 color; // r, g, b – the w is unused
typedef float4 sphere; // xc, yc, zc, r
// despite what we think of the 4 components as representing,
// they are all referenced as .x, .y, .z, and .w
constant float4 G = (float4) ( 0., -9.8, 0., 0. ); // gravity
constant float DT = 0.1; // time step
constant sphere Sphere1 = (sphere)( -100., -800., 0., 600. ); // xc. yc, zc, r

bool IsInsideSphere( point p, sphere s )
{
	float r = fast_length( p.xyz - s.xyz );
	return ( r < s.w );
}

vector Bounce( vector in, vector n )
{
	n.w = 0.;
	n = fast_normalize( n ); // make it a unit vector
	// this is the vector equation for “angle of reflection equals angle of incidence”:
	vector out = in - n * (vector)( 2.*dot( in.xyz, n.xyz ) );
	// adding or subtracting 2 float4’s gives you another float4
	// multiplying 2 float4’s gives you another float4
	// when you want a dot product, use the dot( ) function
	out.w = 0.;
	return out;
}

vector BounceSphere( point p, vector in, sphere s )
{
	vector n;
	n.xyz = p.xyz - s.xyz;
	// the vector from the sphere center to the point is the normal
	return Bounce( in, n );
}

kernel void Particle( global point *dPobj, global vector *dVel, global color *dCobj )
{
	int gid = get_global_id( 0 ); // particle #
	point p = dPobj[gid];
	vector v = dVel[gid];
	point pp = p + v*DT + G * (point)(.5*DT*DT); // p’
	vector vp = v + G*DT; // v’
	pp.w = 1.;
	vp.w = 0.;
	if( IsInsideSphere( pp, Sphere1 ) )
	{
		vp = BounceSphere( p, v, Sphere1 );
		pp = p + vp*DT + G * (point)( .5*DT*DT );
	}
	dPobj[gid] = pp;
	dVel[gid] = vp;
}
