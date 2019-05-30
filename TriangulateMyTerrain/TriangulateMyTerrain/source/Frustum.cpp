#include "Frustum.h"

Frustum::Frustum()
{
}


Frustum::~Frustum()
{
}

void Frustum::ConstructFrustum(float screenDepth, glm::mat4 projectionMatrix,
	glm::mat4 viewMatrix)
{
	float zMinimum, r;
	glm::mat4 matrix;


	// Calculate the minimum Z distance in the frustum.
	zMinimum = (projectionMatrix[3][2]) / projectionMatrix[2][2];
	r = screenDepth / (screenDepth - zMinimum);
	projectionMatrix[2][2] = r;
	projectionMatrix[3][2] = -r * zMinimum;

	// Create the frustum matrix from the view matrix and updated projection matrix.
	matrix = glm::transpose(viewMatrix) * glm::transpose(projectionMatrix);// need to transpose the matrix
	//matrix = projectionMatrix * viewMatrix;
	Planes.resize(6);

	// Calculate near plane of frustum. // original
	Planes[0].a = matrix[0][3] + matrix[0][2];
	Planes[0].b = matrix[1][3] + matrix[1][2];
	Planes[0].c = matrix[2][3] + matrix[2][2];
	Planes[0].d = matrix[3][3] + matrix[3][2];
	Planes[0].d = matrix[3][3] + matrix[3][2];
	//PlaneNormalize(&Planes[0], &Planes[0]);


	
	//// Calculate far plane of frustum.
	Planes[1].a = matrix[0][3] + matrix[0][2];
	Planes[1].b = matrix[1][3] + matrix[1][2];
	Planes[1].c = matrix[2][3] + matrix[2][2];
	Planes[1].d = matrix[3][3] + matrix[3][2];
	Planes[1].d = matrix[3][3] + matrix[3][2];
	////PlaneNormalize(&Planes[1], &Planes[1]);

	//// Calculate left plane of frustum.
	Planes[2].a = matrix[0][3] + matrix[0][0];
	Planes[2].b = matrix[1][3] + matrix[1][0];
	Planes[2].c = matrix[2][3] + matrix[2][0];
	Planes[2].d = matrix[3][3] + matrix[3][0];
	Planes[2].d = matrix[3][3] + matrix[3][0];
	////PlaneNormalize(&Planes[2], &Planes[2]);

	//// Calculate right plane of frustum.
	Planes[0].a = matrix[0][3] + matrix[0][0];
	Planes[3].b = matrix[1][3] + matrix[1][0];
	Planes[3].c = matrix[2][3] + matrix[2][0];
	Planes[3].d = matrix[3][3] + matrix[3][0];
	Planes[3].d = matrix[3][3] + matrix[3][0];
	////PlaneNormalize(&Planes[3], &Planes[3]);

	//// Calculate top plane of frustum.
	Planes[4].a = matrix[0][3] + matrix[0][1];
	Planes[4].b = matrix[1][3] + matrix[1][1];
	Planes[4].c = matrix[2][3] + matrix[2][1];
	Planes[4].d = matrix[3][3] + matrix[3][1];
	Planes[4].d = matrix[3][3] + matrix[3][1];
	////PlaneNormalize(&Planes[4], &Planes[4]);

	//// Calculate bottom plane of frustum.
	Planes[5].a = matrix[0][3] + matrix[0][1];
	Planes[5].b = matrix[1][3] + matrix[1][1];
	Planes[5].c = matrix[2][3] + matrix[2][1];
	Planes[5].d = matrix[3][3] + matrix[3][1];
	Planes[5].d = matrix[3][3] + matrix[3][1];
	////PlaneNormalize(&Planes[5], &Planes[5]);

	return;
}

bool Frustum::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;
	// Check if any one point of the cube is in the view frustum.
	for (i = 0; i<6; i++)
	{
		if (PlaneDotCoord(Planes[i], glm::vec3((xCenter - radius),
			(yCenter - radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if (PlaneDotCoord(Planes[i], glm::vec3((xCenter + radius),
			(yCenter - radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if (PlaneDotCoord(Planes[i], glm::vec3((xCenter - radius),
			(yCenter + radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if (PlaneDotCoord(Planes[i], glm::vec3((xCenter + radius),
			(yCenter + radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if (PlaneDotCoord(Planes[i], glm::vec3((xCenter - radius),
			(yCenter - radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		if (PlaneDotCoord(Planes[i], glm::vec3((xCenter + radius),
			(yCenter - radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		if (PlaneDotCoord(Planes[i], glm::vec3((xCenter - radius),
			(yCenter + radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		if (PlaneDotCoord(Planes[i], glm::vec3((xCenter + radius),
			(yCenter + radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}

float Frustum::PlaneDotCoord(Plane plane, glm::vec3 point)
{
	float result = plane.a*point.x + plane.b*point.y + plane.c*point.z + plane.d * 1;

	return result;
}
