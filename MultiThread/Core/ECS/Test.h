#pragma once

#include "IComponentData.h"
#include "SystemBase.h"

namespace ECS
{
	namespace Test
	{
		struct Component0 : public IComponentData {
		public:
			int value = 0;
		};
		struct Component1 : public IComponentData {
		public:
			int value = 1;
		};
		struct Component2 : public IComponentData {
		public:
			int value = 2;
		};
		struct Component3 : public IComponentData {
		public:
			int value = 3;
		};
		struct Component4 : public IComponentData {
		public:
			int value = 4;
		};
		struct Component5 : public IComponentData {
		public:
			int value = 5;
		};
		struct Component6 : public IComponentData {
		public:
			int value = 6;
		};
		struct Component7 : public IComponentData {
		public:
			int value = 7;
		};
		struct Component8 : public IComponentData {
		public:
			int value = 8;
		};
		struct Component9 : public IComponentData {
		public:
			int value = 9;
		};
		struct Component10 : public IComponentData {
		public:
			int value = 0;
		};

		class TestSystem : public SystemBase {
		public:
			/**
			* @brief コンストラクタ。
			* @param _pWorld ワールドポインタ。
			* @param _Id 一意のID。
			*/
			TestSystem(World* _pWorld, std::size_t _Id)
				: SystemBase(_pWorld, _Id)
			{}

			/**
			* @brief 初期化処理をします。
			*/
			void Init() override
			{
				// アーキタイプに必要なコンポーネントを設定する。
				ArchetypeSetting();
			}

			/**
			* @brief 更新処理をします。
			*/
			void Update(float _deltaTime) override
			{
				this->ExecuteForEntitiesMatching
					<Component0,
					Component1,
					Component2,
					Component3,
					Component4,
					Component5,
					Component6,
					Component7,
					Component8,
					Component9,
					Component10>
					([this]
					(
						Component0& _comp0,
						Component1& _comp1,
						Component2& _comp2,
						Component3& _comp3,
						Component4& _comp4,
						Component5& _comp5,
						Component6& _comp6,
						Component7& _comp7,
						Component8& _comp8,
						Component9& _comp9,
						Component10& _comp10
						) {
							// 処理
							_comp10.value =
								_comp0.value +
								_comp1.value +
								_comp2.value +
								_comp3.value +
								_comp4.value +
								_comp5.value +
								_comp6.value +
								_comp7.value +
								_comp8.value +
								_comp9.value;
						});
			}
		private:
			/**
			* @brief アーキタイプに必要なコンポーネントを設定します。
			*/
			void ArchetypeSetting()
			{
				m_Archetype.AddType<Component0>();
				m_Archetype.AddType<Component1>();
				m_Archetype.AddType<Component2>();
				m_Archetype.AddType<Component3>();
				m_Archetype.AddType<Component4>();
				m_Archetype.AddType<Component5>();
				m_Archetype.AddType<Component6>();
				m_Archetype.AddType<Component7>();
				m_Archetype.AddType<Component8>();
				m_Archetype.AddType<Component9>();
				m_Archetype.AddType<Component10>();
			}
		};
	}
}