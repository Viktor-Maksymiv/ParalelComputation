#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include <cmath>
using namespace std;

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

void Producer(int n, promise<long long> first, promise<bool> started, promise<long long> second)
{
    first.set_value(nthPrime(n));
    started.set_value(true);
    second.set_value(nthPrime(n * 10));
}

void Consumer(int n, future<bool> started)
{
    started.get();
    this_thread::sleep_for(chrono::seconds(2));
    cout << "sqrt(" << n << ") = " << sqrt(n) << endl;
}

int main()
{
    int n;
    cout << "Введіть n: ";
    cin >> n;

    promise<long long> firstPromise;
    promise<bool> startedPromise;
    promise<long long> secondPromise;
    future<long long> firstFuture = firstPromise.get_future();
    future<bool> startedFuture = startedPromise.get_future();
    future<long long> secondFuture = secondPromise.get_future();

    thread producer(Producer, n, move(firstPromise), move(startedPromise), move(secondPromise));
    thread consumer(Consumer, n, move(startedFuture));

    cout << n << "-те просте число: " << firstFuture.get() << endl;
    cout << n * 10 << "-те просте число: " << secondFuture.get() << endl;

    producer.join();
    consumer.join();
    return 0;
}
