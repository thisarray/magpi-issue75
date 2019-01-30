#pragma once
// Include GLM
#undef countof
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#undef countof

class Camera
{
public:
	Camera();
	~Camera();
	
	glm::mat4* GetView();
	glm::mat4* GetProjection();
	
	
	void Update();   // updates using its own position and matrix values
	

	glm::mat4 View;  // used by MVP calculations
	glm::mat4 Projection;
	glm::vec3 CameraPos;
	
	float CameraOffset;
	float Roll;  // when we move independant of an object
	float Pitch;
	float Yaw;
	
	
	float Ratio;
	float FOV;
	float NearPlane;
	float FarPlane;
	glm::vec3 Target;
	glm::vec3 UpVector;
	
	
};

