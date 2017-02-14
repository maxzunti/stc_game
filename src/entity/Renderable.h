#pragma once
#include "Entity.h"
#include "../renderer/GLUtil.h"
#include "../renderer/model/Model.h"
#include "../renderer/model/Texture.h"
#include <memory>

#define M_PI 3.14159265358979323846

// Ideally, also abstract, but currently being used to test VBO initialization
class Renderable : public Entity {
protected:
    std::unique_ptr<Model> model;
    bool model_loaded = false;
    bool renderable = true;

public:
    Renderable();
    Renderable(std::string model_fname, std::string tex_fname);

    virtual bool canRender();
    Model * getModel();
    bool is_model_loaded() const;
    void scale(double x, double y, double z);
};