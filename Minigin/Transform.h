#pragma once
#include <glm/glm.hpp>

namespace dae
{
	class GameObject;

	class Transform final
	{
	public:
		explicit Transform(GameObject* owner) : m_owner{ owner } {}

		const glm::vec3& GetPosition() const { return m_position; }
		void SetPosition(float x, float y, float z = 0);
		void SetPosition(const glm::vec3& position);
		glm::vec3 GetWorldPosition();
		void SetDirty();

	private:
		GameObject* m_owner;

		glm::vec3 m_position{};
		glm::vec3 m_worldPosition{};

		bool m_isDirty{ true };

		void UpdateWorldPosition();
	};
}
