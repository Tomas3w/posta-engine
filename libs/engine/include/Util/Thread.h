#ifndef POSTAENGINE_THREAD_UTILITY_H
#define POSTAENGINE_THREAD_UTILITY_H
#include <memory>
#include <thread>
#include <atomic>

namespace Engine {
	class Thread
	{
		public:
			Thread() : running(false) {}
			Thread(const Thread&) = delete;
			Thread& operator=(const Thread&) = delete;
			~Thread()
			{
				if (thread && thread->joinable())
					thread->join();
			}

			operator bool()
			{
				return thread.get() != nullptr;
			}

			void reset()
			{
				if (thread)
				{
					if (thread->joinable())
						thread->join();
					thread.reset();
				}
			}

			template<typename F, class... Args>
			void reset(F f, Args... args)
			{
				thread.reset(new std::thread([this, f, args...](){ running = true; f(args...); running = false; }));
			}

			bool is_running()
			{
				return running;
			}
		private:
			std::unique_ptr<std::thread> thread;
			std::atomic<bool> running;
	};
}

#endif // POSTAENGINE_THREAD_UTILITY_H
