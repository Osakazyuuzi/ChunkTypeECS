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
 * @brief 非同期関数を管理し、複数のスレッドで実行するマネージャクラス。
 */
class AsyncFunctionManager final {
public:
    /**
     * @brief コンストラクタ。
     * @param _inUseThreadNum 使用するスレッド数。デフォルトはハードウェアのコア数。
     * @throws std::invalid_argument 使用するスレッド数が0の場合にスローされる例外。
     */
    inline AsyncFunctionManager(std::size_t _inUseThreadNum = std::thread::hardware_concurrency()) {
        // スレッドの数だけループを回して、各スレッドでRun関数を実行する。
        for (std::size_t i = 0; i < _inUseThreadNum; ++i) {
            m_Threads.emplace_back([this] { this->Run(); });
        }
    }

    /**
     * @brief テンプレートメンバ関数。非同期に関数を実行する。
     * @param _inFunc 実行する関数。
     * @param _inArgs 実行する関数の引数。
     * @return 関数の実行結果を待機するためのfutureオブジェクト。
     */
    template <typename Func, typename... Args>
    auto Execute(Func&& _inFunc, Args&&... _inArgs) 
        -> std::future<std::invoke_result_t<Func, Args...>> {
        using ReturnType = std::invoke_result_t<Func, Args...>;

        // 関数をパッケージタスクに変換し、それを共有ポインタでラップする。
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<Func>(_inFunc), std::forward<Args>(_inArgs)...)
        );

        // パッケージタスクからfutureを取得する。
        std::future<ReturnType> result = task->get_future();
        {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            // シャットダウン中にタスクが実行されようとした場合、例外を投げる。
            if (m_bShutdown) {
                throw std::runtime_error("Cannot execute tasks after shutdown.");
            }
            // タスクキューに新しいタスクを追加。
            m_TaskQueue.emplace([task]() { (*task)(); });
        }
        // 新しいタスクが追加されたので、待機中のスレッドに通知する。
        m_Condition.notify_one();

        return result;
    }

    /**
     * @brief 非同期関数マネージャのシャットダウン。
     *      全てのスレッドを終了し、全てのタスクを完了させる。
     */
    inline void Shutdown() {
        {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            // シャットダウンフラグをtrueに設定。
            m_bShutdown = true;
        }
        // すべてのスレッドが目覚めるように通知する。
        m_Condition.notify_all();
        // すべてのスレッドが終了するのを待つ。
        for (auto& thread : m_Threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    /**
     * @brief すべてのタスクが完了したかどうかを確認する。
     * @return すべてのタスクが完了していればtrue、そうでなければfalseを返す。
     */
    inline bool IsAllTasksCompleted() const {
        std::lock_guard<std::mutex> lock(m_QueueMutex);
        // タスクキューが空かどうかを返す。
        return m_TaskQueue.empty();
    }

private:
    /**
     * @brief スレッドが実行する関数。
     *      タスクキューからタスクを取り出し、実行する。
     */
    inline void Run() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(m_QueueMutex);
                // 新しいタスクが追加されるか、シャットダウンフラグがtrueになるまで待機。
                m_Condition.wait(lock, [this] { return m_bShutdown || !m_TaskQueue.empty(); });

                // シャットダウンしていてタスクキューが空ならループを抜ける。
                if (m_bShutdown && m_TaskQueue.empty()) {
                    return;
                }

                // キューから次のタスクを取り出す。
                task = std::move(m_TaskQueue.front());
                m_TaskQueue.pop();
            }

            // タスクを実行する。
            task();
        }
    }

    //! 実行用スレッド
    std::vector<std::thread> m_Threads;
    //! シャットダウンフラグ
    std::atomic<bool> m_bShutdown;
    //! タスクキューを保護するミューテックス
    mutable std::mutex m_QueueMutex;
    //! 実行待ちのタスクキュー
    std::queue<std::function<void()>> m_TaskQueue;
    //! タスクの追加を待機する条件変数
    std::condition_variable m_Condition;
};