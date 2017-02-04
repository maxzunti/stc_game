#include "Renderable.h"
#include <iostream>

Renderable::Renderable() { }

Renderable::Renderable(std::string filename) :
    model(new Model(filename)) 
{
    model_loaded = true;
}

const Model * Renderable::getModel() const
{
    return model.get();
}

bool Renderable::canRender() { return true; }

bool Renderable::is_model_loaded() const { return model_loaded;  }