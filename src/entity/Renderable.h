#pragma once
#include "Entity.h"

// Ideally, also abstract, but currently being used to test VBO initialization
class Renderable : public Entity {
protected:

public:
	static const bool renderable = true;
};