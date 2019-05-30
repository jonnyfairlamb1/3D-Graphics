#include "my_view.hpp"
#include <tygra/FileHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>

MyView::MyView()
{
}

MyView::~MyView() {
}

void MyView::setScene(const scene::Context * scene)
{
    scene_ = scene;
}

void MyView::toggleShading()
{
    shade_normals_ = !shade_normals_;
}

void MyView::windowViewWillStart(tygra::Window * window)
{
    assert(scene_ != nullptr);

    GLint compile_status = 0;
    GLint link_status = 0;

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertex_shader_string
        = tygra::createStringFromFile("resource:///terrain_vs.glsl");
    const char *vertex_shader_code = vertex_shader_string.c_str();
    glShaderSource(vertex_shader, 1,(const GLchar **)&vertex_shader_code, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length] = "";
        glGetShaderInfoLog(vertex_shader, string_length, NULL, log);
        std::cerr << log << std::endl;
    }

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragment_shader_string
        = tygra::createStringFromFile("resource:///terrain_fs.glsl");
    const char *fragment_shader_code = fragment_shader_string.c_str();
    glShaderSource(fragment_shader, 1,
                   (const GLchar **)&fragment_shader_code, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length] = "";
        glGetShaderInfoLog(fragment_shader, string_length, NULL, log);
        std::cerr << log << std::endl;
    }

    terrain_sp_ = glCreateProgram();
    glAttachShader(terrain_sp_, vertex_shader);
    glDeleteShader(vertex_shader);
    glAttachShader(terrain_sp_, fragment_shader);
    glDeleteShader(fragment_shader);
    glLinkProgram(terrain_sp_);


    glGetProgramiv(terrain_sp_, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length] = "";
        glGetProgramInfoLog(terrain_sp_, string_length, NULL, log);
        std::cerr << log << std::endl;
    }

	glGenVertexArrays(1, &cube_vao_);
	glBindVertexArray(cube_vao_);
	glGenBuffers(1, &cube_vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_);
	
	


    // X and -Z are on the ground, Y is up
    const float sizeX = scene_->getTerrainSizeX();
    const float sizeY = scene_->getTerrainSizeY();
    const float sizeZ = scene_->getTerrainSizeZ();


	tygra::Image height_image = tygra::createImageFromPngFile(scene_->getTerrainDisplacementMapName());

	int gridwidth = 256; // in points // must be even
	int gridheight = 256;
	int gridwidthless1 = gridwidth - 1;
	int gridheightless1 = gridheight - 1;

	//add verticies with x and -z positions
	for (int i = 0; i < gridwidth; i++)
	{
		for (int j = 0; j < gridheight; j++)
		{
			int x = j;// *4 * 8;
			int z = i;// *4 * 8;

			//std::cout << (uint8_t)((float*)height_image.pixel(i, j)) << std::endl;
			
			float y = *((uint8_t*)height_image.pixel(i, j));


			//for each pixel, get the value
			float total = 0.0f;
			float frequency = 0.09;
			float gain = 0.5;
			float amplitude = 120;
			float lacunarity = 2.0f;
			//how many times do we want to do it
			int octaves = 12;

			for (int q = 0; q < octaves; q++)
			{
				total += Pn.noise((float)i * frequency, (float)j * frequency, 0.8) * amplitude;
				frequency *= lacunarity;
				amplitude *= gain;
			}
			y += total;
			//y = 0;
			//add it to the grid object
			grid_.push_back(glm::vec3(i, y*0.1f, -j));
		}
	}

	//create a bool that makes us able to make the alternating tris
	bool even = true;

	//loop through the rows and collums
	for (int i = 0; i < gridwidthless1; i++)//rows
	{
		for (int j = 0; j < gridheightless1; j++)//Collumns
		{
			int offset = j + (i * gridwidth);

			int a = 0;
			int b = +1;
			int c = +gridwidth + 1;
			int d = +gridwidth;
			if (even)
			{
				//push back the 6 points
				//first triangle
				elements.push_back(offset + a);
				elements.push_back(offset + d);
				elements.push_back(offset + b);

				//second triangle
				elements.push_back(offset + d);
				elements.push_back(offset + c);
				elements.push_back(offset + b);
			}
			else
			{
				//push back the 6 points
				//first triangle
				elements.push_back(offset + a);
				elements.push_back(offset + c);
				elements.push_back(offset + b);
				//second triangle
				elements.push_back(offset + a);
				elements.push_back(offset + d);
				elements.push_back(offset + c);

			}
			//swap so that the dividing edge runs the opposite way
			even = !even;
		}
	}

	//calculate the normals
	gridNormals_.resize(grid_.size());
	for (unsigned int i = 0; i < elements.size(); i += 3)
	{
		gridNormals_[elements[i]] += CalculateSurfaceNormal(grid_[elements[i]],
			grid_[elements[i + 1]],
			grid_[elements[i + 2]]);
		gridNormals_[elements[i + 1]] += CalculateSurfaceNormal(grid_[elements[i]],
			grid_[elements[i + 1]],
			grid_[elements[i + 2]]);
		gridNormals_[elements[i + 2]] += (grid_[elements[i]],
			grid_[elements[i + 1]],
			grid_[elements[i + 2]]);

	}
	//normalise them
	for (unsigned int i = 0; i < gridNormals_.size(); i++)
	{
		gridNormals_[i] = glm::normalize(gridNormals_[i]);

	}


    // below is indicative code for initialising a terrain VAO.

	// below is for initialising a VAO
	glGenBuffers(1, &terrain_mesh_.element_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain_mesh_.element_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		elements.size() * sizeof(unsigned int),
		elements.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	terrain_mesh_.element_count = elements.size();

	glGenBuffers(1, &terrain_mesh_.normal_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, gridNormals_.size() * sizeof(glm::vec3),
		gridNormals_.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &terrain_mesh_.position_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.position_vbo);
	glBufferData(GL_ARRAY_BUFFER, grid_.size() * sizeof(glm::vec3),
		grid_.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &terrain_mesh_.vao);
	glBindVertexArray(terrain_mesh_.vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain_mesh_.element_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.position_vbo);
	glEnableVertexAttribArray(kVertexPosition);
	glVertexAttribPointer(kVertexPosition, 3, GL_FLOAT, GL_FALSE,
		sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//normal
	glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.normal_vbo);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MyView::windowViewDidReset(tygra::Window * window,
                                int width,
                                int height)
{
    glViewport(0, 0, width, height);
}

void MyView::windowViewDidStop(tygra::Window * window)
{
    glDeleteProgram(terrain_sp_);
    glDeleteBuffers(1, &terrain_mesh_.position_vbo);
    glDeleteBuffers(1, &terrain_mesh_.normal_vbo);
    glDeleteBuffers(1, &terrain_mesh_.element_vbo);
    glDeleteVertexArrays(1, &terrain_mesh_.vao);
}

void MyView::windowViewRender(tygra::Window * window)
{
    assert(scene_ != nullptr);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const float aspect_ratio = viewport[2] / (float)viewport[3];

    const auto& camera = scene_->getCamera();
    glm::mat4 projection_xform = glm::perspective(
        glm::radians(camera.getVerticalFieldOfViewInDegrees()),
        aspect_ratio,
        camera.getNearPlaneDistance(),
        camera.getFarPlaneDistance());
    glm::vec3 camera_pos = camera.getPosition();
    glm::vec3 camera_at = camera.getPosition() + camera.getDirection();
    glm::vec3 world_up{ 0, 1, 0 };
    glm::mat4 view_xform = glm::lookAt(camera_pos, camera_at, world_up);


    glClearColor(0.f, 0.f, 0.25f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, shade_normals_ ? GL_FILL : GL_LINE);

    glUseProgram(terrain_sp_);

    GLuint shading_id = glGetUniformLocation(terrain_sp_, "use_normal");
    glUniform1i(shading_id, shade_normals_);

    glm::mat4 world_xform = glm::mat4(1);
    glm::mat4 view_world_xform = view_xform * world_xform;

    GLuint projection_xform_id = glGetUniformLocation(terrain_sp_,
                                                      "projection_xform");
    glUniformMatrix4fv(projection_xform_id, 1, GL_FALSE,
                       glm::value_ptr(projection_xform));

    GLuint view_world_xform_id = glGetUniformLocation(terrain_sp_,
                                                      "view_world_xform");
    glUniformMatrix4fv(view_world_xform_id, 1, GL_FALSE,
                       glm::value_ptr(view_world_xform));

    if (terrain_mesh_.vao) {
        glBindVertexArray(terrain_mesh_.vao);
        glDrawElements(GL_TRIANGLES, terrain_mesh_.element_count,
                       GL_UNSIGNED_INT, 0);
    }
}

glm::vec3 MyView::CalculateSurfaceNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	glm::vec3 Normal;

	glm::vec3 VecU(b - a);
	glm::vec3 VecV(c - a);

	Normal.x = ((VecU.y * VecV.z) - (VecU.z * VecV.y));
	Normal.y = ((VecU.z * VecV.x) - (VecU.x * VecV.z));
	Normal.z = ((VecU.x * VecV.y) - (VecU.y * VecV.x));

	return Normal;
}
