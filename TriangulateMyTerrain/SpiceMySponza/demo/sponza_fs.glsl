#version 330

struct Light
{
	vec3 position;
	float range;
	vec3 colour;
	float intensity;
};


uniform sampler2D sampler_tex0;
uniform vec3 ambient_intensity;
uniform Light lights[5];
uniform int wireframe;

in vec4 normal;
in vec2 varying_texcoord;
in vec3 varying_position;


in vec3 P;
in vec3 N;

out vec4 fragment_colour;

void main(void)
{

	vec3 tex_colour = texture(sampler_tex0, varying_texcoord).rgb;
	vec3 result = vec3(0.85, 0.80, 0.66);

	vec3 overallLight = vec3(0);
	vec3 L = normalize(lights[0].position - P);

	float distance = distance(lights[0].position, P);

	float smoothFD = 1 - smoothstep(0, lights[0].range, distance);
	float pointIntensity = smoothFD * max(0, dot(L, N));

	vec3 diffuse_intensity = lights[0].colour * pointIntensity;

	overallLight += diffuse_intensity;


	result = vec3(0.85, 0.80, 0.66) * (overallLight + ambient_intensity);



	//if we want to see the wireframe
	if (wireframe == 1)
	{
		fragment_colour = vec4(1.0, 1.0, 1.0, 1.0) - ((vec4(1.0, 1.0, 1.0, 1.0) - vec4(result * tex_colour, 1.0)) * smoothstep(30, 40, distance));
	}
	else {
		//if we dont want to see the wireframe
		fragment_colour = vec4(tex_colour, 1.0);
	}
}