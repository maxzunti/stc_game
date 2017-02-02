#pragma once
#include "Entity.h"
#include "../renderer/util/GLUtil.h"
#include "../renderer/util/Model.h"


// Ideally, also abstract, but currently being used to test VBO initialization
class Renderable : public Entity {
protected:
    static bool model_loaded;
    static Model model;

public:
	static const bool renderable = true;
    Renderable();
    // Renderable(std::string filename); // loads a model from a given filename?
};