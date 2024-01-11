#include "Executor.h"

#include "stdafx.h"

using namespace std;

/**
 *   Implementation of executor
 */
class ExecutorImpl : public Executor
{
public:
    ExecutorImpl(int maxThreads);
    void addTask(shared_ptr<Task> task);
    void addTask(function<void()> functor);
    void join();

    /*  The maximum number of threads that may run at once */
    int maxThreads;
    /*  The current number of running threads */
    int numRunningThreads;
    /*  Mutex to coordinate threads */
    mutex mtx;
    /*  Condition variable used to signal when no tasks are running */
    condition_variable cv;
    /*  Queue where we add tasks we aren't ready to run */
    vector<shared_ptr<Task>> queue;
    /*  Threads we have created */
    vector<thread *> threads;
    /*  Weak pointer to the executor */
    weak_ptr<ExecutorImpl> self;
};

/**
 *  Each thread calls the runTasks method which takes
 *  this form of data as a parameter
 */
class RunData
{
public:
    /*  The associated executor */
    shared_ptr<ExecutorImpl> executor;
    /*  The current task */
    shared_ptr<Task> firstTask;
    /*  The thread that is running */
    thread *runningThread;

    RunData(shared_ptr<ExecutorImpl> executor, shared_ptr<Task> firstTask) : executor(executor),
                                                                             firstTask(firstTask),
                                                                             runningThread(NULL) {}
};

/**
 *   When a thread exits, we must delete the associated thread object
 *   This class performs this task
 */
class ThreadCleanup
{
public:
    mutex &mtx;
    RunData *runData;
    ThreadCleanup(mutex &mtx,
                  RunData *runData) : mtx(mtx),
                                      runData(runData)
    {
    }

    ~ThreadCleanup()
    {
        lock_guard<mutex> lock(mtx);
        runData->runningThread->detach();
        delete runData->runningThread;
        delete runData;
    }
};

/**
 *   Run tasks that have been added to the executor
 */
void runTasks(RunData *runData)
{

    shared_ptr<ExecutorImpl> executor = runData->executor;
    shared_ptr<Task> currentTask = runData->firstTask;
    runData->firstTask.reset();

    ThreadCleanup tc(executor->mtx, runData);

    bool taskToPerform = true;
    while (taskToPerform)
    {
        currentTask->execute();
        taskToPerform = false;
        lock_guard<mutex> lock(executor->mtx);
        if (executor->queue.size() > 0)
        {
            taskToPerform = true;
            currentTask = executor->queue.back();
            executor->queue.pop_back();
        }
    }

    lock_guard<mutex> lock(executor->mtx);
    executor->numRunningThreads--;
    if (executor->numRunningThreads == 0)
    {
        executor->cv.notify_all();
    }
}

/**
 *   Creates an executor
 */
ExecutorImpl::ExecutorImpl(int maxThreads) : maxThreads(maxThreads),
                                             numRunningThreads(0)
{
}

/**
 *   Add a task to the executor
 */
void ExecutorImpl::addTask(shared_ptr<Task> task)
{
    lock_guard<mutex> lock(mtx);
    if (numRunningThreads >= maxThreads)
    {
        queue.push_back(task);
    }
    else
    {
        numRunningThreads++;
        shared_ptr<ExecutorImpl> sharedPtr(self);
        RunData *runData = new RunData(sharedPtr, task);
        thread *t = new thread(runTasks, runData);
        runData->runningThread = t;
    }
}

void ExecutorImpl::addTask(function<void()> functor)
{
    class FunctorTask : public Task
    {
    public:
        FunctorTask(function<void()> functor) : functor_(functor) {}
        void execute()
        {
            functor_();
        }

    private:
        function<void()> functor_;
    };

    shared_ptr<Task> functorTask = make_shared<FunctorTask>(functor);
    addTask(functorTask);
}

/**
 *   Wait until all threads have completed
 */
void ExecutorImpl::join()
{
    unique_lock<mutex> lock(mtx);
    while (numRunningThreads > 0)
    {
        cv.wait(lock);
    }
}

/**
 *  Returns an executor
 */
shared_ptr<Executor> Executor::newInstance()
{
    return newInstance(thread::hardware_concurrency());
}

/**
 *  Returns an executor
 */
shared_ptr<Executor> Executor::newInstance(int maxThreads)
{
    shared_ptr<ExecutorImpl> ret = make_shared<ExecutorImpl>(maxThreads);
    ret->self = ret;
    return ret;
}

static void test100Tasks()
{
    class MyTask : public Task
    {
    public:
        bool run;

        void execute()
        {
            run = true;
        }

        MyTask() : run(false) {}
    };

    shared_ptr<Executor> executor = Executor::newInstance();
    vector<shared_ptr<MyTask>> tasks;
    int nTasks = 100;
    for (int i = 0; i < nTasks; i++)
    {
        shared_ptr<MyTask> task = make_shared<MyTask>();
        tasks.push_back(task);
        executor->addTask(task);
    }

    executor->join();
    for (int i = 0; i < nTasks; i++)
    {
        ASSERT(tasks[i]->run);
    }
}

static void test100FunctorTasks()
{
    shared_ptr<Executor> executor = Executor::newInstance();
    vector<int> vals(100, 0);
    int nTasks = 100;
    for (int i = 0; i < nTasks; i++)
    {
        auto lmbd = [&vals, i]()
        { vals[i] = i; };
        executor->addTask(lmbd);
    }
    executor->join();
    for (int i = 0; i < nTasks; i++)
    {
        ASSERT(vals[i] == i);
    }
}

static void testComputeMeanTasks()
{
    vector<double> vec(20);
    mt19937 mersennneEngine;
    uniform_real_distribution<double> dist{1.0, 52.0};
    auto gen = [&]()
    { return dist(mersennneEngine); };
    generate(vec.begin(), vec.end(), gen);
    double sum = accumulate(vec.begin(), vec.end(), 0.0);
    double mean = sum / vec.size();

    int nTasks = 8;
    shared_ptr<Executor> executor = Executor::newInstance(nTasks);
    vector<double> taskSum(nTasks);
    int taskVecLength = (vec.size() + nTasks - 1) / nTasks;
    for (int i = 0; i < nTasks; i++)
    {
        auto lmbd = [&, taskVecLength, i]()
        {
            int startIdx = i * taskVecLength;
            int endIdx = min((i + 1) * taskVecLength, static_cast<int>(vec.size()));
            for (; startIdx < endIdx; startIdx++)
            {
                taskSum[i] += vec[startIdx];
            }
        };
        executor->addTask(lmbd);
    }
    executor->join();
    double calcMean = accumulate(taskSum.begin(), taskSum.end(), 0.0) / vec.size();
    ASSERT_APPROX_EQUAL(mean, calcMean, 0.01);
}

static void testComputeMeanThreads()
{
    size_t vecSize = 60;
    vector<double> vec(vecSize);
    default_random_engine defEngine;
    uniform_real_distribution<> unifDist{0.0, 100.0};
    auto gen = [&]()
    { return unifDist(defEngine); };
    generate(vec.begin(), vec.end(), gen);

    double mean = accumulate(vec.begin(), vec.end(), 0.0) / static_cast<double>(vecSize);

    size_t nThreads = 13;
    size_t taskVecLength = (vecSize + nThreads - 1) / nThreads;
    vector<thread> threadVec;
    vector<double> threadSum(nThreads);
    for (size_t i = 0; i < nThreads; i++)
    {
        threadVec.push_back(thread{[&, taskVecLength, i]()
                                   {
                                       size_t startIdx = i * taskVecLength;
                                       size_t endIdx = min((i + 1) * taskVecLength, vec.size());
                                       for (; startIdx < endIdx; startIdx++)
                                       {
                                           threadSum[i] += vec[startIdx];
                                       }
                                   }});
    }
    for (size_t i = 0; i < nThreads; i++)
    {
        threadVec[i].join();
    }

    double calcMean = accumulate(threadSum.begin(), threadSum.end(), 0.0) / static_cast<double>(vecSize);
    ASSERT_APPROX_EQUAL(mean, calcMean, 0.01);
}

void testExecutor()
{
    TEST(test100Tasks);
    TEST(test100FunctorTasks);
    TEST(testComputeMeanTasks);
    TEST(testComputeMeanThreads);
}
