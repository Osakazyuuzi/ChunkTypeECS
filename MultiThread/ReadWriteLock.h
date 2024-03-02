#pragma once

#include <mutex>
#include <condition_variable>

/**
 * @class ReadWriteLock
 * @brief 読み取りと書き込みアクセスを同期するためのロック機構を提供します。
 *
 * このクラスは、複数の読み取りアクセスが同時に行われることを許可しながら、
 * 書き込みアクセスが排他的に行われることを保証します。これにより、
 * 読み取りアクセスが多いが書き込みアクセスが少ないシナリオでの性能が向上します。
 */
class ReadWriteLock {
public:
    ReadWriteLock()
        : m_RefCount(0), m_bExistsWriter(false) {}

    /**
     * @brief 読み取りアクセス用のロックを取得します。
     *
     * 他のスレッドが書き込み中でない場合にのみ、読み取りアクセスを許可します。
     * 書き込みスレッドが存在する場合、そのスレッドがロックを解放するまで待機します。
     */
    void LockRead() {
        std::unique_lock<std::mutex> lock(m_Mutex);

        m_ReadCondition.wait(lock, [this]() { return !m_bExistsWriter; });
        ++m_RefCount;
    }

    /**
     * @brief 読み取りアクセス用のロックを解放します。
     *
     * 読み取りカウンタをデクリメントし、他の書き込み待ちスレッドがある場合、
     * その一つを起こします。
     */
    void UnlockRead() {
        std::unique_lock<std::mutex> lock(m_Mutex);
        if (--m_RefCount == 0) {
            m_WriteCondition.notify_one();
        }
    }

    /**
     * @brief 書き込みアクセス用のロックを取得します。
     *
     * 他のスレッドが書き込みまたは読み取り中でない場合にのみ、書き込みアクセスを許可します。
     * これらの条件が満たされない場合、条件が満たされるまで待機します。
     */
    void LockWrite() {
        std::unique_lock<std::mutex> lock(m_Mutex);

        m_WriteCondition.wait(lock, [this]() { return !m_bExistsWriter && m_RefCount == 0; });
        m_bExistsWriter = true;
    }

    /**
     * @brief 書き込みアクセス用のロックを解放します。
     *
     * 書き込みフラグをリセットし、待機中の書き込みまたは読み取りスレッドを起こします。
     */
    void UnlockWrite() {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_bExistsWriter = false;

        // 書き込み待ちのスレッドがあれば、一つだけを起こします。
        m_WriteCondition.notify_one();
        // 読み取り待ちのスレッドがあれば、全てを起こします。
        m_ReadCondition.notify_all();
    }

private:
    std::mutex m_Mutex;  ///< ロック状態の同期を取るためのミューテックス。
    std::condition_variable m_ReadCondition;  ///< 読み取りスレッドの待機条件変数。
    std::condition_variable m_WriteCondition; ///< 書き込みスレッドの待機条件変数。
    int m_RefCount;  ///< 現在の読み取りスレッドの数。
    bool m_bExistsWriter;  ///< 現在書き込みを行っているスレッドが存在するかどうか。
};
