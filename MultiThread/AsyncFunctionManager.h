#pragma once

#include <thread>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <stdexcept>
#include <vector>

/**
 * @class AsyncFunctionManager
 * @brief �񓯊��֐����Ǘ����A�����̃X���b�h�Ŏ��s����}�l�[�W���N���X�B
 */
class AsyncFunctionManager final {
public:
    /**
     * @brief �R���X�g���N�^�B
     * @param _inUseThreadNum �g�p����X���b�h���B�f�t�H���g�̓n�[�h�E�F�A�̃R�A���B
     * @throws std::invalid_argument �g�p����X���b�h����0�̏ꍇ�ɃX���[������O�B
     */
    inline AsyncFunctionManager(std::size_t _inUseThreadNum = std::thread::hardware_concurrency()) {
        // �X���b�h�̐��������[�v���񂵂āA�e�X���b�h��Run�֐������s����B
        for (std::size_t i = 0; i < _inUseThreadNum; ++i) {
            m_Threads.emplace_back([this] { this->Run(); });
        }
    }

    /**
     * @brief �e���v���[�g�����o�֐��B�񓯊��Ɋ֐������s����B
     * @param _inFunc ���s����֐��B
     * @param _inArgs ���s����֐��̈����B
     * @return �֐��̎��s���ʂ�ҋ@���邽�߂�future�I�u�W�F�N�g�B
     */
    template <typename Func, typename... Args>
    auto Execute(Func&& _inFunc, Args&&... _inArgs) 
        -> std::future<std::invoke_result_t<Func, Args...>> {
        using ReturnType = std::invoke_result_t<Func, Args...>;

        // �֐����p�b�P�[�W�^�X�N�ɕϊ����A��������L�|�C���^�Ń��b�v����B
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<Func>(_inFunc), std::forward<Args>(_inArgs)...)
        );

        // �p�b�P�[�W�^�X�N����future���擾����B
        std::future<ReturnType> result = task->get_future();
        {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            // �V���b�g�_�E�����Ƀ^�X�N�����s����悤�Ƃ����ꍇ�A��O�𓊂���B
            if (m_bShutdown) {
                throw std::runtime_error("Cannot execute tasks after shutdown.");
            }
            // �^�X�N�L���[�ɐV�����^�X�N��ǉ��B
            m_TaskQueue.emplace([task]() { (*task)(); });
        }
        // �V�����^�X�N���ǉ����ꂽ�̂ŁA�ҋ@���̃X���b�h�ɒʒm����B
        m_Condition.notify_one();

        return result;
    }

    /**
     * @brief �񓯊��֐��}�l�[�W���̃V���b�g�_�E���B
     *      �S�ẴX���b�h���I�����A�S�Ẵ^�X�N������������B
     */
    inline void Shutdown() {
        {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            // �V���b�g�_�E���t���O��true�ɐݒ�B
            m_bShutdown = true;
        }
        // ���ׂẴX���b�h���ڊo�߂�悤�ɒʒm����B
        m_Condition.notify_all();
        // ���ׂẴX���b�h���I������̂�҂B
        for (auto& thread : m_Threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    /**
     * @brief ���ׂẴ^�X�N�������������ǂ������m�F����B
     * @return ���ׂẴ^�X�N���������Ă����true�A�����łȂ����false��Ԃ��B
     */
    inline bool IsAllTasksCompleted() const {
        std::lock_guard<std::mutex> lock(m_QueueMutex);
        // �^�X�N�L���[���󂩂ǂ�����Ԃ��B
        return m_TaskQueue.empty();
    }

private:
    /**
     * @brief �X���b�h�����s����֐��B
     *      �^�X�N�L���[����^�X�N�����o���A���s����B
     */
    inline void Run() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(m_QueueMutex);
                // �V�����^�X�N���ǉ�����邩�A�V���b�g�_�E���t���O��true�ɂȂ�܂őҋ@�B
                m_Condition.wait(lock, [this] { return m_bShutdown || !m_TaskQueue.empty(); });

                // �V���b�g�_�E�����Ă��ă^�X�N�L���[����Ȃ烋�[�v�𔲂���B
                if (m_bShutdown && m_TaskQueue.empty()) {
                    return;
                }

                // �L���[���玟�̃^�X�N�����o���B
                task = std::move(m_TaskQueue.front());
                m_TaskQueue.pop();
            }

            // �^�X�N�����s����B
            task();
        }
    }

    //! ���s�p�X���b�h
    std::vector<std::thread> m_Threads;
    //! �V���b�g�_�E���t���O
    std::atomic<bool> m_bShutdown;
    //! �^�X�N�L���[��ی삷��~���[�e�b�N�X
    mutable std::mutex m_QueueMutex;
    //! ���s�҂��̃^�X�N�L���[
    std::queue<std::function<void()>> m_TaskQueue;
    //! �^�X�N�̒ǉ���ҋ@��������ϐ�
    std::condition_variable m_Condition;
};