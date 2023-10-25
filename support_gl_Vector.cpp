#include "support_gl_Vector.h"
#include <math.h>

/**
 * 2次元ベクトルを生成する
 */
vec2 vec2_create(const GLfloat x, const GLfloat y) {
    vec2 v = { x, y };
    return v;
}

/**
 * 3次元ベクトルを生成する
 */
vec3 vec3_create(const GLfloat x, const GLfloat y, const GLfloat z) {
    vec3 v = { x, y, z };
    return v;
}

/**
 * 3次元ベクトルの長さを取得する
 */
GLfloat vec3_length(const vec3& v) {
    return (GLfloat) sqrt(((double) v.x * (double) v.x) + ((double) v.y * (double) v.y) + ((double) v.z * (double) v.z));
}

/**
 * 3次元ベクトルを正規化する
 */
vec3 vec3_normalize(const vec3& v) {
    const GLfloat len = vec3_length(v);
    return vec3_create(v.x / len, v.y / len, v.z / len);
}

/**
 * 正規化した3次元ベクトルを生成する
 */
vec3 vec3_createNormalized(const GLfloat x, const GLfloat y, const GLfloat z) {
    return vec3_normalize(vec3_create(x, y, z));
}

/**
 * 3次元ベクトルの内積を計算する
 */
GLfloat vec3_dot(const vec3& v0, const vec3& v1) {
    return (v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z);
}

/**
 * 3次元ベクトルの外積を計算する
 */
vec3 vec3_cross(const vec3& v0, const vec3& v1) {
    return vec3_create((v0.y * v1.z) - (v0.z * v1.y), (v0.z * v1.x) - (v0.x * v1.z), (v0.x * v1.y) - (v0.y * v1.x));
}

/**
 * 単位行列を生成する
 */
mat4 mat4_identity() {
    mat4 result;

    int column = 0;
    int row = 0;

    for (column = 0; column < 4; ++column) {
        for (row = 0; row < 4; ++row) {
            if (column == row) {
                result.m[column][row] = 1.0f;
            } else {
                result.m[column][row] = 0.0f;
            }
        }
    }

    return result;
}

/**
 * 行列の転置を行う
 */
mat4 mat4_transpose(const mat4& m) {
    int i;
    int k;
    mat4 result;

    for (i = 0; i < 4; ++i) {
        for (k = 0; k < 4; ++k) {
            // 値の縦横を入れ替える
            result.m[i][k] = m.m[k][i];
        }
    }

    return result;
}

/**
 * 移動行列を作成する
 */
mat4 mat4_translate(const GLfloat x, const GLfloat y, const GLfloat z) {
    mat4 result = mat4_identity();

    result.m[3][0] = x;
    result.m[3][1] = y;
    result.m[3][2] = z;

    return result;
}

/**
 * 拡縮行列を作成する
 */
mat4 mat4_scale(const GLfloat x, const GLfloat y, const GLfloat z) {
    mat4 result = mat4_identity();

    result.m[0][0] = x;
    result.m[1][1] = y;
    result.m[2][2] = z;

    return result;
}

/**
 * 回転行列を生成する
 */
mat4 mat4_rotate(const vec3& axis, const GLfloat rotate) {
    mat4 result;

    const GLfloat x = axis.x;
    const GLfloat y = axis.y;
    const GLfloat z = axis.z;

    const GLfloat c = (GLfloat)cos(degree2radian(rotate));
    const GLfloat s = (GLfloat)sin(degree2radian(rotate));
    {
        result.m[0][0] = (x * x) * (1.0f - c) + c;
        result.m[0][1] = (x * y) * (1.0f - c) - z * s;
        result.m[0][2] = (x * z) * (1.0f - c) + y * s;
        result.m[0][3] = 0;
    }
    {
        result.m[1][0] = (y * x) * (1.0f - c) + z * s;
        result.m[1][1] = (y * y) * (1.0f - c) + c;
        result.m[1][2] = (y * z) * (1.0f - c) - x * s;
        result.m[1][3] = 0;
    }
    {
        result.m[2][0] = (z * x) * (1.0f - c) - y * s;
        result.m[2][1] = (z * y) * (1.0f - c) + x * s;
        result.m[2][2] = (z * z) * (1.0f - c) + c;
        result.m[2][3] = 0;
    }
    {
        result.m[3][0] = 0;
        result.m[3][1] = 0;
        result.m[3][2] = 0;
        result.m[3][3] = 1;
    }

    return result;
}

/**
 * 行列A×行列Bを行う。
 * 頂点に対し、行列B→行列Aの順番で適用することになる。
 */
mat4 mat4_multiply(const mat4& a, const mat4& b) {
    mat4 result;

    int i = 0;
    for (i = 0; i < 4; ++i) {
        result.m[i][0] = a.m[0][0] * b.m[i][0] + a.m[1][0] * b.m[i][1] + a.m[2][0] * b.m[i][2] + a.m[3][0] * b.m[i][3];
        result.m[i][1] = a.m[0][1] * b.m[i][0] + a.m[1][1] * b.m[i][1] + a.m[2][1] * b.m[i][2] + a.m[3][1] * b.m[i][3];
        result.m[i][2] = a.m[0][2] * b.m[i][0] + a.m[1][2] * b.m[i][1] + a.m[2][2] * b.m[i][2] + a.m[3][2] * b.m[i][3];
        result.m[i][3] = a.m[0][3] * b.m[i][0] + a.m[1][3] * b.m[i][1] + a.m[2][3] * b.m[i][2] + a.m[3][3] * b.m[i][3];
    }

    return result;
}


void posMultiplyMt4(GLfloat* _pos, int _n, const mat4& a, const mat4& b) {
	//vec4 result;
	mat4 result;

	int i = 0;
	for (i = 0; i < 4; ++i) {
		result.m[i][0] = a.m[0][0] * b.m[i][0] + a.m[1][0] * b.m[i][1] + a.m[2][0] * b.m[i][2] + a.m[3][0] * b.m[i][3];
		result.m[i][1] = a.m[0][1] * b.m[i][0] + a.m[1][1] * b.m[i][1] + a.m[2][1] * b.m[i][2] + a.m[3][1] * b.m[i][3];
		result.m[i][2] = a.m[0][2] * b.m[i][0] + a.m[1][2] * b.m[i][1] + a.m[2][2] * b.m[i][2] + a.m[3][2] * b.m[i][3];
		result.m[i][3] = a.m[0][3] * b.m[i][0] + a.m[1][3] * b.m[i][1] + a.m[2][3] * b.m[i][2] + a.m[3][3] * b.m[i][3];
	}

	for (i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			_pos[16 * _n + 4 * i + j] = result.m[i][j];
		}
	}
}

void uvMultiplyMt4(GLfloat* _uv, int _n, const mat4& a, const mat4& b) {
	//vec4 result;
	mat4 result;

	/*
	int i = 0;
	for (i = 0; i < 4; ++i) {
		result.m[i][0] = a.m[0][0] * b.m[i][0] + a.m[1][0] * b.m[i][1] + a.m[2][0] * b.m[i][2] + a.m[3][0] * b.m[i][3];
		result.m[i][1] = a.m[0][1] * b.m[i][0] + a.m[1][1] * b.m[i][1] + a.m[2][1] * b.m[i][2] + a.m[3][1] * b.m[i][3];
		result.m[i][2] = a.m[0][2] * b.m[i][0] + a.m[1][2] * b.m[i][1] + a.m[2][2] * b.m[i][2] + a.m[3][2] * b.m[i][3];
		result.m[i][3] = a.m[0][3] * b.m[i][0] + a.m[1][3] * b.m[i][1] + a.m[2][3] * b.m[i][2] + a.m[3][3] * b.m[i][3];
	}
	*/
	result = a;

	/*
	for (i = 0; i < 4; ++i) {
		for (int j = 0; j < 2; ++j) {
			_uv[8 * _n + 2 * i + j] = result.m[i][j];
		}
	}
	*/
	_uv[8 * _n + 0] = result.m[3][0];						_uv[8 * _n + 1] = result.m[3][1];
	_uv[8 * _n + 2] = result.m[3][0];						_uv[8 * _n + 3] = result.m[3][1] + result.m[1][1];
	_uv[8 * _n + 4] = result.m[3][0] + result.m[0][0];		_uv[8 * _n + 5] = result.m[3][1];
	_uv[8 * _n + 6] = result.m[3][0] + result.m[0][0];		_uv[8 * _n + 7] = result.m[3][1] + result.m[1][1];
}

mat4 mat4_identityPos() {
	mat4 result;

	result.m[0][0] = -0.5f; result.m[0][1] = 0.5f; result.m[0][2] = 0; result.m[0][3] = 1.f;
	result.m[1][0] = -0.5f; result.m[1][1] = -0.5f; result.m[1][2] = 0; result.m[1][3] = 1.f;
	result.m[2][0] = 0.5f; result.m[2][1] = 0.5f; result.m[2][2] = 0; result.m[2][3] = 1.f;
	result.m[3][0] = 0.5f; result.m[3][1] = -0.5f; result.m[3][2] = 0; result.m[3][3] = 1.f;

	return result;
}

mat4 mat4_identityUv() {
	mat4 result;

	result.m[0][0] = 0; result.m[0][1] = 0; result.m[0][2] = 0; result.m[0][3] = 0;
	result.m[1][0] = 0; result.m[1][1] = 2; result.m[1][2] = 0; result.m[1][3] = 0;
	result.m[2][0] = 2; result.m[2][1] = 0; result.m[2][2] = 0; result.m[2][3] = 0;
	result.m[3][0] = 2; result.m[3][1] = 2; result.m[3][2] = 0; result.m[3][3] = 0;

	return result;
}