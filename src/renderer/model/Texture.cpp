#include "Texture.h"
#include <iostream>

//For reference:
//	https://open.gl/textures
Texture::Texture(const char* filename)
{
    this->filename = filename;
    int components, Width, tHeight;

    //stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename, &tWidth, &tHeight, &components, 0);

    if (data != NULL)
    {
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);

        if (components == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tWidth, tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else if (components == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tWidth, tHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 7);
        glGenerateMipmap(GL_TEXTURE_2D); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f); // 4x filtering

        //Clean up
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
    } else {
        std::cout << "Error loading texture from " << filename << std::endl;
    }
}

Texture::Texture(GLuint& ID) {
    filename = "none";
    texID = ID;
}

Texture::~Texture() {
    // MAX: Removing delete since we can now pass an already-created texture GLuint to a 'Texture' constrctor
    // glDeleteTextures(1, &texID);
}

// Use program before loading texture
// texUnit can be - GL_TEXTURE0, GL_TEXTURE1, etc...
bool Texture::load(GLuint texUnit, GLuint program, const char* uniformName) const
{
    glActiveTexture(texUnit);
    glBindTexture(GL_TEXTURE_2D, texID);

    GLuint uniformLocation = glGetUniformLocation(program, uniformName);
    glUniform1i(uniformLocation, 0);

    return !CheckGLErrors("load");
}

const std::string Texture::getName() & {
    return filename;
}

GLuint Texture::getID() const {
    return texID;
}