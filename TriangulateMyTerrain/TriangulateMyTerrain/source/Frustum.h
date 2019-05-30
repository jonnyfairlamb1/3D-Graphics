#pragma once
#include "Plane.h" 
#include <glm/glm.hpp>
#include <scene/context.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <vector>

class Frustum
{
public:
	Frustum();
	Frustum(const Frustum&);
	~Frustum();

	void ConstructFrustum(float screenDepth, glm::mat4 projection_xform,
		glm::mat4 view_xform);

	bool CheckCube(float xCenter, float yCenter, float zCenter, float radius);

	float PlaneDotCoord(Plane plane, glm::vec3 point);
	
	/*bool CheckPoint(float x, float y, float z);

	bool CheckCube(float xCenter, float yCenter, float zCenter, float radius);

	bool CheckRectangle(float xCenter, float yCenter, float zCenter,
						float xSize, float ySize, float zSize);
*/
private:
	std::vector<Plane> Planes;
};

