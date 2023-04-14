struct vec3_s {
	float x;
	float y;
	float z;
};

struct ParticlesInfo_s {
	float4		center;
	bool		hasGravity;
};

typedef struct ParticlesInfo_s ParticlesInfo;

typedef struct vec3_s vec3;

constant float G = 9.8;
constant float DT = 0.01;

//constant vec3 mov = { 0.001, 0, 0 };

float4 toFloat4(vec3 vector, float w) {
	float4 result;
	result.x = vector.x;
	result.y = vector.y;
	result.z = vector.z;
	result.w = w;
	return result;
}

vec3 toVec3(float4 vector) {
	vec3 result;
	result.x = vector.x;
	result.y = vector.y;
	result.z = vector.z;
	return result;
}

kernel void Particle( global vec3 *dPobj, global vec3 *dVel, constant ParticlesInfo *info)
{
	int index = get_global_id(0);

	float4 p = toFloat4(dPobj[index], 0);
	float4 v = toFloat4(dVel[index], 0);
	if (info->hasGravity) {
		v += DT * fast_normalize(info->center - p);
	}
	p += DT * v;
	dPobj[index] = toVec3(p);
	dVel[index] = toVec3(v);
}