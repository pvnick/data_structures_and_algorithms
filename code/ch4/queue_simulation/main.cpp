#include <iostream>
#include "queue.h"
#include <string>
#include <stdlib.h>

int main()
{
    const int M = 4;
    int N = 100;
    Queue<int> queues[M];
    for (int i = 0; i < N; ++i, std::cout << std::endl) {
        //N times iterate
        //add customer to one of our M queues, also choose one to service
        int in = rand() % M, out = rand() % M;
        queues[in].put(i);
        std::cout << i << std::endl;
        //if customer currently in serviced queue, remove it
        if (!queues[out].is_empty())
            std::cout << queues[out].get() << " out";
        std::cout << std::endl;
        //print contents of each queue
        for (int k = 0; k != M; k++, std::cout << std::endl) {
            Queue<int> q = queues[k]; //this requires copy constructor/assigner
            std:: cout << k << ": ";
            while (!q.is_empty())
                std::cout << q.get() << " ";
        }
    }
    return 0;
}
