#ifndef ENTITY_H
#define ENTITY_H

// should treat this as an abstract class (i.e. never instantiate)
class Entity {
protected:
	double pos[3] = { 0, 0, 0 }; // x, y, z
	double rot[3] = { 0, 0, 0 }; // rotation about x, y, z axes (degrees or rad? dep. on physX)

public:

	Entity();
	//~Entity(); currently have no specific behaviour

	void getPos(double buf[3]);
	void setPos(double buf[3]);
	double xPos();
	double yPos();
	double zPos();
	double xRot();
	double yRot();
	double zRot();
    virtual bool canRender();
};
#endif ENTITY_H