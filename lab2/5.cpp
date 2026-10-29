#include <iostream>
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

int main()
{
    int mode, n, choice;
    cout << "Режим запуску (1 - deferred, 2 - async): ";
    cin >> mode;
    cout << "Введіть номер простого числа n: ";
    cin >> n;

    launch policy = (mode == 1) ? launch::deferred : launch::async;
    future<long long> f = async(policy, nthPrime, n);

    cout << "Оберіть функцію від n (1 - корінь квадратний, 2 - синус, 3 - натуральний логарифм): ";
    cin >> choice;
    if (choice == 1)
        cout << "sqrt(" << n << ") = " << sqrt(n) << endl;
    else if (choice == 2)
        cout << "sin(" << n << ") = " << sin(n) << endl;
    else
        cout << "ln(" << n << ") = " << log(n) << endl;

    auto start = chrono::steady_clock::now();
    long long result = f.get();
    auto end = chrono::steady_clock::now();

    cout << n << "-те просте число: " << result << endl;
    cout << "Час очікування результату: "
         << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " мс" << endl;
    return 0;
}
