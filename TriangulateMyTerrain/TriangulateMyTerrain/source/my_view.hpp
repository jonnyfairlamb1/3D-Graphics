#pragma once

#include <scene/context.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>
#include "PerlinNoise.h"
#include "Frustum.h"

class MyView : public tygra::WindowViewDelegate
{
public:

    MyView();

    ~MyView();

    void setScene(const scene::Context * scene);

    void toggleShading();

private:

    void windowViewWillStart(tygra::Window * window) override;

    void windowViewDidReset(tygra::Window * window,
                            int width,
                            int height) override;

    void windowViewDidStop(tygra::Window * window) override;

    void windowViewRender(tygra::Window * window) override;

	glm::vec3 CalculateSurfaceNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c);

private:

    const scene::Context * scene_{ nullptr };

    GLuint terrain_sp_{ 0 };
	GLuint shapes_sp_{ 0 };

    bool shade_normals_{ false };

    struct MeshGL
    {
        GLuint position_vbo{ 0 };
        GLuint normal_vbo{ 0 };
        GLuint element_vbo{ 0 };
        GLuint vao{ 0 };
        int element_count{ 0 };
    };
    MeshGL terrain_mesh_;

    enum
    {
        kVertexPosition = 0,
        kVertexNormal = 1,
    };

	GLuint cube_vao_{ 0 };
	GLuint cube_vbo_{ 0 };


	std::vector <glm::vec3>grid_;
	std::vector <glm::vec3>gridNormals_;


	std::vector <GLuint> elements;

	//createa  perlin noise object
	PerlinNoise Pn;

};
