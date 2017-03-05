#ifndef TEXT2D_H
#define TEXT2D_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cstring>

class Text2D {
public:
    struct TEXT_VAO {
        enum { GEOMETRY = 0, COUNT };		//Enumeration assigns each name a value going up (LINES=0, COUNT=1)
    };

    struct TEXT_VBO {
        enum { POINTS, UVS, COUNT };	//POINTS=0, UVS=1, COUNT=2
    };

    Text2D(const char * texturePath);
    ~Text2D();

    GLuint textVao[TEXT_VAO::COUNT];
    GLuint textVbo[TEXT_VBO::COUNT];
    GLuint Text2DTextureID;
    GLuint Text2DVertexBufferID;
    GLuint Text2DUVBufferID;
    GLuint Text2DShaderID;
    GLuint Text2DUniformID;
    Texture *fontTexture;

   // void initText2D(const char * texturePath);
    void printText2D(const char * text, int x, int y, int size);
    bool initVAO(GLuint vao[], GLuint vbo[]);
    bool loadBuffer(GLuint vbo[], std::vector<glm::vec2>& points, std::vector<glm::vec2>& uvs);
};
#endif