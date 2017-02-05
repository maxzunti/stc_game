#include "Renderable.h"
#include <iostream>

Renderable::Renderable() { }

Renderable::Renderable(std::string model_fname, std::string tex_fname) :
    model(new Model(model_fname, tex_fname))
{
    if (model)
        model_loaded = true;
}

bool Renderable::canRender() { return true; }

const Model * Renderable::getModel() const
{
    return model.get();
}

bool Renderable::is_model_loaded() const { return model_loaded;  }

void Renderable::scale(double x, double y, double z) {
    model->scale(x, y, z);
}
