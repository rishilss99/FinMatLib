#include "Pipeline.h"
#include "Executor.h"
#include "Task.h"

using namespace std;

template <typename T>
Pipeline<T>::Pipeline() : empty(true)
{
}

template <typename T>
T Pipeline<T>::read()
{
	unique_lock<mutex> lock(mtx);
	while (empty)
	{
		cv.wait(lock);
	}
	empty = true;
	cv.notify_all();
	return this->value;
}

template <typename T>
void Pipeline<T>::write(T value)
{
	unique_lock<mutex> lock(mtx);
	while (!empty)
	{
		cv.wait(lock);
	}
	this->value = value;
	empty = false;
	cv.notify_all();
}

template <typename T>
class WriteTaskVal : public Task
{
public:
	Pipeline<T> &pipeline;

	void execute()
	{
		for (int i = 0; i < 100; i++)
		{
			pipeline.write(i);
		}
	}

	WriteTaskVal<T>(Pipeline<T> &pipeline) : pipeline(pipeline)
	{
	}
};

template <typename T>
class ReadTaskVal : public Task
{
public:
	Pipeline<T> &pipeline;
	double total;

	void execute()
	{
		for (int i = 0; i < 100; i++)
		{
			total += pipeline.read();
		}
	}

	ReadTaskVal<T>(Pipeline<T> &pipeline) : pipeline(pipeline),
									  total(0.0)
	{
	}
};

template class Pipeline<double>;
template class WriteTaskVal<double>;
template class ReadTaskVal<double>;

static void testTwoThreads()
{
	Pipeline<double> pipeline;
	auto w = make_shared<WriteTaskVal<double>>(pipeline);
	auto r = make_shared<ReadTaskVal<double>>(pipeline);
	SPExecutor executor = Executor::newInstance(2);
	executor->addTask(r);
	executor->addTask(w);
	executor->join();

	ASSERT_APPROX_EQUAL(r->total, 99.0 * 50.0, 0.1);
}

void testPipeline()
{
	TEST(testTwoThreads);
}