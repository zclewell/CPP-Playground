#ifndef __NO_BLOCK_HH__
#define __NO_BLOCK_HH__

#include <vector>
#include <memory>
#include <atomic>

#include "release_pool.hh"

template<typename T, T defaultT, void (*react)(T t)>
class NoBlock {
	typedef std::shared_ptr<T> ptr_t;

	public:
		NoBlock() : m_running(true) {
			m_releasePool = std::make_shared<ReleasePool>(100000);
			m_thread = new pthread_t;
			m_releasePool->start();
		}

		~NoBlock() {
			delete m_thread;
		}

		void start() {
			pthread_create(m_thread, nullptr, threadFunc, this);
		} 

		void stop() { 
			while (std::atomic_load(&reactToNext)) {}
			std::atomic_store(&m_running, false);
		}

		void send(T t) {
			static ptr_t empty_ptr, ptr;

			ptr = std::make_shared<T>(t);
			m_releasePool->add(ptr);

			while(!std::atomic_compare_exchange_weak(&reactToNext, &empty_ptr, ptr)) {
				empty_ptr = nullptr;
			}
		}

	private:
		void inner() {
			static ptr_t empty_ptr, reactToCurr;

			empty_ptr = nullptr;
			reactToCurr = std::atomic_exchange(&reactToNext, empty_ptr);
		
			if (reactToCurr) {
				react(*reactToCurr);
			} else {
				react(defaultT);
			}
		}

		static void *threadFunc(void *arg) {
			NoBlock *nb = (NoBlock*)arg;

			while(std::atomic_load<bool>(&nb->m_running)) {
				nb->inner();
			}

			if (nb->reactToNext) {
				react(*nb->reactToNext);
			}

			return nullptr;
		}

		std::atomic<bool> m_running;
		ptr_t reactToNext;
		std::shared_ptr<ReleasePool> m_releasePool;
		pthread_t *m_thread;
};

#endif /* __NO_BLOCK_HH__ */
