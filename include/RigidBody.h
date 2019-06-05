#ifndef __RIGID_BODY_H__
#define __RIGID_BODY_H__

#include "Vec2.h"

namespace hockey
{

class RigidBody
{
protected:
	Vec2f m_pos {};
	Vec2f m_dir {};
	float m_size = 0.0f;
	float m_force = 0.0f;

public:
	Vec2f& pos() { return m_pos; }
	Vec2f& dir() { return m_dir; }
	float& size() { return m_size; }
	float& force() { return m_force; }

	bool process_collision(RigidBody& that);
	void apply_force();
};

}

#endif  //  !__RIGID_BODY_H__
