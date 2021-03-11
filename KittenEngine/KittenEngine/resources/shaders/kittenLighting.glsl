
layout (binding = 3, std140) uniform lightUBO {
	vec4 light_col;
	vec4 light_params;
	vec3 light_pos;
	int light_type;
};

vec3 getLightDir(vec3 worldPos) {
	if (light_type == 3)
		return light_pos;
	return normalize(worldPos - light_pos);
}

vec3 getLightCol(){
	return light_col.xyz * light_col.w;
}

vec3 getLightCol(vec3 worldPos){
	vec3 c = light_col.xyz * light_col.w;
	float distSqrInv = 1 / length2(worldPos - light_pos);

	switch(light_type) {
	case 0: // Ambient
		return c;
	case 1: // Point
		return c * distSqrInv;
	case 2: // Spot
		return c * distSqrInv * exp(pow2(light_params.w * dot(getLightDir(worldPos), light_params.xyz) - 1));
	case 3: // Dir
		return c;
	}
}