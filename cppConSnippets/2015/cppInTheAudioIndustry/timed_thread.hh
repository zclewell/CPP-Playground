#ifndef __TIMED_THREAD_HH__
#define __TIMED_THREAD_HH__

#include <pthread.h>
#include <unistd.h>

class TimedThread {
	public:
		TimedThread(useconds_t timeout) : m_running(true), m_timeout(timeout) {
			m_thread = new pthread_t;
		}

		virtual ~TimedThread() {
			stop();

			delete m_thread;
		}

		void start() {
			pthread_create(m_thread, nullptr, threadFunc, this);
		}

		void stop() {
			m_running = false;

			pthread_join(*m_thread, nullptr);
		}
	protected:
		virtual void doSomething() = 0;
			
	private:
		static void *threadFunc(void *arg) {
			TimedThread *timedThread = static_cast<TimedThread*>(arg);

			while(timedThread->m_running) {
				usleep(timedThread->m_timeout);
				timedThread->doSomething();
			}

			return nullptr;
		}

		bool m_running;
		pthread_t *m_thread;
		useconds_t m_timeout;
};

#endif /* __TIMED_THREAD_HH__ */	
