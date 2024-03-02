#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <array>

namespace CS
{
	class GameObject;
	
	class IComponent
	{
	public:
		virtual void Update() {}
		void SetOwner(GameObject* _inObjectPtr)
		{
			m_OwnerPtr = _inObjectPtr;
		}
	protected:
		GameObject* m_OwnerPtr = nullptr;
	};

	class GameObject
	{
	public:
		template <typename CompType>
		std::weak_ptr<CompType> AddComponent()
		{
			std::shared_ptr<CompType> newComp = std::make_shared<CompType>();
			m_umComponents[typeid(CompType)] = newComp;
			newComp->SetOwner(this);
			return newComp;
		}

		template <typename CompType>
		std::weak_ptr<CompType> GetComponent()
		{
			return std::static_pointer_cast<CompType>(m_umComponents[typeid(CompType)]);
		}

		void Update()
		{
			for (auto&& spComp : m_umComponents)
			{
				spComp.second->Update();
			}
		}

	private:
		std::unordered_map<std::type_index, std::shared_ptr<IComponent>> m_umComponents;
	};

	namespace Test
	{
		class TestComponent0 : public IComponent {
		public:
			int value = 0;
		};
		class TestComponent1 : public IComponent {
		public:
			int value = 0;
		};
		class TestComponent2 : public IComponent {
		public:
			int value = 0;
		};
		class TestComponent3 : public IComponent {
		public:
			int value = 0;
		};
		class TestComponent4 : public IComponent {
		public:
			int value = 0;
		};
		class TestComponent5 : public IComponent {
		public:
			int value = 0;
		};
		class TestComponent6 : public IComponent {
		public:
			int value = 0;
		};
		class TestComponent7 : public IComponent {
		public:
			int value = 0;
		};
		class TestComponent8 : public IComponent {
		public:
			int value = 0;
		};
		class TestComponent9 : public IComponent {
		public:
			int value = 0;
		};

		class TestComponent10 : public IComponent {
		public:
			void Update() override
			{
				// Žæ“¾
				TestComponent0* pTest0 = m_OwnerPtr->GetComponent<TestComponent0>().lock().get();
				TestComponent1* pTest1 = m_OwnerPtr->GetComponent<TestComponent1>().lock().get();
				TestComponent2* pTest2 = m_OwnerPtr->GetComponent<TestComponent2>().lock().get();
				TestComponent3* pTest3 = m_OwnerPtr->GetComponent<TestComponent3>().lock().get();
				TestComponent4* pTest4 = m_OwnerPtr->GetComponent<TestComponent4>().lock().get();
				TestComponent5* pTest5 = m_OwnerPtr->GetComponent<TestComponent5>().lock().get();
				TestComponent6* pTest6 = m_OwnerPtr->GetComponent<TestComponent6>().lock().get();
				TestComponent7* pTest7 = m_OwnerPtr->GetComponent<TestComponent7>().lock().get();
				TestComponent8* pTest8 = m_OwnerPtr->GetComponent<TestComponent8>().lock().get();
				TestComponent9* pTest9 = m_OwnerPtr->GetComponent<TestComponent9>().lock().get();
			
				// ˆ—
				this->value =
					pTest0->value +
					pTest1->value +
					pTest2->value +
					pTest3->value +
					pTest4->value +
					pTest5->value +
					pTest6->value +
					pTest7->value +
					pTest8->value +
					pTest9->value;
			
			}
			int value = 0;
		};
	}
}