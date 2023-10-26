#define _CRT_SECURE_NO_WARNINGS
#include "Label.h"
#include "strconv.h"

#include <stdlib.h>
#include <wchar.h>
#include <iostream>
#include "support_gl_Vector.h"

Label::Label() :
	text(""),
	wtext(L""),
	fontSize(CREATE_FONT_SIZE),
	fontScale(1.0f)

{
	textAlign = eTYPE_LABEL_ALIGN_LEFT;
	lineSpace = 0;
	characterSpace = 0;
	canvas.position = glm::vec2(0, 0);
	canvas.size = glm::vec2(1280, 720);

}

Label::~Label() {

}

void Label::SetString(std::string _str) {
	text = _str;
	//wtext = utf8_to_wide(text);
	wtext = L"ABCpyj\nABCDEFG";
	
	char src[] = u8"あいうえお";
	char* ploc = setlocale(LC_ALL, "ja_JP.utf8");
	size_t len = mbstowcs(nullptr, src, 0);
	if (len != (size_t)-1)
	{
		wchar_t* pdst = new wchar_t[len + 1];
		memset(pdst, 0, (sizeof(wchar_t) * (len + 1)));
		mbstowcs(pdst, src, (len + 1));
		ploc = setlocale(LC_ALL, "japanese");
		std::wcout << pdst << std::endl;
		delete[] pdst;
	}


	for (int i = 0; wtext[i] != '\0'; i++) {
		FontManager::GetInstance()->CreateCharacter(wtext[i]);
	}
}

void Label::Update() {
	int index = 0;
	int lineIndex = 0;
	int newLineIndex = 0;
	int totalLineIndex = 0;
	float initPosY = canvas.size.y - fontSize;
	float curPosX = 0;
	float curPosY = initPosY;
	std::vector<std::vector<LabelCharacter*>> tmpLabelCharacters;
	std::vector<float> tmpLineWidths;
	float lineInterval = fontSize + lineSpace;
	glm::vec2 charactorPos;

	//文字情報のクリア
	for (LabelCharacter* labelCh : canvas.labelCharacters) {
		delete labelCh;
	}
	canvas.labelCharacters.clear();

	//Scaleの計算
	fontScale = (float)fontSize / (float)CREATE_FONT_SIZE;

	//単語ごとに分割
	std::wstring wardStr = L"";
	std::vector<LabelWard*> tmpLabelWard;
	LabelWard* labelWard = new LabelWard();
	float wardWidth = 0;

	charactorPos.x = 0;
	charactorPos.y = 0;
	for (int i = 0; wtext[i] != '\0'; i++) {
		wardStr += wtext[i];
		if (wtext[i] == NEW_LINE_CODE) {
			labelWard->width = wardWidth;
			labelWard->wtext = wardStr;
			tmpLabelWard.push_back(labelWard);
			labelWard = new LabelWard();
			wardStr = L"";
			continue;
		}
		
		Character* character = FontManager::GetInstance()->GetCharacterData(wtext[i]);
		float nextOffsetX = (float)(character->advance >> 6) * fontScale + characterSpace;
		labelWard->labelCharacters.push_back(new LabelCharacter(charactorPos, glm::vec2(nextOffsetX, fontSize + lineSpace), wtext[i]));

		charactorPos.x += nextOffsetX;
		wardWidth += nextOffsetX;

		if (wtext[i] == HALF_WIDTH_SPACE_CODE) {
			labelWard->width = wardWidth;
			labelWard->wtext = wardStr;
			tmpLabelWard.push_back(labelWard);
			labelWard = new LabelWard();
			wardStr = L"";
		}
	}
	if (wardStr.size()) {
		labelWard->width = wardWidth;
		labelWard->wtext = wardStr;
		tmpLabelWard.push_back(labelWard);
	}

	//文字の配置処理
	curPosX = 0;
	curPosY = initPosY;
	for (int i = 0; i < tmpLabelWard.size(); i++) {
		float x = curPosX + tmpLabelWard[i]->width;
		//表示領域内に文字が収まるか判定
		if (canvas.size.x <= tmpLabelWard[i]->width && curPosX == 0.0f) {//TODO: floatの
			//改行せずそのまま表示
		}
		else if (canvas.size.x <= x) {
			lineIndex++;
			x = 0;
			totalLineIndex = lineIndex + newLineIndex;
			curPosX = 0;
			curPosY = initPosY + totalLineIndex * lineInterval;
		}

		if (i != 0) {
			lineIndex++;
			totalLineIndex = lineIndex + newLineIndex;
			curPosX = 0;
			curPosY = initPosY + totalLineIndex * lineInterval;
		}

		//一文字ごとに配置処理
		for (int k = 0; k < tmpLabelWard[i]->labelCharacters.size(); k++) {
			LabelCharacter* labelCh = tmpLabelWard[i]->labelCharacters[k];
			//改行コード
			/*
			if (labelWard->wtext[i] == NEW_LINE_CODE) {
				newLineIndex++;
				totalLineIndex = lineIndex + newLineIndex;
				curPosX = 0;
				curPosY = initPosY - totalLineIndex * lineInterval;
				if (tmpLabelCharacters.size() <= totalLineIndex) {
					tmpLabelCharacters.resize(totalLineIndex + 1);
					tmpLineWidths.resize(totalLineIndex + 1);
				}
				tmpLineWidths[totalLineIndex] = curPosX;
				continue;
			}
			*/

			//表示領域外に出た場合は改行
			if (canvas.size.x <= (curPosX + labelCh->size.x)) {
				lineIndex++;
				totalLineIndex = lineIndex + newLineIndex;
				curPosX = 0;
				curPosY = initPosY - totalLineIndex * lineInterval;
			}

			labelCh->position.x = curPosX;
			labelCh->position.y = curPosY;

			totalLineIndex = lineIndex + newLineIndex;
			if (tmpLabelCharacters.size() <= totalLineIndex) {
				tmpLabelCharacters.resize(totalLineIndex + 1);
				tmpLineWidths.resize(totalLineIndex + 1);
			}
			tmpLabelCharacters[totalLineIndex].push_back(labelCh);

			curPosX += labelCh->size.x;
			tmpLineWidths[totalLineIndex] = curPosX;
		}
	}

	//アライメント処理
	for (int i = 0; i < tmpLabelCharacters.size(); i++) {
		int width = tmpLineWidths[i];
		float offsetX = 0;
		switch (textAlign)
		{
		case eTYPE_LABEL_ALIGN_LEFT:
			break;
		case eTYPE_LABEL_ALIGN_CENTER:
			offsetX = (float)(canvas.size.x - width) / 2.0f;
			break;
		case eTYPE_LABEL_ALIGN_RIGHT:
			offsetX = (float)(canvas.size.x - width);
			break;
		default:
			break;
		}

		for (int j = 0; j < tmpLabelCharacters[i].size(); j++) {
			LabelCharacter* labelCh = tmpLabelCharacters[i][j];
			labelCh->position.x += offsetX;
			canvas.labelCharacters.push_back(labelCh);
		}
	}

	//Destroy
	for (int i = 0; i < tmpLabelWard.size(); i++) {
		labelWard = tmpLabelWard[i];
		delete labelWard;
	}
	tmpLabelWard.clear();
}

//#define FONT_POS_CONV_X(w)	(((float)(w) / canvas.size.x) * 2.0f - 1.0f)
//#define FONT_POS_CONV_Y(h)	(((float)(h) / canvas.size.y) * 2.0f - 1.0f)
#define FONT_POS_CONV_X(w)	(((float)(w) / canvas.size.x) * 2.0f - 1.0f)
#define FONT_POS_CONV_Y(h)	(((float)(h) / canvas.size.y) * 2.0f - 1.0f)
#if 0
void Label::Draw(GLuint programId) {
	glUseProgram(programId);


	//float x = 0.0f;
	//float y = 0.0f;
	
	for (int i = 0; i < canvas.labelCharacters.size(); i++) {
		LabelCharacter* LabelCharacter = canvas.labelCharacters[i];
		Character* character = FontManager::GetInstance()->GetCharacterData(LabelCharacter->character);
		
		glm::vec2 pos = LabelCharacter->position;
		GLfloat xpos = pos.x;
		GLfloat ypos = pos.y;
		GLfloat w = character->size.x * fontScale;
		GLfloat h = character->size.y * fontScale;

		float vertex_position[] = {
			0.5f, 0.5f,
			-0.5f, 0.5f,
			-0.5f, -0.5f,
			0.5f, -0.5f
			//FONT_POS_CONV_X(xpos + w), FONT_POS_CONV_Y(ypos + h),
			//FONT_POS_CONV_X(xpos), FONT_POS_CONV_Y(ypos + h),
			//FONT_POS_CONV_X(xpos), FONT_POS_CONV_Y(ypos),
			//FONT_POS_CONV_X(xpos + w), FONT_POS_CONV_Y(ypos)
		};

		/*
		const GLfloat vertex_uv[] = {
			character->uvRect.z, character->uvRect.y,
			character->uvRect.x, character->uvRect.y,
			character->uvRect.x, character->uvRect.w,
			character->uvRect.z, character->uvRect.w,
		};
		*/
		const GLfloat vertex_uv[] = {
			1, 0,
			0, 0,
			0, 1,
			1, 1,
		};

		// 何番目のattribute変数か
		int positionLocation = glGetAttribLocation(programId, "position");
		int uvLocation = glGetAttribLocation(programId, "uv");
		int textureLocation = glGetUniformLocation(programId, "texMap");
		int unif_matrix = glGetUniformLocation(programId, "unif_matrix");
		int unif_uvmatrix = glGetUniformLocation(programId, "unif_uvmatrix");


		/*
		GLuint MatrixID = glGetUniformLocation(programId, "MVP");

		float horizontalAngle = 3.14f;
		// Initial vertical angle : none
		float verticalAngle = 0.0f;
		glm::vec3 direction(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
		);

		// Right vector
		glm::vec3 right = glm::vec3(
			sin(horizontalAngle - 3.14f / 2.0f),
			0,
			cos(horizontalAngle - 3.14f / 2.0f)
		);

		// Up vector
		glm::vec3 up = glm::cross(right, direction);
		glm::vec3 position = glm::vec3(0, 0, 5);
		// Projection matrix : 45ｰ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		glm::mat4 Projection = glm::perspective(45.0f, (float)1280 / (float)720, 0.1f, 100.0f);
		position += direction * 2.5f;
		// Camera matrix
		glm::mat4 View = glm::lookAt(
			position, // Camera is at (4,3,3), in World Space
			position + direction, // and looks at the origin
			up  // Head is up (set to 0,-1,0 to look upside-down)
		);
		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 Model = glm::mat4(1.0f);
		// Our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
		*/


		// attribute属性を有効にする
		glEnableVertexAttribArray(positionLocation);
		glEnableVertexAttribArray(uvLocation);

		const float scWidth = 1280;
		const float scHeight = 720;
		// アスペクト補正用行列
		const GLfloat surfaceAspect = (GLfloat)scWidth / (GLfloat)scHeight;
		const mat4 aspect = mat4_scale(1, surfaceAspect, 1);

		const GLfloat xScale = (GLfloat)character->size.x / (GLfloat)scWidth * 2.0f;
		const GLfloat yScale = (GLfloat)character->size.y / (GLfloat)scHeight * 2.0f;

		const mat4 scale = mat4_scale(xScale, yScale, 0);

		float pos_x = pos.x + character->bearing.x;
		//float pos_y = pos.y - (character->size.y - character->bearing.y);
		static float ss_ = 2.0f;
		//ss_ += 0.001f;
		//float pos_y = 300 - character->size.y*2.0f - (character->bearing.y)* ss_;
		float pos_y = pos.y - (character->bearing.y) * ss_;

		// 移動行列を作成
		// 左上座標は元座標とスケーリング値から計算可能。1.0f - xScaleしているのは目減りした量を計算するため。
		const GLfloat vertexLeft = 0.5f + (1.0f - xScale) * 0.5f;
		const GLfloat vertexTop = 0.5f + (1.0f - (yScale * surfaceAspect)) * 0.5f;
		const GLfloat moveX = (pos_x) / (GLfloat)scWidth * 2.0f;
		const GLfloat moveY = -((pos_y) / (GLfloat)scHeight * 2.0f);

		// 左上に移動し、さらに右下方へオフセットさせる
		mat4 translate = mat4_translate(-vertexLeft + moveX, vertexTop + moveY, 0);
		const mat4 rotate = mat4_rotate(vec3_create(1, 0, 0), 0);

		mat4 matrix = mat4_identity();
		matrix = mat4_multiply(translate, aspect);
		matrix = mat4_multiply(matrix, rotate);
		matrix = mat4_multiply(matrix, scale);

		/*
		const GLfloat ancMoveX2 = (GLfloat)(spriteWidth) / (GLfloat)_size.width * _anchorpoint.x;
		const GLfloat ancMoveY2 = (GLfloat)(spriteHeight) / (GLfloat)_size.width * _anchorpoint.y;
		const mat4 rotTranslate0 = mat4_translate(ancMoveX2, -ancMoveY2, 0);
		_mat = mat4_multiply(_mat, rotTranslate0);
		_mat = mat4_multiply(_mat, rotate);
		const mat4 rotTranslate1 = mat4_translate(-ancMoveX2, ancMoveY2, 0);
		_mat = mat4_multiply(_mat, rotTranslate1);

		_mat = mat4_multiply(_mat, scale);
		*/


		// uniform属性を設定する
		glUniform1i(textureLocation, 0);
		glUniformMatrix4fv(unif_matrix, 1, GL_FALSE, (GLfloat*)matrix.m);

		{
			GLfloat x = ((i % FONT_ATLAS_LINE_LENGTH) * FONT_ATLAS_ONE_DATA_SIZE_WIDTH);
			GLfloat y = FONT_ATLAS_TEXTURE_SIZE_HEIGHT - character->size.y;
			const GLfloat _xScale = (GLfloat)character->size.x / (GLfloat)FONT_ATLAS_TEXTURE_SIZE_WIDTH;
			const GLfloat _yScale = (GLfloat)character->size.y / (GLfloat)FONT_ATLAS_TEXTURE_SIZE_HEIGHT;

			const mat4 _scale = mat4_scale(_xScale, _yScale, 0);

			// 移動を行う
			const GLfloat _xMove = (GLfloat)x / (GLfloat)FONT_ATLAS_TEXTURE_SIZE_WIDTH;
			const GLfloat _yMove = (GLfloat)y / (GLfloat)FONT_ATLAS_TEXTURE_SIZE_HEIGHT;
			const mat4 _translate = mat4_translate(_xMove, _yMove, 0);

			mat4 _matrix = mat4_multiply(_translate, _scale);
			//mat4 _matrix = mat4_identity();
			glUniformMatrix4fv(unif_uvmatrix, 1, GL_FALSE, (GLfloat*)_matrix.m);
		}

		// attribute属性を登録
		glVertexAttribPointer(positionLocation, 2, GL_FLOAT, false, 0, vertex_position);
		glVertexAttribPointer(uvLocation, 2, GL_FLOAT, false, 0, vertex_uv);


		// モデルの描画
		glBindTexture(GL_TEXTURE_2D, FontManager::GetInstance()->GetFontTextureId());
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
}
#else
void Label::Draw(GLuint programId) {
	glUseProgram(programId);


	//float x = 0.0f;
	//float y = 0.0f;

	for (int i = 0; i < canvas.labelCharacters.size(); i++) {
		LabelCharacter* LabelCharacter = canvas.labelCharacters[i];
		Character* character = FontManager::GetInstance()->GetCharacterData(LabelCharacter->character);
		glm::vec2 pos = LabelCharacter->position;
		//GLfloat xpos = pos.x;
		//GLfloat ypos = pos.y;
		//GLfloat w = character->size.x * fontScale;
		//GLfloat h = character->size.y * fontScale;

		float vertex_position[] = {
			0.5f, 0.5f,
			-0.5f, 0.5f,
			-0.5f, -0.5f,
			0.5f, -0.5f
			//FONT_POS_CONV_X(xpos + w), FONT_POS_CONV_Y(ypos + h),
			//FONT_POS_CONV_X(xpos), FONT_POS_CONV_Y(ypos + h),
			//FONT_POS_CONV_X(xpos), FONT_POS_CONV_Y(ypos),
			//FONT_POS_CONV_X(xpos + w), FONT_POS_CONV_Y(ypos)
		};

		/*
		const GLfloat vertex_uv[] = {
			character->uvRect.z, character->uvRect.y,
			character->uvRect.x, character->uvRect.y,
			character->uvRect.x, character->uvRect.w,
			character->uvRect.z, character->uvRect.w,
		};
		*/
		const GLfloat vertex_uv[] = {
			1, 0,
			0, 0,
			0, 1,
			1, 1,
		};

		// 何番目のattribute変数か
		int positionLocation = glGetAttribLocation(programId, "position");
		int uvLocation = glGetAttribLocation(programId, "uv");
		int textureLocation = glGetUniformLocation(programId, "texMap");
		int unif_matrix = glGetUniformLocation(programId, "unif_matrix");
		int unif_uvmatrix = glGetUniformLocation(programId, "unif_uvmatrix");

		// attribute属性を有効にする
		glEnableVertexAttribArray(positionLocation);
		glEnableVertexAttribArray(uvLocation);

		const float scWidth = 1280;
		const float scHeight = 720;
		// アスペクト補正用行列
		const GLfloat surfaceAspect = (GLfloat)scWidth / (GLfloat)scHeight;
		const mat4 aspect = mat4_scale(1, surfaceAspect, 1);

		//const GLfloat xScale = (GLfloat)character->size.x / (GLfloat)scWidth * 2.0f;
		//const GLfloat yScale = (GLfloat)character->size.y / (GLfloat)scHeight * 2.0f;
		const GLfloat xScale = (GLfloat)40 / (GLfloat)scWidth * 2.0f;
		const GLfloat yScale = (GLfloat)40 / (GLfloat)scHeight * 2.0f;

		const mat4 scale = mat4_scale(xScale, yScale, 0);
		//const mat4 scale = mat4_scale(1.0, 1.0, 1.0);

		//float pos_x = pos.x + character->bearing.x;
		float pos_x = 0;
		//float pos_y = pos.y - (character->size.y - character->bearing.y);
		static float ss_ = 2.0f;
		//ss_ += 0.001f;
		//float pos_y = 300 - character->size.y*2.0f - (character->bearing.y)* ss_;
		//float pos_y = pos.y - (character->bearing.y) * ss_;
		float pos_y = 0;

		// 移動行列を作成
		// 左上座標は元座標とスケーリング値から計算可能。1.0f - xScaleしているのは目減りした量を計算するため。
		const GLfloat vertexLeft = 0.5f + (1.0f - xScale) * 0.5f;
		const GLfloat vertexTop = 0.5f + (1.0f - (yScale * surfaceAspect)) * 0.5f;
		const GLfloat moveX = (pos_x) / (GLfloat)scWidth * 2.0f;
		const GLfloat moveY = -((pos_y) / (GLfloat)scHeight * 2.0f);

		// 左上に移動し、さらに右下方へオフセットさせる
		mat4 translate = mat4_translate(-vertexLeft + moveX, vertexTop + moveY, 0);
		//mat4 translate = mat4_translate(0, 0, 0);
		const mat4 rotate = mat4_rotate(vec3_create(1, 0, 0), 0);

		mat4 matrix = mat4_identity();
		matrix = mat4_multiply(translate, aspect);
		matrix = mat4_multiply(matrix, rotate);
		matrix = mat4_multiply(matrix, scale);

		/*
		const GLfloat ancMoveX2 = (GLfloat)(spriteWidth) / (GLfloat)_size.width * _anchorpoint.x;
		const GLfloat ancMoveY2 = (GLfloat)(spriteHeight) / (GLfloat)_size.width * _anchorpoint.y;
		const mat4 rotTranslate0 = mat4_translate(ancMoveX2, -ancMoveY2, 0);
		_mat = mat4_multiply(_mat, rotTranslate0);
		_mat = mat4_multiply(_mat, rotate);
		const mat4 rotTranslate1 = mat4_translate(-ancMoveX2, ancMoveY2, 0);
		_mat = mat4_multiply(_mat, rotTranslate1);

		_mat = mat4_multiply(_mat, scale);
		*/


		// uniform属性を設定する
		glUniform1i(textureLocation, 0);
		glUniformMatrix4fv(unif_matrix, 1, GL_FALSE, (GLfloat*)matrix.m);

		{
			GLfloat x = ((i % FONT_ATLAS_LINE_LENGTH) * FONT_ATLAS_ONE_DATA_SIZE_WIDTH);
			GLfloat y = FONT_ATLAS_TEXTURE_SIZE_HEIGHT - character->size.y;
			const GLfloat _xScale = (GLfloat)character->size.x / (GLfloat)FONT_ATLAS_TEXTURE_SIZE_WIDTH;
			const GLfloat _yScale = (GLfloat)character->size.y / (GLfloat)FONT_ATLAS_TEXTURE_SIZE_HEIGHT;

			const mat4 _scale = mat4_scale(_xScale, _yScale, 0);
			//const mat4 _scale = mat4_scale(1.0, 1.0, 1.0);

			// 移動を行う
			const GLfloat _xMove = (GLfloat)x / (GLfloat)FONT_ATLAS_TEXTURE_SIZE_WIDTH;
			const GLfloat _yMove = (GLfloat)y / (GLfloat)FONT_ATLAS_TEXTURE_SIZE_HEIGHT;
			const mat4 _translate = mat4_translate(_xMove, _yMove, 0);
			//const mat4 _translate = mat4_translate(0, 0, 0);

			mat4 _matrix = mat4_multiply(_translate, _scale);
			//mat4 _matrix = mat4_identity();
			glUniformMatrix4fv(unif_uvmatrix, 1, GL_FALSE, (GLfloat*)_matrix.m);
		}

		// attribute属性を登録
		glVertexAttribPointer(positionLocation, 2, GL_FLOAT, false, 0, vertex_position);
		glVertexAttribPointer(uvLocation, 2, GL_FLOAT, false, 0, vertex_uv);


		// モデルの描画
		glBindTexture(GL_TEXTURE_2D, FontManager::GetInstance()->GetFontTextureId());
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		break;
	}
}
#endif
