vec3_t(float) vec4_vec3(vec4_t(float) v)
{
	if(v.w == 0)
		return (vec3_t(float)) { v.x, v.y, v.z };
	else
	{
		float t = 1 / v.w;
		return (vec3_t(float))  { v.x * t, v.y * t, v.z * t };
	}
}


static void convert_to_vulkan_clipspace(vec2_t(float) *  const vertices, u32 count)
{
	for(u32 i = 0; i < count; i++)
		vertices[i].y = -vertices[i].y;
}

static vec2_t(float)* foreach_vertex3d(mesh3d_t* mesh, mat4_t(float) m)
{
	vec2_t(float)* vertices2d = heap_newv(vec2_t(float), mesh3d_positions_count(mesh));
	for(u32 i = 0; i < mesh3d_positions_count(mesh); i++)
	{
		vec3_t(float) position = mesh3d_position_get(mesh, i);
		vec3_t(float) v = vec4_vec3(mat4_mul_vec4(float)(m, position.x, position.y, position.z, 1));
		ref(vec2_t(float), vertices2d, i).x = v.z;
		ref(vec2_t(float), vertices2d, i).y = v.y;
	}
	return vertices2d;
}

static void vertex_position(vec3_t(float) position, void* mesh)
{
	mesh3d_position_add_vec3(mesh, position);
}

static void vertex_normal(vec3_t(float) normal, void* mesh)
{
	mesh3d_normal_add_vec3(mesh, normal);
}
