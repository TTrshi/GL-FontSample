#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext.hpp>

/**
 * 360度系からラジアン角度に修正する
 */
#define degree2radian(degree) ((degree * 3.141592) / 180.0)

/**
 * 2次元ベクトルを保持する構造体
 */
typedef struct vec2 {
    /**
     * X値
     */
    GLfloat x;

    /**
     * Y値
     */
    GLfloat y;
} vec2;

/**
 * 3次元ベクトルを保持する構造体
 */
typedef struct vec3 {
    /**
     * X値
     */
    GLfloat x;

    /**
     * Y値
     */
    GLfloat y;

    /**
     * Z値
     */
    GLfloat z;
} vec3;

/**
 * 4次元ベクトルを保持する構造体
 */
typedef struct vec4 {
    /**
     * X値
     */
    GLfloat x;

    /**
     * Y値
     */
    GLfloat y;

    /**
     * Z値
     */
    GLfloat z;

    /**
     * W値
     */
    GLfloat w;
} vec4;

/**
 * 行列を保持する構造体
 */
typedef struct mat4 {
    GLfloat m[4][4];
} mat4;

/**
 * 2次元ベクトルを生成する
 */
vec2 vec2_create(const GLfloat x, const GLfloat y);

/**
 * 3次元ベクトルを生成する
 */
vec3 vec3_create(const GLfloat x, const GLfloat y, const GLfloat z);

/**
 * 正規化した3次元ベクトルを生成する
 */
vec3 vec3_createNormalized(const GLfloat x, const GLfloat y, const GLfloat z);

/**
 * 3次元ベクトルの長さを取得する
 */
GLfloat vec3_length(const vec3& v);

/**
 * 3次元ベクトルを正規化する
 */
vec3 vec3_normalize(const vec3& v);

/**
 * 3次元ベクトルの内積を計算する
 */
GLfloat vec3_dot(const vec3& v0, const vec3& v1);

/**
 * 3次元ベクトルの外積を計算する
 */
vec3 vec3_cross(const vec3& v0, const vec3& v1);

/**
 * 単位行列を生成する
 */
mat4 mat4_identity();

/**
 * 行列の転置を行う
 */
mat4 mat4_transpose(const mat4& m);

/**
 * 移動行列を作成する
 */
mat4 mat4_translate(const GLfloat x, const GLfloat y, const GLfloat z);

/**
 * 拡縮行列を作成する
 */
mat4 mat4_scale(const GLfloat x, const GLfloat y, const GLfloat z);

/**
 * 回転行列を生成する
 */
mat4 mat4_rotate(const vec3& axis, const GLfloat rotate);

/**
 * 行列の乗算を行う
 */
mat4 mat4_multiply(const mat4& a, const mat4& b);


void posMultiplyMt4(GLfloat* _pos, int _n, const mat4& a, const mat4& b);
void uvMultiplyMt4(GLfloat* _uv, int _n, const mat4& a, const mat4& b);
mat4 mat4_identityPos();
mat4 mat4_identityUv();