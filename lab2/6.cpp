#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <deque>
#include <vector>
using namespace std;

mutex m;
condition_variable cv;
deque<packaged_task<long long(int)>> tasks;
deque<int> numbers;
bool finished = false;

long long nthPrime(int n)
{
    int count = 0;
    long long candidate = 1;
    while (count < n)
    {
        candidate++;
        bool prime = true;
        for (long long d = 2; d * d <= candidate; d++)
            if (candidate % d == 0)
            {
                prime = false;
                break;
            }
        if (prime)
            count++;
    }
    return candidate;
}

void Worker()
{
    while (true)
    {
        unique_lock<mutex> lk(m);
        cv.wait(lk, [] { return !tasks.empty() || finished; });
        if (tasks.empty())
            return;
        packaged_task<long long(int)> task = move(tasks.front());
        int n = numbers.front();
        tasks.pop_front();
        numbers.pop_front();
        lk.unlock();
        task(n);
    }
}

int main()
{
    thread worker(Worker);
    vector<int> requested;
    vector<future<long long>> results;
    int n;
    cout << "Введіть n (для завершення введіть будь-який нецифровий символ): ";
    while (true)
    {
        if (!(cin >> n) || n < 1)
            break;
        packaged_task<long long(int)> task(nthPrime);
        results.push_back(task.get_future());
        requested.push_back(n);
        {
            lock_guard<mutex> lk(m);
            tasks.push_back(move(task));
            numbers.push_back(n);
        }
        cv.notify_one();
    }
    {
        lock_guard<mutex> lk(m);
        finished = true;
    }
    cv.notify_one();
    for (size_t i = 0; i < results.size(); i++)
        cout << requested[i] << "-те просте число: " << results[i].get() << endl;
    worker.join();
    return 0;
}
