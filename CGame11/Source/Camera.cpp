#include "Camera.h"


// create a camera with some basic details
Camera::Camera()
{
	// Projection matrix : 45° Field of View, 16:9 ratio, display range : near 0.1 unit <->  far 1600 units
	Ratio = 16.0f / 9.0f;
	FOV = 45;
	NearPlane = 0.1f;
	FarPlane = 14000.0f;
	Target = glm::vec3(0.0f, 0.0f, 0.0f);
	UpVector = glm::vec3(0, 1, 0);
		
	Projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 200.0f);
	//glOrtho(0, width, 0, height, -1, 1);
	
//	Projection = glm::ortho(0.f, 48.0f, 0.f, 32.0f, -1.f, 200.f);
	// give a devault camera position and offset
	CameraPos = glm::vec3(0.0f, 00.0f,25.0f);

	View  = glm::lookAt(
		CameraPos,
		 // Camera is at cameraPos, in World Space
		Target,
		 // look at the origin
		UpVector  // pointing up( 0,-1,0 will be upside-down));
		);
	

}

Camera::~Camera()
{
	
}


glm::mat4* Camera::GetView()
{
	return &View;
}



glm::mat4* Camera::GetProjection()
{
	
	return &Projection;
}


void	Camera::Update()
{
	// a simpler update just works out the new look at based on your position	
	// THIS SHOULD ALWAYS BE OVERRIDDEN
	// a simpler update just works out the new look at based on your position	
	CameraPos.x = Target.x;
	CameraPos.y	= Target.y + CameraOffset;
	// just for effect we will let the camera move up and down a bit, just moving the camera itself lets us tilt so we will move
	// the point we look at as well, but not by as much
	glm::vec3 Target2 = Target;
	Target2.y += CameraOffset / 2.0f;
	
	
			View  = glm::lookAt(
			CameraPos,
				 // Camera is at cameraPos, in World Space
			Target2, 		 // look at the target (usus
			glm::vec3(0, 1, 0)  // pointing up( 0,-1,0 will be upside-down
		);
	

}
	


