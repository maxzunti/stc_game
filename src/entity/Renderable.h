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
    std::vector<Model*> models;
    bool model_loaded = false;
    bool renderable = true;

public:
    Renderable();
    ~Renderable();
    Renderable(std::string model_fname, std::string tex_fname);

    virtual bool canRender();
    std::vector<Model*>& getModels();
    void setRenderable(bool);
    void scale(double x, double y, double z);
    void reset_scale();
    void scaleModels();

    // Model-scaling normalization factors
    float X_MODEL_SCALE = 1.;
    float Y_MODEL_SCALE = 1.;
    float Z_MODEL_SCALE = 1.;

    // Allows silhouette thickness to be tweaked per-renderable
    float SIL_X_SCALE = Model::DEFAULT_SIL;
    float SIL_Y_SCALE = Model::DEFAULT_SIL;
    float SIL_Z_SCALE = Model::DEFAULT_SIL;
    float SIL_JITTER = 0.0;
    void setSil(bool);
};