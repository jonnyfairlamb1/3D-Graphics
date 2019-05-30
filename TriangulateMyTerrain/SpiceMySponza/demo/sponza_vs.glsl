#version 330
uniform mat4 combined_xform;

in vec3 vertex_position;
in vec3 vertex_normal;
in vec2 vertex_texcoord;

out vec2 varying_texcoord;
out vec4 normal;
void main(void)
{
	varying_texcoord = vertex_texcoord;
	normal = vec4((mat3(combined_xform) * vertex_normal + 1) / 2, 0);
	gl_Position = combined_xform * vec4(vertex_position, 1.0);

}
