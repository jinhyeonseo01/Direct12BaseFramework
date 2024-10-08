#pragma once
#include "stdafx.h"
#include "EObject.h"
#include "IDelayedDestroy.h"

namespace dxe
{
	class Component : public EObject, public IDelayedDestroy
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
		void* Clone() override;
		void ReRef() override;

		std::weak_ptr<GameObject> gameObject;

	protected:
		bool _first = true;
	public:
		bool IsFirst();
		void FirstDisable();
		void Destroy() override;

		//Init <- 이건 Component가 GameObject에 처음 소속될때.
		// 
		virtual void Init();
		virtual void Start();
		virtual void Update();
		virtual void LateUpdate();
		virtual void OnEnable();
		virtual void OnDisable();
		virtual void OnDestroy();
		virtual void OnComponentDestroy();

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


