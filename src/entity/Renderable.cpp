#include "Renderable.h"
#include <iostream>

Renderable::Renderable() { }

Renderable::Renderable(std::string model_fname, std::string tex_fname)
{
    //models.push_back(new Model(model_fname, tex_fname));
    std::cout << "loading a model: " << std::endl;
    models = Model::load_multimesh_models(model_fname, tex_fname);
    std::cout << "loading " << model_fname << ":  size = " << models.size() << std::endl;

    if (models.size() > 0)
        model_loaded = true;
}

Renderable::~Renderable() {
    for (Model* m : models) {
        delete m;
    }
    models.clear();
}


bool Renderable::canRender() { return renderable; }

std::vector<Model*>& Renderable::getModels()
{
    return models;
}

// bool Renderable::is_model_loaded() const { return model_loaded;  }

void Renderable::scale(double x, double y, double z) {
    for (Model* m : models) {
        m->scale(x, y, z);
    }
}

void Renderable::reset_scale() {
    for (Model* m : models) {
        m->reset_scale();
    }
}


void Renderable::setRenderable(bool val) { renderable = val; }
