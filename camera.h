#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<glad/glad.h>
#include <GLFW/glfw3.h>

class Camera
{
private:
	float width;
	float height;
	float fov;
	float pitch;
	float yaw;

	//position of the camera
	glm::vec3 position;

	//direction where the camera is looking
	glm::vec3 direction;

	glm::vec3 up;

public:
	// the view matrix
	glm::mat4 view;

	// the projection matrix
	glm::mat4 projection;



	// Default constructor
	Camera();

	// init the camera
	void Init(float windowWidth, float windowHeight, glm::vec3 pos);

	//rotate camera using pitch and yaw
	void Rotate(float pitch, float yaw);

	// Move position of the camera with a vec3
	void Move(glm::vec3 pos);

	// set the position of the camera with 3 float
	void SetPosition(float x, float y, float z);

	// set the position of the camera with a vec3
	void SetPosition(glm::vec3 pos);

	//set the divection of the camera
	void SetDirection(glm::vec3 direction);

	const glm::vec3& GetPosition();

	const glm::vec3& GetDirection();

	// change the prespective matrix
	void ChangePerspective(float FOV, float windowWidth, float windowHeight, float near, float far);

	// update the camera matrice using its data
	void Update();

	float GetFOV();

	float GetScreenRatio();
};

#endif