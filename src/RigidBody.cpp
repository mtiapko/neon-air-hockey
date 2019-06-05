#include "RigidBody.h"

namespace hockey
{

bool RigidBody::process_collision(RigidBody& that)
{
	float dist = m_pos.distance(that.m_pos);
	float min_dist = m_size + that.m_size;
	float delta = min_dist - dist;

	if (delta <= 0.0f)
		return false;

	auto norm = that.m_pos - m_pos;
	norm.normalize();

	m_force = std::min(m_force, delta);
	m_pos -= m_dir * m_force;

	float dot = that.m_dir.dot(norm);
	that.m_dir = that.m_dir * that.m_force + norm * delta + m_dir * m_force * dot;
	that.m_force = that.m_dir.normalize();
	return true;
}

void RigidBody::apply_force()
{
	m_pos += m_dir * m_force;
}

}
