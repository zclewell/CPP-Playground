#ifndef __RELEASE_POOL_HH__
#define __RELEASE_POOL_HH__

#include <unistd.h>

#include <mutex>
#include <algorithm>

#include "timed_thread.hh"

class ReleasePool : public TimedThread {
	public:
		ReleasePool(useconds_t timeout) : TimedThread(timeout) {
		}

		~ReleasePool() {
			stop();
		}

		void add(std::shared_ptr<void> p) {
			const std::lock_guard<std::mutex> lock(m_mutex);
			m_vector.push_back(p);
		}
	protected:
		void doSomething() override {
			const std::lock_guard<std::mutex> lock(m_mutex);
			m_vector.erase(std::remove_if(m_vector.begin(), m_vector.end(), [](std::shared_ptr<void> &p) { return p.use_count() <= 1; }), m_vector.end());
		}

		std::vector<std::shared_ptr<void> > m_vector;
		std::mutex m_mutex;
};

#endif /* __RELEASE_POOL_HH__ */
