#include "GameObject.h"
#include "Components.h"

dae::GameObject::~GameObject()
{
	SetParent(nullptr);

	for (GameObject* child : m_children)
	{
		child->m_parent = nullptr;
	}
}

void dae::GameObject::SetParent(GameObject* parent, bool worldPositionStays)
{
	if (parent == this || m_parent == parent || (parent && IsChild(parent)))
		return;

	glm::vec3 worldPos = GetWorldPosition();

	if (m_parent)
	{
		auto& otherChildern = m_parent->m_children;
		otherChildern.erase(std::remove(otherChildern.begin(), otherChildern.end(), this), otherChildern.end());
	}

	m_parent = parent;

	if (m_parent)
	{
		m_parent->m_children.emplace_back(this);
	}

	if (worldPositionStays)
	{
		if (m_parent)
		{
			glm::vec3 newLocal = worldPos - m_parent->GetWorldPosition();
			m_transform.SetPosition(newLocal);
		}
		else
		{
			m_transform.SetPosition(worldPos);
		}
	}

	SetDirty();
}

bool dae::GameObject::IsChild(GameObject* obj) const
{
	for (GameObject* child : m_children)
	{
		if (child == obj)
			return true;

		if (child->IsChild(obj))
			return true;
	}

	return false;
}

void dae::GameObject::AddChild(GameObject* child, bool worldPositionStays)
{
	if (!child || child == this || child->IsChild(this) || child->m_parent == this)
		return;

	glm::vec3 worldPos = child->GetWorldPosition();

	if (child->m_parent)
	{
		auto& otherChildern = child->m_parent->m_children;
		otherChildern.erase(std::remove(otherChildern.begin(), otherChildern.end(), child), otherChildern.end());
	}

	child->m_parent = this;

	m_children.emplace_back(child);

	if (worldPositionStays)
	{
		glm::vec3 newLocal = worldPos - GetWorldPosition();
		child->m_transform.SetPosition(newLocal);
	}

	child->SetDirty();
}

void dae::GameObject::RemoveChild(GameObject* child, bool worldPositionStays)
{
	if (!child)
		return;

	glm::vec3 worldPos = child->GetWorldPosition();

	m_children.erase(std::remove(m_children.begin(), m_children.end(), child), m_children.end());

	child->m_parent = nullptr;

	if (worldPositionStays)
	{
		child->m_transform.SetPosition(worldPos);
	}

	child->SetDirty();
}

void dae::GameObject::Update(float delta_time)
{
	for (auto& component : m_components)
	{
		component->Update(delta_time);
	}
}

void dae::GameObject::Render() const
{
	for (auto& component : m_components)
	{
		component->Render();
	}
}

void dae::GameObject::SetPosition(float x, float y)
{
	m_transform.SetPosition(x, y, 0.0f);
	SetDirty();
}

glm::vec3 dae::GameObject::GetWorldPosition()
{
	if (m_isDirty)
		UpdateWorldPosition();

	return m_worldPosition;
}

void dae::GameObject::SetDirty()
{
	m_isDirty = true;

	for (GameObject* child : m_children)
		child->SetDirty();
}

void dae::GameObject::UpdateWorldPosition()
{
	if (m_parent == nullptr)
		m_worldPosition = m_transform.GetPosition();
	else
		m_worldPosition = m_parent->GetWorldPosition() + m_transform.GetPosition();

	m_isDirty = false;
}
