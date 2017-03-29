#include "Skyline.h"
#include <iostream>

using namespace std;

Skyline::Skyline(int mmSize, GLuint &mm_frameBuffer, const std::vector<Entity*>& ents) {
    GLubyte* data = (GLubyte*)malloc(mmSize * mmSize * 4);
    if (data) {
        glBindFramebuffer(GL_FRAMEBUFFER, mm_frameBuffer);
        glReadPixels(0, 0, mmSize, mmSize, GL_UNSIGNED_BYTE, GL_RGBA, data);
        for (int i = 0; i < mmSize; i + 4) {
            for (int j = 0; j < mmSize; j + 4) {
                GLubyte pixel = *(data + i + j);
                if (pixel == 0) {
                    cout << "zero!" << endl;
                }
                else {
                    cout << "nonzero!" << endl;
                }
            }
        }
    }
    else {
        cout << "Error: unable to allocate room for Skyline gen" << endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
