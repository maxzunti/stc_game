#include "Skybox.h"
#include <iostream>

GLfloat skyboxVertices[] = {
    // Positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
};

Skybox::Skybox(std::string files[6]) {
    glGenTextures(1, &tex_id);
    glActiveTexture(GL_TEXTURE0);
    int components, tWidth, tHeight;

    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);
    for (int i = 0; i < 6; i++) {
        face_names[i] = files[i];
        unsigned char* data = stbi_load(face_names[i].c_str(), &tWidth, &tHeight, &components, 0);

        if (data != NULL)
        {
            if (components == 3)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, tWidth, tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            else if (components == 4)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, tWidth, tHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Error loading texture from " << face_names[i] << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Clean up
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    initSkyboxVAO();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,				//Which buffer you're loading too
        sizeof(GLfloat)*3*36,	//Size of data in array (in bytes)
        &skyboxVertices[0],							//Start of array (&points[0] will give you pointer to start of vector)
        GL_STATIC_DRAW						//GL_DYNAMIC_DRAW if you're changing the data often
                                            //GL_STATIC_DRAW if you're changing seldomly
        );
}

//Describe the setup of the Vertex Array Object
bool Skybox::initSkyboxVAO()
{
    glBindVertexArray(vao);		//Set the active Vertex Array (should only have 1, right?)

    glEnableVertexAttribArray(0);		//Tell opengl you're using layout attribute 0 (For shader input)
    glBindBuffer(GL_ARRAY_BUFFER, vbo);		//Set the active Vertex Buffer
    glVertexAttribPointer(
        0,				//Attribute
        3,				//Size # Components
        GL_FLOAT,	//Type
        GL_FALSE, 	//Normalized?
        3*sizeof(GLfloat),	//Stride
        (void*)0			//Offset
        );

    return !CheckGLErrors("initSkyboxVAO");		//Check for errors in initialize
}