#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <fstream>
#include <vector>

#include "fontManager.h"
#include "Label.h"
#include "shaderCompile.h"
#include "support_gl_Vector.h"
#include "rect.h"

using namespace std;

const int g_width  = 1280;
const int g_height = 720;

#define RENDER_WIDTH 1280
#define RENDER_HEIGHT 720

GLuint crateShader()
{
#if 1
    //バーテックスシェーダのコンパイル
    GLuint vShaderId = glCreateShader(GL_VERTEX_SHADER);
    
    string vertexShader = R"#(
    attribute vec3 position;
    attribute vec4 uv;
    varying vec2 vuv;
    //uniform mat4 MVP;
    uniform mat4 unif_matrix;
    uniform mat4 unif_uvmatrix;
    void main(void){
        gl_Position = unif_matrix * vec4(position, 1.0);
        vuv = (unif_uvmatrix * uv).xy;
    }
    )#";

    const char* vs = vertexShader.c_str();
    /*
    const char* vs =
        "#version 320 es\n"
        //"precision highp float;\n"
        //"precision lowp int;\n"
        "layout( location = 0 ) in vec3 a_position;\n"
        "layout( location = 1 ) in vec2 a_texCoord;\n"
        "uniform mediump mat4 unif_matrix;\n"
        "out vec2 v_texCoord;\n"
        "void main(){\n"
        "	gl_Position = unif_matrix * vec4(a_position, 1.0);\n"
        "	v_texCoord = a_texCoord;\n"
        "}\n";
        */
    glShaderSource(vShaderId, 1, &vs, NULL);
    glCompileShader(vShaderId);
    
    //フラグメントシェーダのコンパイル
    GLuint fShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    
    string fragmentShader = R"#(
    varying vec2 vuv;
    uniform sampler2D texture;
    void main(void){
        gl_FragColor = texture2D(texture, vuv);
        //gl_FragColor.a = 0.5;
    }
    )#";
    const char* fs = fragmentShader.c_str();
    /*
    const char* fs =
        "#version 320 es\n"
        //"precision highp float;\n"
        //"precision lowp int;\n"
        "layout( location = 0 ) out vec4 o_Color;\n"
        "in vec2 v_texCoord;\n"
        "uniform lowp sampler2D texMap; \n"
        "void main() {\n"
        "	o_Color = texture(texMap, v_texCoord);\n"
        "}\n";
    */
    glShaderSource(fShaderId, 1, &fs, NULL);
    glCompileShader(fShaderId);
    
    //プログラムオブジェクトの作成
    GLuint programId = glCreateProgram();
    glAttachShader(programId,vShaderId);
    glAttachShader(programId,fShaderId);
    
    // リンク
    glLinkProgram(programId);
    
    glUseProgram(programId);
#endif
    /*
    const char* vs =
        "#version 320 es\n"
        //"precision highp float;\n"
        //"precision lowp int;\n"
        "layout( location = 0 ) in vec3 a_position;\n"
        "layout( location = 1 ) in vec2 a_texCoord;\n"
        "uniform mediump mat4 unif_matrix;\n"
        "out vec2 v_texCoord;\n"
        "void main(){\n"
        "	gl_Position = unif_matrix * vec4(a_position, 1.0);\n"
        "	v_texCoord = a_texCoord;\n"
        "}\n";
    const char* fs =
        "#version 320 es\n"
        //"precision highp float;\n"
        //"precision lowp int;\n"
        "layout( location = 0 ) out vec4 o_Color;\n"
        "in vec2 v_texCoord;\n"
        "uniform lowp sampler2D texMap; \n"
        "void main() {\n"
        "	o_Color = texture(texMap, v_texCoord);\n"
        "}\n";
    GLuint programId = Shader_createProgramFromSource(vs, fs);
    */
    assert(glGetError() == GL_NO_ERROR);
    return programId;
}

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
    GLint unif_uvmatrix;
}shader;
void createRenderBuffer() {
    glGenFramebuffers(1, &buffer.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, buffer.framebuffer);

    glGenTextures(1, &buffer.fontRenderedTexture);
    glBindTexture(GL_TEXTURE_2D, buffer.fontRenderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, RENDER_WIDTH, RENDER_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenRenderbuffers(1, &buffer.depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, buffer.depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, RENDER_WIDTH, RENDER_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer.depthrenderbuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.fontRenderedTexture, 0);
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

    //
    {
        //バーテックスシェーダのコンパイル
        GLuint vShaderId = glCreateShader(GL_VERTEX_SHADER);

        string vertexShader = R"#(
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

        string fragmentShader = R"#(
    varying vec2 vuv;
    uniform sampler2D texture;
    void main(void){
        gl_FragColor = texture2D(texture, vuv);
        //gl_FragColor.a = 0.2;
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

GLuint loadTexture(string filename)
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // テクスチャのアンバインド
    delete[] textureBuffer;
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return texID;
}

void matrix_sprite(
    float rect_x, float rect_y, float rect_width, float rect_height, 
    float scaling_x, float scaling_y,
    float rotation_x, float rotation_y, float rotation_z,
    float viewport_width, float viewport_height,
    mat4& mat_result)
{
    float sprite_x = rect_x;
    float sprite_y = rect_y;
    float sprite_width = rect_width * scaling_x;
    float sprite_height = rect_height * scaling_y;

    float surface_aspect = viewport_width / viewport_height;
    mat4 mat_aspect = mat4_scale(1, surface_aspect, 1);

    float scale_x = sprite_width / viewport_width * 2.0f;
    float scale_y = sprite_height / viewport_width * 2.0f;
    mat4 mat_scale = mat4_scale(scale_x, scale_y, 0);

    float vertex_left = 0.5f + (1.0f - scale_x) * 0.5f;
    float vertex_top = 0.5f + (1.0f - (scale_y * surface_aspect)) * 0.5f;
    float translate_x = sprite_x / viewport_width * 2.0f;
    float translate_y = sprite_y / viewport_height * 2.0f;
    mat4 mat_translate = mat4_translate(-vertex_left + translate_x, vertex_top - translate_y, 0);

    mat4 mat_rotate_x = mat4_rotate(vec3_create(1, 0, 0), rotation_x);
    mat4 mat_rotate_y = mat4_rotate(vec3_create(0, 1, 0), rotation_y);
    mat4 mat_rotate_z = mat4_rotate(vec3_create(0, 0, 1), rotation_z);
    mat4 mat_rotate = mat4_identity();
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
    mat4& mat_result)
{
    float scale_x = uv_rect_width / texture_width;
    float scale_y = uv_rect_height / texture_height;
    mat4 mat_scale = mat4_scale(scale_x, scale_y, 0);

    float translate_x = uv_rect_x / texture_width;
    float translate_y = uv_rect_y / texture_height;
    mat4 mat_translate = mat4_translate(translate_x, translate_y, 0);

    mat_result = mat4_multiply(mat_translate, mat_scale);
}

int main()
{
    if( !glfwInit() ){
        return -1;
    }
    
    GLFWwindow* window = glfwCreateWindow(g_width, g_height, "Simple", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    // モニタとの同期
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glewInit();
    //glEnable(GL_CULL_FACE);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::wstring    wstr = L"Hello world.";
    FontManager::GetInstance()->Initialize();
    FontManager::GetInstance()->CreateFont("C:\\Windows\\Fonts\\meiryo.ttc");
    
    GLuint programId = crateShader();
    assert(glGetError() == GL_NO_ERROR);
    //GLuint texID = loadTexture("cat.raw");

    createRenderBuffer();

    Label* label = new Label();
    std::string    str = "あい";
    label->SetString(str);
    label->SetAlign(eTYPE_LABEL_ALIGN_RIGHT);
    label->SetCanvasSize(glm::vec2(200, 400));
    label->Update();

    

    // ゲームループ
    while (!glfwWindowShouldClose(window)) {
        assert(glGetError() == GL_NO_ERROR);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
       /*
        glBindFramebuffer(GL_FRAMEBUFFER, buffer.framebuffer);
        glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);
        glClearColor(0.0, 0.0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearDepth(1.0);

        label->Draw(programId);
        */

        /*
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, g_width, g_height);
        glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearDepth(1.0);
        label->Draw(programId);
        */
        //===============================================================
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, g_width, g_height);
        glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearDepth(1.0);
        

        
        {
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
            //glUniform1i(shader.textureLocation, buffer.fontRenderedTexture);
            glUniform1i(shader.textureLocation, 0);
            assert(glGetError() == GL_NO_ERROR);
            
            float scWidth = g_width;
            float scHeight = g_height;

            mat4 matrix;
            matrix_sprite(
                scWidth / 4, scHeight / 4, scWidth/2, scHeight/2,
                1.0, 1.0,
                0,0,0,
                g_width, g_height,
                matrix);

            mat4 uv_matrix;
            matrix_sprite_uv(
                0, 1024 / 2, 1024, 512,
                1024, 1024,
                uv_matrix);

            /*
            // アスペクト補正用行列
            const GLfloat surfaceAspect = (GLfloat)scWidth / (GLfloat)scHeight;
            const GLfloat _surfaceAspect = (GLfloat)scHeight / (GLfloat)scWidth;
            const mat4 aspect = mat4_scale(1, _surfaceAspect, 1);
            
            const GLfloat xScale = (GLfloat)(RENDER_WIDTH / 1.0f) / (GLfloat)scWidth * 2.0f;
            const GLfloat yScale = (GLfloat)(RENDER_HEIGHT / 1.0f) / (GLfloat)scHeight * 2.0f;
            
            const mat4 scale = mat4_scale(xScale, yScale, 0);
            
            float pos_x = 0;
            float pos_y = 0;
            
            // 移動行列を作成
            // 左上座標は元座標とスケーリング値から計算可能。1.0f - xScaleしているのは目減りした量を計算するため。
            const GLfloat vertexLeft = 0.5f + (1.0f - xScale) * 0.5f;
            const GLfloat vertexTop = 0.5f + (1.0f - (yScale * surfaceAspect)) * 0.5f;
            const GLfloat moveX = (pos_x) / (GLfloat)scWidth * 2.0f;
            const GLfloat moveY = -((pos_y) / (GLfloat)scHeight * 2.0f);
            
            // 左上に移動し、さらに右下方へオフセットさせる
            //mat4 translate = mat4_translate(-vertexLeft + moveX, vertexTop + moveY, 0);
            mat4 translate = mat4_translate(0, 0, 0);
            mat4 rotate = mat4_rotate(vec3_create(1, 0, 0), 180);
            
            mat4 matrix = mat4_identity();
            //matrix = mat4_multiply(translate, aspect);
            matrix = mat4_multiply(matrix, translate);
            matrix = mat4_multiply(matrix, rotate);
            matrix = mat4_multiply(matrix, scale);
            */
            
            glUniformMatrix4fv(shader.unif_matrix, 1, GL_FALSE, (GLfloat*)matrix.m);
            glUniformMatrix4fv(shader.unif_uvmatrix, 1, GL_FALSE, (GLfloat*)uv_matrix.m);
            glVertexAttribPointer(shader.positionLocation, 2, GL_FLOAT, false, 0, vertex_position);
            glVertexAttribPointer(shader.uvLocation, 2, GL_FLOAT, false, 0, vertex_uv);
            //glBindTexture(GL_TEXTURE_2D, buffer.fontRenderedTexture);
            glBindTexture(GL_TEXTURE_2D, FontManager::GetInstance()->GetFontTextureId());
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
        
        glFinish();
        // バッファの入れ替え
        glfwSwapBuffers(window);
        
        // イベント待ち
        glfwPollEvents();
    }
    
    FontManager::GetInstance()->Finalize();
    glfwTerminate();
    
    return 0;
}
