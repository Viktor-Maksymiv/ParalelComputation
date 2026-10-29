#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>
using namespace std;

mutex m;
queue<int> numbers;
bool ready = false;

bool isPrime(int n)
{
    if (n < 2)
        return false;
    for (int i = 2; i * i <= n; i++)
        if (n % i == 0)
            return false;
    return true;
}

void DataPreparation()
{
    cout << "Введіть цілі числа (для завершення введіть будь-який нецифровий символ):" << endl;
    int x;
    while (cin >> x)
    {
        lock_guard<mutex> lk(m);
        numbers.push(x);
    }
    lock_guard<mutex> lk(m);
    ready = true;
}

void DataProcessing()
{
    unique_lock<mutex> lk(m);
    while (!ready)
    {
        lk.unlock();
        this_thread::sleep_for(chrono::milliseconds(100));
        lk.lock();
    }
cout << "Прості числа:" << endl;
    while (!numbers.empty())
    {
        int x = numbers.front();
        numbers.pop();
        if (isPrime(x))
            cout << x << " ";
    }
    cout << endl;
}

int main()
{
    thread preparation(DataPreparation);
    preparation.detach();
    thread processing(DataProcessing);
    processing.join();
    return 0;
}
