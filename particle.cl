struct vec3_s {
	float x;
	float y;
	float z;
};

typedef struct vec3_s vec3;

kernel void Particle( global vec3 *dPobj, global vec3 *dVel)
{
	int i = get_global_id(0);

	dPobj[i].x += 0.001f;
}