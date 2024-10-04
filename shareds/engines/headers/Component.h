#pragma once
#include "stdafx.h"
#include "EObject.h"

namespace dxe
{
	class Component : public EObject
	{
	public:
		Component();
		virtual ~Component();
		Component(const Component& component);
		Component(Component&& component) noexcept;
		Component& operator=(const Component& component);
		Component& operator=(Component&& component) noexcept;
		bool operator<(const Component& other) const;
		bool operator==(const Component& other) const;

	public:
		void* Clone() const override;
		void ReRef() const override;

		std::weak_ptr<GameObject> gameObject;

		//Init <- 이건 Component가 GameObject에 처음 소속될때.
		// 
		//Start 첫프레임
		//Update
		//LastUpdate
		//OnEnable
		//OnDisable
		//OnDestroy

		//Remove Cycle

		//BeforeRendering
		//Rendering
		//AfterRendering

		//------------------
		//OnCollision(GameObject& other) // 아니면 콜리전으로

		//------------------
		//gameobject weak
		//transform

	};
}


