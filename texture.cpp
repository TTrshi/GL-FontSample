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

using namespace std;

const int g_width  = 1280;
const int g_height = 720;

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
    GLuint texID = loadTexture("cat.raw");

    Label* label = new Label();
    std::string    str = "あい";
    label->SetString(str);
    label->SetAlign(eTYPE_LABEL_ALIGN_RIGHT);
    label->SetCanvasSize(glm::vec2(200, 400));
    label->Update();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, g_width, g_height);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(programId);
    // ゲームループ
    while (!glfwWindowShouldClose(window)) {
        assert(glGetError() == GL_NO_ERROR);
        // 画面の初期化
        glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearDepth(1.0);
        
        /*
        // 頂点データ
        float vertex_position[] = {
            //0.5f, 0.5f,
            //-0.5f, 0.5f,
            //-0.5f, -0.5f,
            //0.5f, -0.5f
            1.0f, 1.0f,
            -1.0f, 1.0f,
            -1.0f, -1.0f,
            1.0f, -1.0f
        };
        
        const GLfloat vertex_uv[] = {
            1, 0,
            0, 0,
            0, 1,
            1, 1,
        };
        
        // 何番目のattribute変数か
        int positionLocation = glGetAttribLocation(programId, "position");
        int uvLocation = glGetAttribLocation(programId, "uv");
        int textureLocation = glGetUniformLocation(programId, "texture");
        
        // attribute属性を有効にする
        glEnableVertexAttribArray(positionLocation);
        glEnableVertexAttribArray(uvLocation);
        
        // uniform属性を設定する
        glUniform1i(textureLocation, 0);

        // attribute属性を登録
        glVertexAttribPointer(positionLocation, 2, GL_FLOAT, false, 0, vertex_position);
        glVertexAttribPointer(uvLocation, 2, GL_FLOAT, false, 0, vertex_uv);
        
    
        // モデルの描画
        glBindTexture(GL_TEXTURE_2D, FontManager::GetInstance()->GetFontTextureId());
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        */
        //==============================================================        // 頂点データ
        /*
        float vertex_position[] = {
            //0.5f, 0.5f,
            //-0.5f, 0.5f,
            //-0.5f, -0.5f,
            //0.5f, -0.5f
            1.0f, 1.0f,
            -1.0f, 1.0f,
            -1.0f, -1.0f,
            1.0f, -1.0f
        };

        const GLfloat vertex_uv[] = {
            1, 0,
            0, 0,
            0, 1,
            1, 1,
        };

        // 何番目のattribute変数か
        int positionLocation = glGetAttribLocation(programId, "position");
        int uvLocation = glGetAttribLocation(programId, "uv");
        int textureLocation = glGetUniformLocation(programId, "texture");
        
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
        glm::mat4 Projection = glm::perspective(45.0f, (float)g_width / (float)g_height, 0.1f, 100.0f);
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
        

        // attribute属性を有効にする
        glEnableVertexAttribArray(positionLocation);
        glEnableVertexAttribArray(uvLocation);

        // uniform属性を設定する
        glUniform1i(textureLocation, 0);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        // attribute属性を登録
        glVertexAttribPointer(positionLocation, 2, GL_FLOAT, false, 0, vertex_position);
        glVertexAttribPointer(uvLocation, 2, GL_FLOAT, false, 0, vertex_uv);


        // モデルの描画
        glBindTexture(GL_TEXTURE_2D, FontManager::GetInstance()->GetFontTextureId());
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        */
        label->Draw(programId);

        //===============================================================
        
        // バッファの入れ替え
        glfwSwapBuffers(window);
        
        // イベント待ち
        glfwPollEvents();
    }
    
    FontManager::GetInstance()->Finalize();
    glfwTerminate();
    
    return 0;
}
