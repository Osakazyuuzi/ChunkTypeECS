#include <cstdio>
#include <vector>

#include "Profiler.h"
#include "Core/CS/CSManager.h"
#include "Core/ECS/Test.h"
#include "Core/ECS/World.h"

constexpr std::size_t TEST_NUM = 1000000;

int main(void) {
    Profiler m_Profiler;
    
    // CS設計
    {
        //=== 初期化
        std::vector<CS::GameObject> vCSObjects;
        vCSObjects.reserve(TEST_NUM);
        for (std::size_t i = 0; i < TEST_NUM; i++) {
            // オブジェクト追加
            vCSObjects.emplace_back();
            // コンポーネント追加
            vCSObjects.back().AddComponent<CS::Test::TestComponent0>();
            vCSObjects.back().AddComponent<CS::Test::TestComponent1>();
            vCSObjects.back().AddComponent<CS::Test::TestComponent2>();
            vCSObjects.back().AddComponent<CS::Test::TestComponent3>();
            vCSObjects.back().AddComponent<CS::Test::TestComponent4>();
            vCSObjects.back().AddComponent<CS::Test::TestComponent5>();
            vCSObjects.back().AddComponent<CS::Test::TestComponent6>();
            vCSObjects.back().AddComponent<CS::Test::TestComponent7>();
            vCSObjects.back().AddComponent<CS::Test::TestComponent8>();
            vCSObjects.back().AddComponent<CS::Test::TestComponent9>();
            vCSObjects.back().AddComponent<CS::Test::TestComponent10>();
        }

        //=== 処理計測
        std::cout << "【基本的なコンポーネント指向】\n";

        for (int i = 0; i < 5; i++)
        {
            m_Profiler.ProfileStart(); // 計測開始
            for (auto&& obj : vCSObjects)
            {
                obj.Update();
            }
            m_Profiler.OutputElapsedTime(); // 経過時間出力
        }
    }

    //=== ECS設計
    {
        //=== 初期化
        using namespace ECS;
        World world;
        auto manager = world.GetEntityManager();
        Archetype archetype;
        archetype.AddType<Test::Component0>();
        archetype.AddType<Test::Component1>();
        archetype.AddType<Test::Component2>();
        archetype.AddType<Test::Component3>();
        archetype.AddType<Test::Component4>();
        archetype.AddType<Test::Component5>();
        archetype.AddType<Test::Component6>();
        archetype.AddType<Test::Component7>();
        archetype.AddType<Test::Component8>();
        archetype.AddType<Test::Component9>();
        archetype.AddType<Test::Component10>();
        for (std::size_t i = 0; i < TEST_NUM; i++) {
            // オブジェクト追加
            auto entity = manager->CreateEntity(archetype);
            // 初期化
            manager->SetComponent(entity, Test::Component0());
            manager->SetComponent(entity, Test::Component1());
            manager->SetComponent(entity, Test::Component2());
            manager->SetComponent(entity, Test::Component3());
            manager->SetComponent(entity, Test::Component4());
            manager->SetComponent(entity, Test::Component5());
            manager->SetComponent(entity, Test::Component6());
            manager->SetComponent(entity, Test::Component7());
            manager->SetComponent(entity, Test::Component8());
            manager->SetComponent(entity, Test::Component9());
            manager->SetComponent(entity, Test::Component10());
        }

        world.AddSystem<Test::TestSystem>(0);

        //=== 処理計測
        std::cout << "【チャンク型ECS設計】        \n";

        for (int i = 0; i < 5; i++)
        {
            m_Profiler.ProfileStart(); // 計測開始
            world.Update(NULL);
            m_Profiler.OutputElapsedTime(); // 経過時間出力
        }
    }

    return 0;
}