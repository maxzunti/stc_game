#ifndef ENTITY_H
#define ENTITY_H
#include <glm/glm.hpp>

// should treat this as an abstract class (i.e. never instantiate)
class Entity {
protected:
	glm::vec3 pos = { 0, 0, 0 }; // x, y, z
    glm::vec3 rot = { 0, 0, 0 };; // rotation about x, y, z axes (degrees or rad? dep. on physX)

public:

	Entity();
	//~Entity(); currently have no specific behaviour

	const glm::vec3& getPos() const;
    virtual void setPos(double x, double y, double z);
    virtual void setPos(glm::vec3 &newPos);
    const glm::vec3& getRot() const;
    virtual void setRot(double x, double y, double z);
    virtual void setRot(glm::vec3 &newRot);
	double xPos() const;
	double yPos() const;
	double zPos() const;
	double xRot() const;
	double yRot() const;
	double zRot() const;
    virtual bool canRender();
};
#endif ENTITY_H