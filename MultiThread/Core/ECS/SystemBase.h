#pragma once

#include "Archetype.h"
#include "Chunk.h"
#include "World.h"
#include "EntityManager.h"

#include "../../AsyncFunctionManager.h"

namespace ECS
{
	/**
	* @class SystemBase
	* @brief システムベースクラス。全てのシステムはこれを継承する。
	*/
	class SystemBase
	{
	public:
		/**
		* @brief コンストラクタ。
		* @param _pWorld 属するワールドへのポインタ。
		* @param _Id システムを識別する一意のID。
		*/
		SystemBase(World* _pWorld, std::size_t _Id)
			: m_pWorld(_pWorld)
			, m_SystemId(_Id)
		{}

		/**
		* @brief 最初に一度のみ処理を実行します。
		*/
		virtual void Init() {}

		/**
		* @brief 処理を実行します。
		* @param _deltaTime 前フレームとの差[ms]
		*/
		virtual void Update(float _deltaTime) {}

		/**
		* @brief 描画を実行します。
		*/
		virtual void Draw() {}

		/**
		* @brief 実行するのに必要とするアーキタイプを取得します。
		* @return const Archetype& 実行するのに必要なアーキタイプ。
		*/
		inline const Archetype& GetArchetype() const noexcept
		{
			return m_Archetype;
		}

		/**
		* @brief 実行するのに必要とするアーキタイプを取得します。
		* @return Archetype* 実行するのに必要なアーキタイプ。
		*/
		inline Archetype* GetArchetype()
		{
			return &m_Archetype;
		}

		/**
		* @brief 属しているワールドのエンティティマネージャーを取得します。
		* @return std::shared_ptr<EntityManager> 属しているワールドのエンティティマネージャー。
		*/
		std::shared_ptr<EntityManager> GetEntityManager();

		/**
		* @brief システムを識別する一意のIDを取得します。
		* @return std::size_t 一意のID
		*/
		std::size_t GetSystemId() const noexcept
		{
			return m_SystemId;
		}

	protected:
		/**
		* @brief 必要なアーキタイプを含んでいるエンティティに関数を実行します。
		* @tparam Components コンポーネントの型のリスト。
		* @param _func 実行する関数。
		*/
		template <class... Components, typename Func>
		void ExecuteForEntitiesMatching(Func&& _func)
		{
			// アーキタイプが含まれているチャンクリストを取得
			auto pChunkList = m_pWorld->GetEntityManager()->GetContainChunkList(m_Archetype);

			for (auto&& pChunk : pChunkList)
			{
				auto func = std::forward<Func>(_func);


				// 必要なコンポーネント群を抜き出して、処理を実行
				ExecuteForEntitiesMatchingImpl(pChunk, func, pChunk->GetComponentList<Components>()...);
			}
		}

	private:
		/**
		* @brief 全ての該当エンティティに関数を実行します。
		* @param _pChunk 該当チャンク。
		* @param _func 実行する関数。
		* @param _args 関数に引き渡すコンポーネント群。
		*/
		template <typename Func, class... Args>
		static void ExecuteForEntitiesMatchingImpl(
			Chunk* _pChunk,
			Func&& _func,
			Args... _args)
		{
			for (std::uint32_t i = 0; i < _pChunk->GetSize(); ++i)
			{
				_func(_args[i]...);
			}
		}

	protected:
		//! 実行するのに必要とするアーキタイプ。
		Archetype m_Archetype;
	protected:
		//! システムID
		std::size_t m_SystemId;
	private:
		//! 属しているワールドへのポインタ。
		World* m_pWorld = nullptr;
	};
}