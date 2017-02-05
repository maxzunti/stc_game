#pragma once
#include "Entity.h"
#include "../renderer/GLUtil.h"
#include "../renderer/model/Model.h"
#include "../renderer/model/Texture.h"
#include <memory>

// Ideally, also abstract, but currently being used to test VBO initialization
class Renderable : public Entity {
protected:
    std::unique_ptr<Model> model;
    bool model_loaded = false;

public:
    Renderable();
    Renderable(std::string model_fname, std::string tex_fname);

    virtual bool canRender();
    const Model * getModel() const;
    bool is_model_loaded() const;
    void scale(double x, double y, double z);
};