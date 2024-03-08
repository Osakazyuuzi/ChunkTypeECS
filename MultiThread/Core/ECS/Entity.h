#pragma once

#include "Common/Id.h"

namespace ECS
{
	/**
	* @struct Entity
	* @brief ECSアーキテクチャにおけるエンティティを表す構造体。
	*/
	struct Entity
	{
		/**
		* @brief コンストラクタ
		* @param _index エンティティのインデックス。
		* @param _version エンティティのバージョン
		*/
		Entity(const std::uint32_t _index, const std::uint32_t _version)
			: m_Identifier(NULL)
		{
			SetIndex(m_Identifier, _index);
			SetVersion(m_Identifier, _version);
		}

		/**
		* @brief コンストラクタ
		* @param _Identifier エンティティの識別ビット。
		*/
		Entity(const EntityIdentifier _Identifier)
			: m_Identifier(_Identifier)
		{}

		//! エンティティの一意の識別ビット。
		EntityIdentifier m_Identifier;
	};
}