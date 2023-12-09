#include <chrono>
#include <thread>

class Timer
{
public:
	template<class DurationType = std::chrono::seconds>
	int64_t Time() const noexcept
	{
		return std::chrono::duration_cast<DurationType>(std::chrono::high_resolution_clock::now() - start).count();
	}

	template<class T, class DurationType = std::chrono::seconds>
	void Sleep(T t)
	{
		std::this_thread::sleep_for((DurationType)t);
	}

	void Reset() noexcept;

private:
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
};

