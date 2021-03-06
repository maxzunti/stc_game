#pragma once
#include "../GLUtil.h"

class Texture {
    GLuint texID;
    std::string filename;
    int tWidth, tHeight;

public:
    Texture(const char* filename);
    Texture(GLuint &ID); // reference an existing ID

    ~Texture();

    //Use program before loading texture
    //	texUnit can be - GL_TEXTURE0, GL_TEXTURE1, etc...
    bool load(GLuint texUnit, GLuint program, const char* uniformName) const;

    const std::string getName()&;
    GLuint getID() const;
};