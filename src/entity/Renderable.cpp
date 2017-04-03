#include "Renderable.h"
#include <iostream>

Renderable::Renderable() {  }

Renderable::Renderable(std::string model_fname, std::string tex_fname)
{
    models = Model::load_multimesh_models(model_fname, tex_fname);

    if (models.size() > 0) {
        model_loaded = true;
    }
    scaleModels();
}

Renderable::Renderable(std::vector<Model*>& mods) {
    // reuse an existing model
    models = mods;
    if (models.size() > 0) {
        model_loaded = true;
        renderable = true;
        copied = true;
    }
}

Renderable::~Renderable() {
    if (!copied) {
        for (Model* m : models) {
            delete m;
        }
        models.clear();
    }
}

void Renderable::scaleModels() {
    for (auto mptr : models) {
        mptr->x_norm = X_MODEL_SCALE;
        mptr->y_norm = Y_MODEL_SCALE;
        mptr->z_norm = Z_MODEL_SCALE;
        mptr->sil_x = SIL_X_SCALE;
        mptr->sil_y = SIL_Y_SCALE;
        mptr->sil_z = SIL_Z_SCALE;
        mptr->sil_jitter =  SIL_JITTER;
    }
}

bool Renderable::canRender() { return renderable; }

std::vector<Model*>& Renderable::getModels()
{
    return models;
}

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

void Renderable::setSil(bool sil) {
    for (auto &m : models) {
        m->drawSil(sil);
    }
}