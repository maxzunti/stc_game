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
	void setPos(double x, double y, double z);
    const glm::vec3& getRot() const;
    void setRot(double x, double y, double z);
	double xPos();
	double yPos();
	double zPos();
	double xRot();
	double yRot();
	double zRot();
    virtual bool canRender();
};
#endif ENTITY_H