#pragma once
#include "Entity.h"
#include "../renderer/GLUtil.h"
#include "../renderer/model/Model.h"
#include <memory>

// Ideally, also abstract, but currently being used to test VBO initialization
class Renderable : public Entity {
protected:
    bool model_loaded = false;
    std::unique_ptr<Model> model;

public:
    Renderable();
    Renderable(std::string filename); // loads a model from a given filename?

    virtual bool canRender();
    const Model * getModel() const;
    bool is_model_loaded() const;
};