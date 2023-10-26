#include "fontManager.h"
#include "support_gl_Vector.h"


FontManager* FontManager::instance;

#define FONT_ATLAS_ONE_DATA_UV_SIZE_WIDTH(w)	(((float)(w) / (float)FONT_ATLAS_TEXTURE_SIZE_WIDTH) * 2.0f)
#define FONT_ATLAS_ONE_DATA_UV_SIZE_HEIGHT(h)	(((float)(h) / (float)FONT_ATLAS_TEXTURE_SIZE_HEIGHT) * 2.0f)

#define ATTRIBUTE_NAME_POSITION				"position"
#define ATTRIBUTE_NAME_UV					"uv"
#define UNIFORM_NAME_TEXTURE				"texture"

FontManager::FontManager() 
{

}

FontManager::~FontManager() 
{
	

}

void FontManager::Initialize() {

	glGenFramebuffers(1, &buffer.framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, buffer.framebuffer);

	glGenTextures(1, &buffer.fontRenderedTexture);
	glBindTexture(GL_TEXTURE_2D, buffer.fontRenderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FONT_ATLAS_TEXTURE_SIZE_WIDTH, FONT_ATLAS_TEXTURE_SIZE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenRenderbuffers(1, &buffer.depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, buffer.depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, FONT_ATLAS_TEXTURE_SIZE_WIDTH, FONT_ATLAS_TEXTURE_SIZE_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer.depthrenderbuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.fontRenderedTexture, 0);
	//assert(glGetError() != GL_NO_ERROR);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << error << std::endl;
	}

	
	//Clear Buffer
	glBindFramebuffer(GL_FRAMEBUFFER, buffer.framebuffer);
	glViewport(0, 0,FONT_ATLAS_TEXTURE_SIZE_WIDTH, FONT_ATLAS_TEXTURE_SIZE_HEIGHT);
	glClearColor(0.5, 0.5, 0.5, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth(1.0);
	
	//
	CrateShader();
	shader.positionLocation = glGetAttribLocation(shader.glProgramId, ATTRIBUTE_NAME_POSITION);
	shader.uvLocation = glGetAttribLocation(shader.glProgramId, ATTRIBUTE_NAME_UV);
	shader.textureLocation = glGetUniformLocation(shader.glProgramId, UNIFORM_NAME_TEXTURE);
	shader.unif_matrix = glGetUniformLocation(shader.glProgramId, "unif_matrix");
	assert(glGetError() == GL_NO_ERROR);



	//FreeType Initialize
	FT_Init_FreeType(&freetype_library);
}

void FontManager::Finalize() {
	auto begin = characters.begin(), end = characters.end();
	for (auto iter = begin; iter != end; iter++) {
		delete iter->second;
	}
	characters.clear();

	FT_Done_Face(freetype_face);
	FT_Done_FreeType(freetype_library);

	glDeleteFramebuffers(1, &buffer.framebuffer);
	glDeleteTextures(1, &buffer.fontRenderedTexture);
	glDeleteRenderbuffers(1, &buffer.depthrenderbuffer);
}

void FontManager::CreateFont(std::string _font_name) {
	FT_New_Face(freetype_library, _font_name.c_str(), 0, &freetype_face);
	FT_Set_Pixel_Sizes(freetype_face, 0, CREATE_FONT_SIZE);
}

bool FontManager::CreateCharacter(wchar_t _c) {
	if (isSpecialCharacter(_c)) {
		return true;
	}
	auto found = characters.find(_c);
	if (found != end(characters)) {
		return true;
	}
	int wroteNum = characters.size();

	if (FT_Load_Glyph(freetype_face, FT_Get_Char_Index(freetype_face, _c), FT_LOAD_RENDER)) {
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
		return false;
	}

	GLfloat x = ((wroteNum % FONT_ATLAS_LINE_LENGTH) * FONT_ATLAS_ONE_DATA_SIZE_WIDTH);
	GLfloat y = (wroteNum / FONT_ATLAS_ONE_DATA_SIZE_HEIGHT) * FONT_ATLAS_ONE_DATA_SIZE_HEIGHT;
	glm::vec4 uvRect = glm::vec4(
		FONT_ATLAS_TEXTURE_UV_U(x),
		FONT_ATLAS_TEXTURE_UV_V(y),
		FONT_ATLAS_TEXTURE_UV_U(x) + FONT_ATLAS_TEXTURE_UV_U(freetype_face->glyph->bitmap.width),
		FONT_ATLAS_TEXTURE_UV_V(y) + FONT_ATLAS_TEXTURE_UV_V(freetype_face->glyph->bitmap.rows));

	Character* character = new Character(
		uvRect,
		glm::ivec2(freetype_face->glyph->bitmap.width, freetype_face->glyph->bitmap.rows),
		glm::ivec2(freetype_face->glyph->bitmap_left, freetype_face->glyph->bitmap_top),
		freetype_face->glyph->advance.x
	);

	auto result = characters.insert(std::make_pair(_c, character));
	if (!result.second) {
		result.first->second = character;
	}

	if (freetype_face->glyph->bitmap.width != 0 && freetype_face->glyph->bitmap.rows != 0) {
		GLuint texture;
		glCreateTextures(GL_TEXTURE_2D, 1, &texture);
		glTextureStorage2D(texture, 1, GL_R8, freetype_face->glyph->bitmap.width, freetype_face->glyph->bitmap.rows);
		glTextureSubImage2D(texture, 0, 0, 0, freetype_face->glyph->bitmap.width, freetype_face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, freetype_face->glyph->bitmap.buffer);
		assert(glGetError() == GL_NO_ERROR);

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		assert(glGetError() == GL_NO_ERROR);

		//テクスチャ書き込み
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		glBindFramebuffer(GL_FRAMEBUFFER, buffer.framebuffer);
		glViewport(
			0, 0,
			FONT_ATLAS_TEXTURE_SIZE_WIDTH, FONT_ATLAS_TEXTURE_SIZE_HEIGHT
		);
		assert(glGetError() == GL_NO_ERROR);

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

		glUseProgram(shader.glProgramId);
		glEnableVertexAttribArray(shader.positionLocation);
		glEnableVertexAttribArray(shader.uvLocation);
		glUniform1i(shader.textureLocation, 0);

		const float scWidth = FONT_ATLAS_TEXTURE_SIZE_WIDTH;
		const float scHeight = FONT_ATLAS_TEXTURE_SIZE_HEIGHT;
		// アスペクト補正用行列
		const GLfloat surfaceAspect = (GLfloat)FONT_ATLAS_TEXTURE_SIZE_WIDTH / (GLfloat)FONT_ATLAS_TEXTURE_SIZE_HEIGHT;
		const mat4 aspect = mat4_scale(1, surfaceAspect, 1);

		const GLfloat xScale = (GLfloat)freetype_face->glyph->bitmap.width / (GLfloat)scWidth * 2.0f;
		const GLfloat yScale = (GLfloat)freetype_face->glyph->bitmap.rows / (GLfloat)scHeight * 2.0f;

		const mat4 scale = mat4_scale(xScale, yScale, 0);

		float pos_x = x;
		//float pos_y = FONT_ATLAS_TEXTURE_SIZE_HEIGHT - FONT_ATLAS_ONE_DATA_SIZE_HEIGHT;
		float pos_y = 0;

		// 移動行列を作成
		// 左上座標は元座標とスケーリング値から計算可能。1.0f - xScaleしているのは目減りした量を計算するため。
		const GLfloat vertexLeft = 0.5f + (1.0f - xScale) * 0.5f;
		const GLfloat vertexTop = 0.5f + (1.0f - (yScale * surfaceAspect)) * 0.5f;
		const GLfloat moveX = (pos_x) / (GLfloat)scWidth * 2.0f;
		const GLfloat moveY = -((pos_y) / (GLfloat)scHeight * 2.0f);

		// 左上に移動し、さらに右下方へオフセットさせる
		mat4 translate = mat4_translate(-vertexLeft + moveX, vertexTop + moveY, 0);
		const mat4 rotate = mat4_rotate(vec3_create(1, 0, 0), 180);

		mat4 matrix = mat4_identity();
		matrix = mat4_multiply(translate, aspect);
		matrix = mat4_multiply(matrix, rotate);
		matrix = mat4_multiply(matrix, scale);

		glUniformMatrix4fv(shader.unif_matrix, 1, GL_FALSE, (GLfloat*)matrix.m);
		glVertexAttribPointer(shader.positionLocation, 2, GL_FLOAT, false, 0, vertex_position);
		glVertexAttribPointer(shader.uvLocation, 2, GL_FLOAT, false, 0, vertex_uv);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &texture);
	}
	return true;
}

Character* FontManager::GetCharacterData(wchar_t _c) {
	auto found = characters.find(_c);
	if (found == end(characters)) {
		return nullptr;
	}
	return found->second;
}

void FontManager::CrateShader()
{
	GLuint vShaderId = glCreateShader(GL_VERTEX_SHADER);
	std::string vertexShader = R"#(
    attribute vec3 position;
    attribute vec2 uv;
    varying vec2 vuv;
    uniform mat4 unif_matrix;
    void main(void){
        gl_Position = unif_matrix * vec4(position, 1.0);
        vuv = uv;
    }
    )#";
	const char* vs = vertexShader.c_str();
	glShaderSource(vShaderId, 1, &vs, NULL);
	glCompileShader(vShaderId);

	GLuint fShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragmentShader = R"#(
    varying vec2 vuv;
    uniform sampler2D texture;
    void main(void){
        //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0) * texture2D(texture, vuv).r;
		gl_FragColor = texture2D(texture, vuv);
    }
    )#";
	const char* fs = fragmentShader.c_str();
	glShaderSource(fShaderId, 1, &fs, NULL);
	glCompileShader(fShaderId);

	shader.glProgramId = glCreateProgram();
	glAttachShader(shader.glProgramId, vShaderId);
	glAttachShader(shader.glProgramId, fShaderId);
	glLinkProgram(shader.glProgramId);
}

bool FontManager::isSpecialCharacter(wchar_t _c) {
	if (_c == NEW_LINE_CODE) {
		return true;
	}
	return false;
}

bool FontManager::isHalfSpaceCharacter(wchar_t _c) {
	if (_c == HALF_WIDTH_SPACE_CODE) {
		return true;
	}
	return false;
}

