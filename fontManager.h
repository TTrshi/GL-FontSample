#pragma once
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H


#define FONT_ATLAS_TEXTURE_SIZE_WIDTH		1024
#define FONT_ATLAS_TEXTURE_SIZE_HEIGHT		1024
#define FONT_ATLAS_ONE_DATA_SIZE_WIDTH		64
#define FONT_ATLAS_ONE_DATA_SIZE_HEIGHT		64
#define FONT_ATLAS_LINE_LENGTH				(FONT_ATLAS_TEXTURE_SIZE_WIDTH / FONT_ATLAS_ONE_DATA_SIZE_WIDTH)
#define CREATE_FONT_SIZE	48
#define FONT_ATLAS_TEXTURE_UV_U(x)			((float)(x) / (float)FONT_ATLAS_TEXTURE_SIZE_WIDTH)
#define FONT_ATLAS_TEXTURE_UV_V(y)			((float)(y) / (float)FONT_ATLAS_TEXTURE_SIZE_HEIGHT)

#define NEW_LINE_CODE		'\n'
#define HALF_WIDTH_SPACE_CODE		' '

class Character {
public:
	GLuint     texture;
	glm::vec4 uvRect;
	glm::ivec2 size;
	glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
	GLuint     advance;    // Offset to advance to next glyph

	Character(glm::vec4 _uvRect, glm::ivec2 _size, glm::ivec2 _bearing, GLuint _advance) :
		texture(0)
	{
		uvRect = _uvRect;
		size = _size;
		bearing = _bearing;
		advance = _advance;
	}
	~Character() {
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &texture);
	}
};


class FontManager
{
private:
	FT_Library freetype_library;
	FT_Face freetype_face;
	std::string font_name;
	std::map<wchar_t, Character*> characters;

	struct {
		GLuint framebuffer;
		GLuint fontRenderedTexture;
		GLuint depthrenderbuffer;
	}buffer;

	struct {
		GLuint glProgramId;
		GLint positionLocation;
		GLint uvLocation;
		GLint textureLocation;
		GLint unif_matrix;
	}shader;

	static FontManager* instance;
public:
	FontManager();
	~FontManager();

	static FontManager* GetInstance() {
		if (instance == nullptr) {
			instance = new FontManager();
		}
		return instance;
	}

	void Initialize();
	void Finalize();

	void CreateFont(std::string _font_name);
	bool CreateCharacter(wchar_t _c);
	Character* GetCharacterData(wchar_t _c);
	GLuint GetFontTextureId() { return buffer.fontRenderedTexture; }
	void CrateShader();
	
	bool isSpecialCharacter(wchar_t _c);
	bool isHalfSpaceCharacter(wchar_t _c);
};