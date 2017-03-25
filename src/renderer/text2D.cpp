// Adapted/Inspired by http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-11-2d-text/
#include "model\Texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "text2D.h"
//#include "GLUtil.h"
#include <iostream>

using namespace glm;
using namespace std;

Text2D::Text2D(const char * texturePath) {

    // Initialize texture "assets/textures/font.png"
    fontTexture = new Texture(texturePath);
    Text2DTextureID = fontTexture->getID();

    // Initialize VBO
    glGenBuffers(1, &Text2DVertexBufferID);
    glGenBuffers(1, &Text2DUVBufferID);

    // Initialize uniforms' IDs
    // MAX: Are we still using these?
    Text2DShaderID = shader[SHADER::TEXT];
    Text2DUniformID = glGetUniformLocation(shader[SHADER::TEXT], "textTextureSample");

    glGenBuffers(TEXT_VBO::COUNT, textVbo);
    glGenVertexArrays(TEXT_VAO::COUNT, textVao);
    Text2D::initVAO(textVao, textVbo);
}

Text2D::Text2D(Texture * tex) {
    // Initialize texture "assets/textures/font.png"
    fontTexture = tex;
    Text2DTextureID = fontTexture->getID();

    // Initialize VBO
    glGenBuffers(1, &Text2DVertexBufferID);
    glGenBuffers(1, &Text2DUVBufferID);

    // Initialize uniforms' IDs
    Text2DShaderID = shader[SHADER::TEXT];
    Text2DUniformID = glGetUniformLocation(shader[SHADER::TEXT], "textTextureSample");

    glGenBuffers(TEXT_VBO::COUNT, textVbo);
    glGenVertexArrays(TEXT_VAO::COUNT, textVao);
    Text2D::initVAO(textVao, textVbo);
}

Text2D::~Text2D() { }


void Text2D::printText2D(const char * text, int x, int y, int size, int width, int height, float alpha) {

    glBindVertexArray(textVao[TEXT_VAO::GEOMETRY]);
    glDisable(GL_STENCIL_TEST);

    unsigned int length = strlen(text);
    float kern_factor = 0.5; // space_reduction

    // Fill buffers
    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> UVs;
    int j = 0;
    for (unsigned int i = 0; i < length; i++) {
        j++;
        if (text[i] == '\n') { 
            j = 0;
            y -= size;
        }
        glm::vec2 vertex_up_left = glm::vec2(x + (j*kern_factor)*size, y + size);
        glm::vec2 vertex_up_right = glm::vec2(x + (j*kern_factor)*size + size, y + size);
        glm::vec2 vertex_down_right = glm::vec2(x + (j*kern_factor)*size + size, y);
        glm::vec2 vertex_down_left = glm::vec2(x + (j*kern_factor)*size, y);

        vertices.push_back(vertex_up_left);
        vertices.push_back(vertex_down_left);
        vertices.push_back(vertex_up_right);

        vertices.push_back(vertex_down_right);
        vertices.push_back(vertex_up_right);
        vertices.push_back(vertex_down_left);

        char character = text[i];
        float uv_x = (character % 16) / 16.0f;
        float uv_y = (character / 16) / 16.0f;

        glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
        glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 16.0f, uv_y);
        glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 16.0f));
        glm::vec2 uv_down_left = glm::vec2(uv_x, (uv_y + 1.0f / 16.0f));
        UVs.push_back(uv_up_left);
        UVs.push_back(uv_down_left);
        UVs.push_back(uv_up_right);

        UVs.push_back(uv_down_right);
        UVs.push_back(uv_up_right);
        UVs.push_back(uv_down_left);
    }

    // Bind shader
    glUseProgram(shader[SHADER::TEXT]);

    loadBuffer(textVbo, vertices, UVs);

    glUniform1i(glGetUniformLocation(shader[SHADER::TEXT], "height"), height);
    glUniform1i(glGetUniformLocation(shader[SHADER::TEXT], "width"), width);
    glUniform1f(glGetUniformLocation(shader[SHADER::TEXT], "alphaMod"), alpha);

    //Bind texture
    fontTexture->load(GL_TEXTURE0, shader[SHADER::TEXT], "textTextureSample");
   
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glDrawArrays(
        GL_TRIANGLES,		//What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
        0,                  // First position
        vertices.size()		//how many points
    );

    glDisable(GL_BLEND);

    CheckGLErrors("text render");
}

void Text2D::drawTexture(int x, int y, int width, int height, int sWidth, int sHeight, float alpha, bool flipped) {
    glBindVertexArray(textVao[TEXT_VAO::GEOMETRY]);
    glDisable(GL_STENCIL_TEST);

    // Fill buffers
    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> UVs;

    glm::vec2 vertex_up_right;
    glm::vec2 vertex_up_left;
    glm::vec2 vertex_down_left;
    glm::vec2 vertex_down_right;

    if (flipped) {
        // Actually flipped horizontally
        vertex_up_left = glm::vec2(x + width, y);
        vertex_up_right = glm::vec2(x, y);
        vertex_down_right = glm::vec2(x, y + height);
        vertex_down_left = glm::vec2(x + width, y + height);
    }
    else {
        vertex_up_right = glm::vec2(x + width, y);
        vertex_up_left = glm::vec2(x, y);
        vertex_down_left = glm::vec2(x, y + height);
        vertex_down_right = glm::vec2(x + width, y + height);
    }

    vertices.push_back(vertex_up_left);
    vertices.push_back(vertex_down_left);
    vertices.push_back(vertex_up_right);

    vertices.push_back(vertex_down_right);
    vertices.push_back(vertex_up_right);
    vertices.push_back(vertex_down_left);

    UVs.push_back(glm::vec2(0, 1));
    UVs.push_back(glm::vec2(0, 0));
    UVs.push_back(glm::vec2(1, 1));

    UVs.push_back(glm::vec2(1, 0));
    UVs.push_back(glm::vec2(1, 1));
    UVs.push_back(glm::vec2(0, 0));

    // Bind shader
    glUseProgram(shader[SHADER::TEXT]);

    loadBuffer(textVbo, vertices, UVs);

    glUniform1i(glGetUniformLocation(shader[SHADER::TEXT], "height"), sHeight);
    glUniform1i(glGetUniformLocation(shader[SHADER::TEXT], "width"), sWidth);
    glUniform1f(glGetUniformLocation(shader[SHADER::TEXT], "alphaMod"), alpha);

    //Bind texture
    fontTexture->load(GL_TEXTURE0, shader[SHADER::TEXT], "textTextureSample");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glDrawArrays(
        GL_TRIANGLES,		//What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
        0,                  // First position
        vertices.size()		//how many points
    );

    glDisable(GL_BLEND);

    CheckGLErrors("Text2D raw texture render");
}

//Describe the setup of the Vertex Array Object
bool Text2D::initVAO(GLuint vao[], GLuint vbo[])
{
    glBindVertexArray(vao[TEXT_VAO::GEOMETRY]);		//Set the active Vertex Array (should only have 1, right?)

    glEnableVertexAttribArray(0);		//Tell opengl you're using layout attribute 0 (For shader input)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXT_VBO::POINTS]);		//Set the active Vertex Buffer
    glVertexAttribPointer(
        0,				//Attribute
        2,				//Size # Components
        GL_FLOAT,	//Type
        GL_FALSE, 	//Normalized?
        sizeof(vec2),	//Stride
        (void*)0			//Offset
        );

    glEnableVertexAttribArray(1);       //Tell opengl you're using layout attribute 2
    glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXT_VBO::UVS]);
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(vec2),
        (void*)0
        );

    return !CheckGLErrors("initTextVAO text2D.cpp");		//Check for errors in initialize
}

//Loads buffers with data
bool Text2D::loadBuffer(GLuint vbo[], vector<glm::vec2>& points, vector<glm::vec2>& uvs)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXT_VBO::POINTS]);
    glBufferData(
        GL_ARRAY_BUFFER,				//Which buffer you're loading too
        sizeof(vec2)*points.size(),	//Size of data in array (in bytes)
        &points[0],							//Start of array (&points[0] will give you pointer to start of vector)
        GL_STATIC_DRAW						//GL_DYNAMIC_DRAW if you're changing the data often
                                            //GL_STATIC_DRAW if you're changing seldomly
        );

    glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXT_VBO::UVS]);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vec2)*uvs.size(),
        &uvs[0],
        GL_STATIC_DRAW
        );

    return !CheckGLErrors("loadTextBuffer");
}