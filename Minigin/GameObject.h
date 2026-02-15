#pragma once
#include <string>
#include <memory>
#include <vector>

#include "Transform.h"
#include "Components.h"

namespace dae
{
	class GameObject final
	{
		std::vector<std::unique_ptr<Component>> m_components{};

		Transform m_transform{};
		//std::shared_ptr<Texture2D> m_texture{};

	public:
		virtual void Update(float delta_time);
		virtual void Render() const;

		//void SetTexture(const std::string& filename);
		void SetPosition(float x, float y);

		GameObject() = default;
		virtual ~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		// Component Functions
		template<typename T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			auto component = std::make_unique<T>(this, std::forward<Args>(args)...);
			T* raw = component.get();
			m_components.push_back(std::move(component));
			return raw;
		}


		template<typename T>
		T* GetComponent()
		{
			for (auto& component : m_components)
			{
				if (auto castComponent = dynamic_cast<T*>(component.get()))
					return castComponent;
			}

			return nullptr;
		}

		template<typename T>
		void RemoveComponent()
		{
			m_components.erase(
				std::remove_if(m_components.begin(), m_components.end(),
					[](const std::unique_ptr<Component>& comp)
					{
						return dynamic_cast<T*>(comp.get()) != nullptr;
					}),
				m_components.end());
		}
	};

}
