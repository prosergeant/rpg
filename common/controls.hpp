#ifdef _WIN32
#include <glfw3.h>
#else
#include <GLFW/glfw3.h>
#endif
//extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

//#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 1, 0 ); 

vec3 getPos()
{
	return position;
}

// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;

// Initial vertical angle : none

//float verticalAngle = 0.7071f; //-0.140f;
float verticalAngle = 0.0f;

// Initial Field of View
float initialFoV = 45.0f;

float iy = 0.0f;
float speed = 30.0f; // 3 units / second
float mouseSpeed = 0.004f;

bool onGround = false;
bool sp = false;
bool qp = false;
bool wire = false;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if(yoffset > 0)
	{
		speed += 10.0f;
	}
	else
	{
		speed -= 10.0f;
		if(speed <= 0.0f) speed = 0.0f;
	}
}

void computeMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, w/2, h/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float( w/2 - xpos );
	verticalAngle   += mouseSpeed * float( h/2 - ypos );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f),
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);

	// Up vector
	glm::vec3 up = glm::cross( right, direction );
	
	
	
	/**

	*/
	position.y += iy * deltaTime;
	if(position.y > 0.0f ) { iy -= 1.11f * deltaTime; } 
	if(position.y <= 0.0f) { position.y = 0.0f; onGround = true; iy = 0.0f; }
	// Move forward
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		position.z += direction.z * deltaTime * speed;
		position.x += direction.x * deltaTime * speed;
		///position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		position.z -= direction.z * deltaTime * speed;
		position.x -= direction.x * deltaTime * speed;
		///position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}
	// Move Up
	if (glfwGetKey( window, GLFW_KEY_SPACE ) == GLFW_PRESS && onGround == true && sp == false/***/){
		sp = true;
		onGround = false;
		iy = 3.0f;
		//position.y += deltaTime * pow(speed, 8); // * speed * speed * speed * speed;
		//position += up * deltaTime * speed;
	}
	if (glfwGetKey( window, GLFW_KEY_SPACE ) == GLFW_RELEASE && onGround == true) sp = false;
	
	// Move Down
	if (glfwGetKey( window, GLFW_KEY_LEFT_CONTROL ) == GLFW_PRESS){
		position -= up * deltaTime * speed;
	}
	
	if (glfwGetKey( window, GLFW_KEY_Q ) == GLFW_PRESS && qp == false){
		qp = true;
		wire = !wire;
	}
	if (glfwGetKey( window, GLFW_KEY_Q ) == GLFW_RELEASE) qp = false;
	
	//if (glfwGetKey( window, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS)speed = 160.0f;
	
	//if (glfwGetKey( window, GLFW_KEY_LEFT_SHIFT ) == GLFW_RELEASE)speed = 30.0f;
	
	//glfwSetScrollCallback(window, scroll_callback);
	
	if (glfwGetKey( window, GLFW_KEY_GRAVE_ACCENT ) == GLFW_PRESS){
		if(c_on){
			st = CONSOLE;
			c_run = true;
		}
	}
	
	//position.y += iy; // * deltaTime;

	/*
	if (glfwGetKey( window, GLFW_KEY_E ) == GLFW_PRESS){
	}
	if (glfwGetKey( window, GLFW_KEY_E ) == GLFW_RELEASE) {
	}
	*/
	
	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 16.0f / 9.0f, 0.1f, 20000.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );
						   

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}
