#ifndef ENTITY_H
#define ENTITY_H
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// should treat this as an abstract class (i.e. never instantiate)
class Entity {
protected:
	glm::vec3 pos = { 0, 0, 0 }; // x, y, z
    glm::quat qrot;
    glm::vec3 dir; // forward-facing direction

public:

	Entity();
	//~Entity(); currently have no specific behaviour

	const glm::vec3& getPos() const;
    glm::quat getQRot() const;
    glm::vec3 getDir() const;

    virtual void setPos(double x, double y, double z);
    virtual void setPos(glm::vec3 newPos);
    virtual void setRot(double x, double y, double z);
    virtual void setRot(glm::vec3 newRot);

	double xPos() const;
	double yPos() const;
	double zPos() const;

    virtual bool canRender();
};
#endif ENTITY_H