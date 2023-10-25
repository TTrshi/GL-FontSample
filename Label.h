#pragma once
#include "fontManager.h"
#include <vector>

typedef enum
{
	eTYPE_LABEL_ALIGN_LEFT,			//左揃え
	eTYPE_LABEL_ALIGN_CENTER,		//中央揃え
	eTYPE_LABEL_ALIGN_RIGHT			//右揃え
}ENUM_TYPE_LABEL_ALIGN;

class LabelCharacter {
public:
	glm::vec2 position;
	wchar_t character;
	glm::vec2 size;

	LabelCharacter(glm::vec2 _position, glm::vec2 _size, wchar_t _character)
	{
		position = _position;
		size = _size;
		character = _character;
	}
};

class LabelWard {
public:
	std::wstring wtext;
	glm::vec2 position;
	float width;
	std::vector<LabelCharacter*> labelCharacters;

	LabelWard()
	{
		wtext.clear();
		position = glm::vec2(0, 0);
		width = 0;
	}
};

class Label
{
private:
	std::string text;
	std::wstring wtext;

	
	float fontSize;
	float fontScale;
	float lineSpace;
	float characterSpace;
	ENUM_TYPE_LABEL_ALIGN textAlign;
	glm::vec3 color;
	glm::vec2 anchor;

	struct {
		glm::vec2 position;
		glm::vec2 size;
		std::vector<LabelCharacter*> labelCharacters;
	}canvas;


	
public:
	Label();
	~Label();

	void SetString(std::string _str);
	void SetFontSize(float _size) { fontSize = _size;}
	void SetCanvasSize(const glm::vec2 _size) { canvas.size = _size; }
	void SetFontColor(const glm::fvec3 _color) { color = _color;}
	void SetLineSpace(float _space) { lineSpace = _space; }
	void SetCharacterSpace(float _space) { characterSpace = _space; }
	void SetAlign(ENUM_TYPE_LABEL_ALIGN _align) { textAlign = _align; }

	void Update();

	void Draw(GLuint programId);
};