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

void dae::GameObject::SetParent(GameObject* parent, bool keepWorldPosition)
{
	if (IsChild(parent) || parent == this || m_parent == parent)
		return;

	if (parent == nullptr)
		m_transform.SetPosition(m_transform.GetWorldPosition());
	else
	{
		if (keepWorldPosition)
			m_transform.SetPosition(m_transform.GetWorldPosition() - parent->GetTransform().GetWorldPosition());

		m_transform.SetDirty();
	}

	if (m_parent)
		m_parent->RemoveChild(this);

	m_parent = parent;

	if (m_parent)
		m_parent->AddChild(this);
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
}
