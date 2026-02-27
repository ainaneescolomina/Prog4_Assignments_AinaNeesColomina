#include "Transform.h"
#include "GameObject.h"

void dae::Transform::SetPosition(const float x, const float y, const float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;

	SetDirty();
}

void dae::Transform::SetPosition(const glm::vec3& position) 
{ 
	m_position = position; 
	SetDirty();

}

glm::vec3 dae::Transform::GetWorldPosition()
{
	if (m_isDirty)
		UpdateWorldPosition();

	return m_worldPosition;
}

void dae::Transform::SetDirty()
{
	m_isDirty = true;

	for (int i = 0; i < m_owner->GetChildCount(); ++i)
	{
		m_owner->GetChildAt(i)->GetTransform().SetDirty();
	}
}

void dae::Transform::UpdateWorldPosition()
{
    if (!m_isDirty)
        return;

    if (auto parent = m_owner->GetParent())
        m_worldPosition = parent->GetTransform().GetWorldPosition() + m_position;
    else
        m_worldPosition = m_position;

    m_isDirty = false;
}