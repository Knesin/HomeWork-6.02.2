#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <string>

struct Bar_data {
public:
    int number;
    std::thread::id id_process;
    std::atomic<int> progress;
    std::chrono::duration<double> time;
};

void print(Bar_data*& b, const int count) {
    const int length_bar = 25; // длина прогрес бара
    std::string full_bar(length_bar, '#');
    int exit_flag = count;
    while (exit_flag) //выполняется пока все процессы не проставят время выполнения по завершению
    {
        exit_flag = count;
        system("cls");
        std::cout << "#\tid\t\tProgress Bar\t\tTime" << std::endl; //заголовок
        for (int i = 0; i < count; ++i) {
            std::cout << b[i].number << '\t' << b[i].id_process << '\t';
            if (b[i].time.count() > 0) { //процес завершился
                std::cout << full_bar;
                std::cout << '\t' << b[i].time.count() << "s" << std::endl;
                exit_flag--;
            }
            else {
                for (int j = 0; j < b[i].progress / (100 / length_bar); j++) {//рисуем шкалу
                    std::cout << "#";
                }
                std::cout << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void func(Bar_data& b) {
    b.id_process = std::this_thread::get_id();
    b.progress = 0;
    auto start = std::chrono::high_resolution_clock::now();
    while (b.progress != 100) { //в этом цикле должны быть основные вычисления
        b.progress++;
        if (b.progress > 100)
            b.progress = 100;
        std::this_thread::sleep_for(std::chrono::milliseconds(100 / (b.number + 1)));
    }
    auto end = std::chrono::high_resolution_clock::now();
    b.time = end - start;
}

int main()
{
    int count_thread = 4; //количество потоков
    std::thread* t = new std::thread[count_thread];
    Bar_data* bd = new Bar_data[count_thread];
    for (int i = 0; i < count_thread; ++i) {
        bd[i].number = i;
        t[i] = std::thread(func, std::ref(bd[i]));
    }
    std::thread print_bar(print, std::ref(bd), count_thread);

    for (int i = 0; i < count_thread; ++i) {
        t[i].join();
    }
    print_bar.join();
    delete[] t;
    delete[] bd;
}