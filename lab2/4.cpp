#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
using namespace std;

mutex m;
condition_variable cv;
queue<int> numbers;

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
    cv.notify_one();
}

void DataProcessing()
{
    unique_lock<mutex> lk(m);
    cv.wait(lk, [] { return !numbers.empty(); });
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
