// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include "support_gl_Vector.h"

float g_width = 1024;
float g_height = 768;
#define RENDER_WIDTH 1280
#define RENDER_HEIGHT 720

GLuint loadTexture(std::string filename)
{
	// テクスチャIDの生成
	GLuint texID;
	glGenTextures(1, &texID);

	// ファイルの読み込み
	std::ifstream fstr(filename, std::ios::binary);
	const size_t fileSize = static_cast<size_t>(fstr.seekg(0, fstr.end).tellg());
	fstr.seekg(0, fstr.beg);
	char* textureBuffer = new char[fileSize];
	fstr.read(textureBuffer, fileSize);

	// テクスチャをGPUに転送
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, textureBuffer);

	// テクスチャの設定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// テクスチャのアンバインド
	delete[] textureBuffer;
	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}
struct {
	GLuint framebuffer;
	GLuint colorbuffer;
	GLuint depthrenderbuffer;
}buffer;
struct {
	GLuint glProgramId;
	GLint positionLocation;
	GLint uvLocation;
	GLint textureLocation;
	GLint unif_matrix;
	GLint unif_uvmatrix;
}shader;
void createUvShader() {
	//
	{
		/*
		glGenFramebuffers(1, &buffer.framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.framebuffer);

		glGenTextures(1, &buffer.colorbuffer);
		glBindTexture(GL_TEXTURE_2D, buffer.colorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, RENDER_WIDTH, RENDER_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glGenRenderbuffers(1, &buffer.depthrenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, buffer.depthrenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, RENDER_WIDTH, RENDER_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer.depthrenderbuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.colorbuffer, 0);
		//assert(glGetError() != GL_NO_ERROR);
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << error << std::endl;
		}


		//Clear Buffer
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.framebuffer);
		glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);
		glClearColor(0.0, 1.0, 0.0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearDepth(1.0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		*/

		//バーテックスシェーダのコンパイル
		GLuint vShaderId = glCreateShader(GL_VERTEX_SHADER);

		std::string vertexShader = R"#(
		attribute vec3 position;
		attribute vec4 uv;
		varying vec2 vuv;
		uniform mat4 unif_matrix;
		uniform mat4 unif_uvmatrix;
		void main(void){
			gl_Position = unif_matrix * vec4(position, 1.0);
			vuv = (unif_uvmatrix * uv).xy;
		}
		)#";

		const char* vs = vertexShader.c_str();
		glShaderSource(vShaderId, 1, &vs, NULL);
		glCompileShader(vShaderId);

		//フラグメントシェーダのコンパイル
		GLuint fShaderId = glCreateShader(GL_FRAGMENT_SHADER);

		std::string fragmentShader = R"#(
		varying vec2 vuv;
		uniform sampler2D texture;
		void main(void){
			gl_FragColor = texture2D(texture, vuv);
		}
		)#";
		const char* fs = fragmentShader.c_str();
		glShaderSource(fShaderId, 1, &fs, NULL);
		glCompileShader(fShaderId);

		//プログラムオブジェクトの作成
		GLuint programId = glCreateProgram();
		glAttachShader(programId, vShaderId);
		glAttachShader(programId, fShaderId);

		// リンク
		glLinkProgram(programId);
		shader.glProgramId = programId;
	}
	shader.positionLocation = glGetAttribLocation(shader.glProgramId, "position");
	shader.uvLocation = glGetAttribLocation(shader.glProgramId, "uv");
	shader.textureLocation = glGetUniformLocation(shader.glProgramId, "texture");
	shader.unif_matrix = glGetUniformLocation(shader.glProgramId, "unif_matrix");
	shader.unif_uvmatrix = glGetUniformLocation(shader.glProgramId, "unif_uvmatrix");
	assert(glGetError() == GL_NO_ERROR);
}

void matrix_sprite(
	float rect_x, float rect_y, float rect_width, float rect_height,
	float scale_x, float scale_y,
	float rotation_x, float rotation_y, float rotation_z,
	float surface_width, float surface_height,
	support::mat4& mat_result)
{
	float sprite_x = rect_x;
	float sprite_y = rect_y;
	float sprite_width = rect_width * scale_x;
	float sprite_height = rect_height * scale_y;

	float surface_aspect = surface_width / surface_height;
	support::mat4 mat_aspect = mat4_scale(1, surface_aspect, 1);

	float mat_scale_x = sprite_width / surface_width * 2.0f;
	float mat_scale_y = sprite_height / surface_width * 2.0f;    //Aspect ratio consideration
	support::mat4 mat_scale = mat4_scale(mat_scale_x, mat_scale_y, 0);

	float vertex_left = 0.5f + (1.0f - mat_scale_x) * 0.5f;
	float vertex_top = 0.5f + (1.0f - (mat_scale_y * surface_aspect)) * 0.5f;
	float translate_x = sprite_x / surface_width * 2.0f;
	float translate_y = sprite_y / surface_height * 2.0f;
	support::mat4 mat_translate = mat4_translate(-vertex_left + translate_x, vertex_top - translate_y, 0);

	support::mat4 mat_rotate_x = mat4_rotate(vec3_create(1, 0, 0), rotation_x);
	support::mat4 mat_rotate_y = mat4_rotate(vec3_create(0, 1, 0), rotation_y);
	support::mat4 mat_rotate_z = mat4_rotate(vec3_create(0, 0, 1), rotation_z);
	support::mat4 mat_rotate = mat4_identity();
	mat_rotate = mat4_multiply(mat_rotate, mat_rotate_x);
	mat_rotate = mat4_multiply(mat_rotate, mat_rotate_y);
	mat_rotate = mat4_multiply(mat_rotate, mat_rotate_z);

	mat_result = mat4_multiply(mat_translate, mat_aspect);
	mat_result = mat4_multiply(mat_result, mat_rotate);
	mat_result = mat4_multiply(mat_result, mat_scale);
}

void matrix_sprite_uv(
	float uv_rect_x, float uv_rect_y, float uv_rect_width, float uv_rect_height,
	float texture_width, float texture_height,
	support::mat4& mat_result)
{
	float scale_x = uv_rect_width / texture_width;
	float scale_y = uv_rect_height / texture_height;
	support::mat4 mat_scale = mat4_scale(scale_x, scale_y, 0);

	float translate_x = uv_rect_x / texture_width;
	float translate_y = uv_rect_y / texture_height;
	support::mat4 mat_translate = mat4_translate(translate_x, translate_y, 0);

	mat_result = mat4_multiply(mat_translate, mat_scale);
}

support::mat4 mat4_convert(glm::mat4 gl_mat) {
	support::mat4 result;

	int column = 0;
	int row = 0;

	for (column = 0; column < 4; ++column) {
		for (row = 0; row < 4; ++row) {
			if (column == row) {
				result.m[column][row] = gl_mat[column][row];
			}
			else {
				result.m[column][row] = gl_mat[column][row];
			}
		}
	}

	return result;
}

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);


	// Open a window and create its OpenGL context
	window = glfwCreateWindow(g_width, g_height, "Tutorial 0 - Keyboard and Mouse", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
    glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEBUG_OUTPUT);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// ============================================================
	GLuint texID = loadTexture("cat.raw");
	createUvShader();
	// ============================================================

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint Unif_matrixID = glGetUniformLocation(programID, "unif_matrix");

	// Get a handle for our buffers
	GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	GLuint vertexUVID = glGetAttribLocation(programID, "vertexUV");

	// Load the texture
	GLuint Texture = loadDDS("uvtemplate.DDS");
	
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	const float mag_vertex = 0.5f;
	static const GLfloat g_vertex_buffer_data[] = { 
		-1.0f * mag_vertex,-1.0f * mag_vertex,-1.0f * mag_vertex,
		-1.0f * mag_vertex,-1.0f * mag_vertex, 1.0f * mag_vertex,
		-1.0f * mag_vertex, 1.0f * mag_vertex, 1.0f * mag_vertex,
		 1.0f * mag_vertex, 1.0f * mag_vertex,-1.0f * mag_vertex,
		-1.0f * mag_vertex,-1.0f * mag_vertex,-1.0f * mag_vertex,
		-1.0f * mag_vertex, 1.0f * mag_vertex,-1.0f * mag_vertex,
		 1.0f * mag_vertex,-1.0f * mag_vertex, 1.0f * mag_vertex,
		-1.0f * mag_vertex,-1.0f * mag_vertex,-1.0f * mag_vertex,
		 1.0f * mag_vertex,-1.0f * mag_vertex,-1.0f * mag_vertex,
		 1.0f * mag_vertex, 1.0f * mag_vertex,-1.0f * mag_vertex,
		 1.0f * mag_vertex,-1.0f * mag_vertex,-1.0f * mag_vertex,
		-1.0f * mag_vertex,-1.0f * mag_vertex,-1.0f * mag_vertex,
		-1.0f * mag_vertex,-1.0f * mag_vertex,-1.0f * mag_vertex,
		-1.0f * mag_vertex, 1.0f * mag_vertex, 1.0f * mag_vertex,
		-1.0f * mag_vertex, 1.0f * mag_vertex,-1.0f * mag_vertex,
		 1.0f * mag_vertex,-1.0f * mag_vertex, 1.0f * mag_vertex,
		-1.0f * mag_vertex,-1.0f * mag_vertex, 1.0f * mag_vertex,
		-1.0f * mag_vertex,-1.0f * mag_vertex,-1.0f * mag_vertex,
		-1.0f * mag_vertex, 1.0f * mag_vertex, 1.0f * mag_vertex,
		-1.0f * mag_vertex,-1.0f * mag_vertex, 1.0f * mag_vertex,
		 1.0f * mag_vertex,-1.0f * mag_vertex, 1.0f * mag_vertex,
		 1.0f * mag_vertex, 1.0f * mag_vertex, 1.0f * mag_vertex,
		 1.0f * mag_vertex,-1.0f * mag_vertex,-1.0f * mag_vertex,
		 1.0f * mag_vertex, 1.0f * mag_vertex,-1.0f * mag_vertex,
		 1.0f * mag_vertex,-1.0f * mag_vertex,-1.0f * mag_vertex,
		 1.0f * mag_vertex, 1.0f * mag_vertex, 1.0f * mag_vertex,
		 1.0f * mag_vertex,-1.0f * mag_vertex, 1.0f * mag_vertex,
		 1.0f * mag_vertex, 1.0f * mag_vertex, 1.0f * mag_vertex,
		 1.0f * mag_vertex, 1.0f * mag_vertex,-1.0f * mag_vertex,
		-1.0f * mag_vertex, 1.0f * mag_vertex,-1.0f * mag_vertex,
		 1.0f * mag_vertex, 1.0f * mag_vertex, 1.0f * mag_vertex,
		-1.0f * mag_vertex, 1.0f * mag_vertex,-1.0f * mag_vertex,
		-1.0f * mag_vertex, 1.0f * mag_vertex, 1.0f * mag_vertex,
		 1.0f * mag_vertex, 1.0f * mag_vertex, 1.0f * mag_vertex,
		-1.0f * mag_vertex, 1.0f * mag_vertex, 1.0f * mag_vertex,
		 1.0f * mag_vertex,-1.0f * mag_vertex, 1.0f * mag_vertex
	};

	// Two UV coordinatesfor each vertex. They were created withe Blender.
	static const GLfloat g_uv_buffer_data[] = { 
		0.000059f, 0.000004f, 
		0.000103f, 0.336048f, 
		0.335973f, 0.335903f, 
		1.000023f, 0.000013f, 
		0.667979f, 0.335851f, 
		0.999958f, 0.336064f, 
		0.667979f, 0.335851f, 
		0.336024f, 0.671877f, 
		0.667969f, 0.671889f, 
		1.000023f, 0.000013f, 
		0.668104f, 0.000013f, 
		0.667979f, 0.335851f, 
		0.000059f, 0.000004f, 
		0.335973f, 0.335903f, 
		0.336098f, 0.000071f, 
		0.667979f, 0.335851f, 
		0.335973f, 0.335903f, 
		0.336024f, 0.671877f, 
		1.000004f, 0.671847f, 
		0.999958f, 0.336064f, 
		0.667979f, 0.335851f, 
		0.668104f, 0.000013f, 
		0.335973f, 0.335903f, 
		0.667979f, 0.335851f, 
		0.335973f, 0.335903f, 
		0.668104f, 0.000013f, 
		0.336098f, 0.000071f, 
		0.000103f, 0.336048f, 
		0.000004f, 0.671870f, 
		0.336024f, 0.671877f, 
		0.000103f, 0.336048f, 
		0.336024f, 0.671877f, 
		0.335973f, 0.335903f, 
		0.667969f, 0.671889f, 
		1.000004f, 0.671847f, 
		0.667979f, 0.335851f
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	do{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, g_width, g_height);
		// Clear the screen
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearDepth(1.0);
		
		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		support::mat4 matrix_box = mat4_identity();
		glUniformMatrix4fv(Unif_matrixID, 1, GL_FALSE, (GLfloat*)matrix_box.m);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		//glBindTexture(GL_TEXTURE_2D, texID);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			vertexPosition_modelspaceID,  // The attribute we want to configure
			3,                            // size
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(vertexUVID);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			vertexUVID,                   // The attribute we want to configure
			2,                            // size : U+V => 2
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
		);

		// Draw the triangles !
		glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(vertexPosition_modelspaceID);
		glDisableVertexAttribArray(vertexUVID);
		

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Label
		glBindTexture(GL_TEXTURE_2D, texID);
		glUseProgram(shader.glProgramId);
		glEnableVertexAttribArray(shader.positionLocation);
		glEnableVertexAttribArray(shader.uvLocation);
		float vertex_position[] = {
			0.5f, 0.5f,
			-0.5f, 0.5f,
			-0.5f, -0.5f,
			0.5f, -0.5f
		};

		const GLfloat vertex_uv[] = {
			1, 0,
			0, 0,
			0, 1,
			1, 1,
		};
		glVertexAttribPointer(shader.positionLocation, 2, GL_FLOAT, false, 0, vertex_position);
		glVertexAttribPointer(shader.uvLocation, 2, GL_FLOAT, false, 0, vertex_uv);
		glUniform1i(shader.textureLocation, 0);
		assert(glGetError() == GL_NO_ERROR);
		for (int i = 0; i < 10; i++) {
			const float canvas_width = 500;
			const float canvas_height = 700;
			float w = 100;
			float h = 100;
			float lineSize = canvas_width / w;
			float x = (w + 0) * (i % (int)lineSize) + 0;
			//float y = (h + 0) * (i / (int)lineSize) + (canvas_height- h) * 0.5f;
			float y = (h + 0) * (i / (int)lineSize);

			support::mat4 matrix;
			matrix_sprite(
				x, y, w, h,
				1.0f, 1.0f,
				0, 0, 0,
				canvas_width, canvas_height,
				matrix);

			support::mat4 uv_matrix;
			matrix_sprite_uv(
				0, 0, w, h,
				w, h,
				uv_matrix);

			const float canvasPerUnit = 1.0f;
			const float label_scale = canvasPerUnit * 0.5f;
			float canvas_aspect = canvas_height / canvas_width;
			const float pos_x = 0.0f;
			const float pos_y = 0.0f;
			const float pos_z = mag_vertex + 0.0001f;

			support::mat4 vr_matrix;
			glm::mat4 LabelModelMatrix = glm::mat4(1.0);
			LabelModelMatrix = glm::translate(LabelModelMatrix, glm::vec3(
				pos_x, 
				pos_y + (1.0 - canvas_aspect) * label_scale,
				pos_z
			));
			const float M_PI = 3.1415926536;
			//LabelModelMatrix = glm::rotate(LabelModelMatrix, M_PI / 2.0f, glm::vec3(0, 1, 0));
			LabelModelMatrix = glm::scale(LabelModelMatrix, glm::vec3(label_scale, label_scale * canvas_aspect, label_scale));

			glm::mat4 test_MVP = ProjectionMatrix * ViewMatrix * LabelModelMatrix;
			vr_matrix = mat4_convert(test_MVP);

			matrix = mat4_multiply(vr_matrix, matrix);
			glUniformMatrix4fv(shader.unif_matrix, 1, GL_FALSE, (GLfloat*)matrix.m);
			glUniformMatrix4fv(shader.unif_uvmatrix, 1, GL_FALSE, (GLfloat*)uv_matrix.m);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}
		glDisableVertexAttribArray(shader.positionLocation);
		glDisableVertexAttribArray(shader.uvLocation);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &TextureID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

const char* message_00[] = {
	"test",
	"test",
	"test",
};
const char* message_01[] = {
	"test",
	"test",
	"test",
};
const char** datTextTbl[] = {
	message_00,
	message_01,
};