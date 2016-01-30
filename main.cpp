// v 0.1.0
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <stdexcept>
#include <exception>
#include <iostream>

//#define DEBUG
//#define DEBUG_FRAMES

#define FOR(a) for(int i = 0; i < a; i++)
#define FOR_j(a) for(int j = 0; j < a; j++)
#define FOR_d(j, a) for(j = 0; j < a; j++)
#define FOR_s(q, w) for(int i = q; i < w; i++)
#define PI 3.141592652

// Include GLEW
#include <GL/glew.h>

#ifdef _WIN32
#include <glfw3.h>
#else
#include <GLFW/glfw3.h>
#endif
GLFWwindow* window;


// Include GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>


int w = 0, h = 0, chooseTex = 0, posX = 0, posY = 0, posZ = 0, shader_v = 0; 
int tgm = 0, tcl = 0;
int sphere_detail = 10;
GLuint shader, billShader;
GLuint CameraRight_worldspace_ID, 
		CameraUp_worldspace_ID,
		ViewProjMatrixID,
		BillboardPosID,
		BillboardSizeID,
		billboard_vertex_buffer;
GLfloat g_vertex_buffer_data[] = { 
		 -0.5f, -0.5f, 0.0f,
		  0.5f, -0.5f, 0.0f,
		 -0.5f,  0.5f, 0.0f,
		  0.5f,  0.5f, 0.0f
	};
float vertAngle = 0, horAngle = 0;
float p_radius = 5.0f;
std::string model_path = "", title = "Default", font;

std::vector<std::vector<glm::vec3> > vertices;
std::vector<std::vector<glm::vec2> > uvs;
std::vector<std::vector<glm::vec3> > normals;
std::vector<std::vector<unsigned short> > indices;
std::vector<std::string> model_p;
std::vector<std::string> texture_p;
std::vector<GLuint> Textures;

bool c_run = false;
bool c_on = true;

enum states {GAME, CONSOLE } st = GAME;

#include <iniparser/src/iniparser.h>
#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/vboindexer.hpp>
#include <common/objloader.hpp>
#include <common/text2D.hpp>
#include "commands.hpp"
#include <common/obj.hpp>

int parse_ini_file(char* ini_name);
void init();
void billboard_draw(GLuint shader, float x, float y, float z);

int map[5][5] = {{0,0,0,0,0},
				 {0,1,1,1,0},
				 {0,1,1,1,0},
				 {0,1,1,1,0},
				 {0,0,0,0,0}};

int main()
{
	srand(time (0));
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}
	
	int status;
	
	status = parse_ini_file("Game.ini");
	if(status != 0)
	{
		printf("Error parse Game.ini file\n");
		system("pause");
		return 0;
	}
	if(shader_v == 3){
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}else{
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	}
	window = glfwCreateWindow( w, h, title.c_str(), NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		#ifdef DEBUG
		printf("Trying to restart with another arguments - width = 1600 | height = 900\nw = %i\nh = %i\n", w, h);
		#endif
		system("pause");
		#ifdef DEBUG
		window = glfwCreateWindow( 1600, 900, title.c_str(), NULL, NULL);
		if(window == NULL)
		{
			fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
			system("pause");
			glfwTerminate();
			return -1;
		}
		#endif
		#ifndef DEBUG
		glfwTerminate();
		return -1;
		#endif
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, w/2, h/2);
	glfwSetScrollCallback(window, scroll_callback);

	

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS); 

	glEnable(GL_CULL_FACE);
	
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	billShader = LoadShaders( "Billboard.vertexshader", "Billboard.fragmentshader" );

	CameraRight_worldspace_ID  = glGetUniformLocation(billShader, "CameraRight_worldspace");
	CameraUp_worldspace_ID  = glGetUniformLocation(billShader, "CameraUp_worldspace");
	ViewProjMatrixID = glGetUniformLocation(billShader, "VP");
	BillboardPosID = glGetUniformLocation(billShader, "BillboardPos");
	BillboardSizeID = glGetUniformLocation(billShader, "BillboardSize");
	
	
	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);
	
	init();	
	
	std::vector<Obj*> objs;
	std::vector<Obj*>::iterator oIT;
	
	/**
	objs.push_back(	new Obj(0.0f, 0.0f, 0.0f,
			vertices[0],
			uvs[0], 	
			normals[0], 
			indices[0],
			Textures[0]));
	
	objs[0]->CreateLand(3000); //CreateAABB();
	*/
	
	objs.push_back( new Obj(500.0f, -15.0f, 15.0f,
							vertices[1],
							uvs[1],
							normals[1],
							indices[1],
							Textures[0]));
	objs[objs.size()-1]->size = glm::vec3(0.05f, 0.05f, 0.05f);
	objs[objs.size()-1]->type = 1;
		
	cube cub, cub2;
	cub = CreateSphere(100.0f, sphere_detail);
	cub2 = CreateSphere(43.3f, 2);
	
	objs.push_back(new Obj(300.0f, 0.0f, 300.0f,
							cub.st_vert,
							cub.st_uv,
							cub.st_norm,
							cub.st_indices,
							Textures[0]));
	
	for(int i = 0; i < 5; i++)
	{
		for(int j = 0; j < 5; j++)
		{
			if(map[i][j] == 0)
			{
				objs.push_back(new Obj(50.0f*j, 0.0f, 50.0f*i,
							cub2.st_vert,
							cub2.st_uv,
							cub2.st_norm,
							cub2.st_indices,
							Textures[0]));
			}
			
			if(map[i][j] == 1)
			{
				objs.push_back(new Obj(50.0f*j, -50.0f, 50.0f*i,
							cub2.st_vert,
							cub2.st_uv,
							cub2.st_norm,
							cub2.st_indices,
							Textures[0]));
			}
		}
	}
								
	position = glm::vec3(posX, posY, posZ);
	verticalAngle = vertAngle;
	horizontalAngle = horAngle;
	
	initText2D( font.c_str() );

	double lastTime = glfwGetTime();
	int nbFrames = 0;
	char text[256];

	do{
		
		#ifdef DEBUG
		#ifdef DUBUG_FRAMES
		double currentTime = glfwGetTime();
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
			printf("%f ms/frame\n", 1000.0/double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}
		#endif
		#endif

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if(st == GAME){
		computeMatricesFromInputs();
		}

		if(st == CONSOLE){
			console_update();
		}

		if(coord == true)
		{
			coord = false;
			try
			{
				objs.push_back(	new Obj(oX, oY, oZ,
				vertices[nModel],
				uvs[nModel],
				normals[nModel],
				indices[nModel],
				Textures[texID])); //texName[chooseTex]);
				objs[objs.size() - 1]->size = glm::vec3(sX,sY,sZ);
			}
			catch(const std::out_of_range& e)
			{
				std::cout << e.what() << std::endl;
			}
		}

		if(send == true)
		{
			objs.push_back(	new Obj(position.x, position.y, position.z,
			vertices[0],
			uvs[0],
			normals[0],
			indices[0],
			Textures[0]));
			send = false;
		}

		if(rm == true)
		{
			try
			{
				if(objs.empty())
					throw std::range_error("objs is empty");
				
				if(ix >= objs.size())
					throw std::range_error("objs out of range");
				
				objs.erase(objs.begin()+ix);
				printf("%i index removed from %i\n", ix, objs.size());
			}
			catch(const std::out_of_range& e)
			{
				std::cout << "Trying to remove " << ix << " from " << objs.size() << std::endl;
				std::cout << "Objs vector - out of range(" << e.what() << ")" << std::endl;
			}
			
			catch(const std::range_error& e)
			{
				std::cout << "Trying to remove " << ix << " from " << objs.size() << std::endl;
				std::cout << "Objs vector - range_error(" << e.what() << ")" << std::endl;
			}
			
			rm = false;
		}
		
		tcl = 0;
		
		if(test)
			objs[0]->dd.z = -(speed/2 * g_deltaTime);
		
		if(!test)
			objs[0]->dd.z = 0.0f;
		
		for(oIT = objs.begin(); oIT != objs.end(); oIT++)
		{
			Obj *a = *oIT;
			
			a->pos += a->dd;
			
			testColl2(a); //checkCollObjCam(a);
			
			glm::vec3 dist_v3 = a->pos - position;
			float dist_f = glm::length(dist_v3);
			
			a->ExtractFrustum();
			a->CreateBSphere();
			
			if(a->type == 1)
			{		
				glm::vec3 tempUp = glm::cross(g_direction, g_right);
				a->pos = position + (g_direction*8.0f + g_right*4.0f) + (tempUp*2.0f);
				a->rot *= position; //g_direction;
			}
			
			if(a->SphereInFrustum(a->pos.x, a->pos.y, a->pos.z, a->bsphere_radius) && dist_f <= 1000)
			{
				tcl++;
				a->DrawAt(a->pos.x, a->pos.y, a->pos.z);
				
				//glEnable(GL_BLEND);
				//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
				//FOR(8)
				{
				//	billboard_draw(billShader,a->aabb[i].x,a->aabb[i].y,a->aabb[i].z);
				}
				//glDisable(GL_BLEND);
			}
			/*else
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
				billboard_draw(billShader, a->pos.x, a->pos.y, a->pos.z);
				glDisable(GL_BLEND);
			}*/
		}
		
		//char text[256];
		sprintf(text,"%.2f ", verticalAngle );
		printText2D(text, 10, 560, 20);

		sprintf(text,"%.2f ", horizontalAngle );
		printText2D(text, 10, 520, 20);
		
		//glm::vec3 dis = objs[0]->pos - position;
		sprintf(text,"tgm %i ", tgm );
		printText2D(text, 10, 480, 20);

		sprintf(text,"tcl %i ", tcl );
		printText2D(text, 10, 440, 20);
		
		sprintf(text,"pX %.2f ", position.x);
		printText2D(text, 150, 560, 20);
		
		sprintf(text,"pY %.2f ", position.y);
		printText2D(text, 150, 520, 20);
		
		sprintf(text,"pZ %.2f ", position.z);
		printText2D(text, 150, 480, 20);
		
		sprintf(text,"speed %.2f ", speed);
		printText2D(text, 150, 440, 20);
		
		#ifndef DEBUG
		double currentTime = glfwGetTime();
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
			// printf and reset
			// sprintf(text, "%.2f ", 1000.0/double(nbFrames));
			// printText2D(text, 10, 480, 20);
			nbFrames = 0;
			lastTime += 1.0;
		}

			sprintf(text, "%.0f ", 1000.0/double(nbFrames));
			printText2D(text, 10, 480, 20);
		#endif	
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	} 
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );
	for(oIT = objs.begin(); oIT != objs.end(); oIT++)
		{
			Obj *a = *oIT;
			delete a;
		}
	glDeleteVertexArrays(1, &VertexArrayID);
		glDeleteProgram(billShader);
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return 0;
}


int parse_ini_file(char * ini_name)
{
    dictionary  *   ini ;

    ini = iniparser_load(ini_name);
    if (ini==NULL) {
        fprintf(stderr, "cannot parse file: %s\n", ini_name);
        return -1 ;
    }
    iniparser_dump(ini, stderr);
	
	char* path;
	
	p_radius = iniparser_getdouble(ini, "Level:p_rad", -1.0);
	#ifdef DEBUG
	printf("p_radius = %f\n", p_radius);
	#endif
	
	vertAngle = iniparser_getdouble(ini, "Player:vertAngle", -1.0);
	#ifndef _WIN32
	vertAngle -= 3.07f;
	#endif
	#ifdef DEBUG
	printf("vertAngle = %f\n", vertAngle);
	#endif
	
	horAngle = iniparser_getdouble(ini, "Player:horAngle", -1.0);
	#ifndef _WIN32
	horAngle -= 4.10f;
	#endif
	#ifdef DEBUG
	printf("horAngle = %f\n", horAngle);
	#endif
	
	path = const_cast<char*>(iniparser_getstring(ini, "Level:model", NULL));
	model_path = path;
	model_p.push_back(model_path);
	#ifdef DEBUG
	printf("path = %s\nmodel_path = %s\n", path, model_path.c_str());
	#endif
	
	path = const_cast<char*>(iniparser_getstring(ini, "Level:model2", NULL));
	model_path = path;
	model_p.push_back(model_path);
	#ifdef DEBUG
	printf("path = %s\nmodel_path = %s\n", path, model_path.c_str());
	#endif
	
	path = const_cast<char*>(iniparser_getstring(ini, "Global:title", NULL));
	title = path;
	#ifdef DEBUG
	printf("path = %s\ntitle = %s\n", path, title.c_str());
	#endif
	
	w = iniparser_getint(ini, "Global:intW", -1);
	h = iniparser_getint(ini, "Global:intH", -1);
	#ifdef DEBUG
	printf("w = %i\nh = %i\n", w, h);
	#endif
	
	path = const_cast<char*>(iniparser_getstring(ini, "Level:t1", NULL));
	model_path = path;
	texture_p.push_back(model_path);
	#ifdef DEBUG
	printf("path = %s\nmodel_path = %s\n", path, model_path.c_str());
	#endif
	
	path = const_cast<char*>(iniparser_getstring(ini, "Level:t2", NULL));
	model_path = path;
	texture_p.push_back(model_path);
	#ifdef DEBUG
	printf("path = %s\nmodel_path = %s\n", path, model_path.c_str());
	#endif
	
	path = const_cast<char*>(iniparser_getstring(ini, "Level:t3", NULL));
	model_path = path;
	texture_p.push_back(model_path);
	#ifdef DEBUG
	printf("texBuff = %s\ntexName = %s\n", texBuff[2], texName[2].c_str());
	#endif
	
	path = const_cast<char*>(iniparser_getstring(ini, "Global:font", NULL));
	font = path;
	#ifdef DEBUG
	printf("font = %s\n", font.c_str());
	#endif
	
	chooseTex = iniparser_getint(ini, "Global:nTexture", -1);
	#ifdef DEBUG
	printf("chooseTex = %i\n", chooseTex);
	#endif

	posX = iniparser_getint(ini, "Player:pX", -1);
	#ifdef DEBUG
	printf("Player.x = %i\n", posX);
	#endif
	
	posY = iniparser_getint(ini, "Player:pY", -1);
	#ifdef DEBUG
	printf("Player.y = %i\n", posY);
	#endif

	posZ = iniparser_getint(ini, "Player:pZ", -1);
	#ifdef DEBUG
	printf("Player.z = %i\n", posZ);
	#endif

	c_on = iniparser_getboolean(ini, "Global:console", -1);
	#ifdef DEBUG
	printf("Console = %d\n", c_on);
	#endif
	
	shader_v = iniparser_getint(ini, "Global:shader", -1);
	#ifdef DEBUG
	printf("Shader version = %i\n", shader_v);
	#endif
	
	sphere_detail = iniparser_getint(ini, "Level:detail", -1);
	#ifdef DEBUG
	printf("detail = %i\n", sphere_detail);
	#endif
	
    iniparser_freedict(ini);
    return 0 ;
}


void init()
{
	std::vector<unsigned short> indicess;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
		
	try
	{	
		FOR(model_p.size()){
			
		indicess.clear();
		indexed_vertices.clear();
		indexed_uvs.clear();
		indexed_normals.clear();
		
		loadOBJ(model_p[i].c_str(), indexed_vertices, indexed_uvs, indexed_normals, indicess);
		
		vertices.push_back(indexed_vertices);
		uvs.push_back(indexed_uvs);
		normals.push_back(indexed_normals);
		indices.push_back(indicess);
		}
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	
	shader = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );
	
	FOR(texture_p.size())
	{
		Textures.push_back(loadDDS(texture_p[i].c_str()));
	}
	
	glUseProgram(shader);
	GLuint LightID = glGetUniformLocation(shader, "LightPosition_worldspace");
	
	glm::vec3 lightPos = glm::vec3(4,7,4);
	glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
}


void billboard_draw(GLuint shader, float x, float y, float z)
{
	glUseProgram(shader);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Textures[2]);
			GLuint TextureID  = glGetUniformLocation(shader, "MyTextureSampler");
	glUniform1i(TextureID, 0);
			
	glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			
	glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
	glUniform3f(CameraUp_worldspace_ID   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
			
	glUniform3f(BillboardPosID, x, y, z); // The billboard will be just above the cube
	glUniform2f(BillboardSizeID, 1.0f, /*0.125f*/ 1.0f);     // and 1m*12cm, because it matches its 256*32 resolution
		
	glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glDisableVertexAttribArray(0);
}