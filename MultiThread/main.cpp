#include <cstdio>
#include <vector>

#include "Profiler.h"
#include "Core/CS/CSManager.h"
#include "Core/ECS/Test.h"
#include "Core/ECS/World.h"

constexpr std::size_t TEST_NUM = 1000000;

int main(void) {
    Profiler m_Profiler;
    
    // CS�݌v
    {
        //=== ������
        std::vector<CS::GameObject> vCSObjects;
        vCSObjects.reserve(TEST_NUM);
        for (std::size_t i = 0; i < TEST_NUM; i++) {
            // �I�u�W�F�N�g�ǉ�
            vCSObjects.emplace_back();
            // �R���|�[�l���g�ǉ�
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

        //=== �����v��
        std::cout << "�y��{�I�ȃR���|�[�l���g�w���z\n";

        for (int i = 0; i < 5; i++)
        {
            m_Profiler.ProfileStart(); // �v���J�n
            for (auto&& obj : vCSObjects)
            {
                obj.Update();
            }
            m_Profiler.OutputElapsedTime(); // �o�ߎ��ԏo��
        }
    }

    //=== ECS�݌v
    {
        //=== ������
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
            // �I�u�W�F�N�g�ǉ�
            auto entity = manager->CreateEntity(archetype);
            // ������
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

        //=== �����v��
        std::cout << "�y�`�����N�^ECS�݌v�z        \n";

        for (int i = 0; i < 5; i++)
        {
            m_Profiler.ProfileStart(); // �v���J�n
            world.Update(NULL);
            m_Profiler.OutputElapsedTime(); // �o�ߎ��ԏo��
        }
    }

    return 0;
}