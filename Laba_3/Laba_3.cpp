
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include "mpi.h" 
#define TAG_N 1// тег размера массива
#define TAG_A 2// тег массива
#define TAG_S 3// тег результата



using namespace std;

int calculation(int* a, int N, int rank, int size) {// массив а, кол-во эл, № процесса, кол-во процессов
    int nt, beg, h, end, i;
    int sum = 0;// сумма (результат выполнения)
    nt = rank;// № процесса
    h = N / size;// узнаем сколько элементов будет входить в процесс (разбиваем общее число эл-тов на кол-во процессов)/ шаг
    beg = h * nt;//начало параллельного процесса (скорее всего одного)
    //вся конструкция для того, чтоб захватить все эл-ты (с 23 по 30 строку)
    if (nt == size - 1)
    {
        end = N - 1;
    }
    else
    {
        end = beg + h;
    }

    cout << "Process, begin, end of array " << nt << " " << beg << " " << end << "\n";// сообщение о № процесса, начале, конце
    for (i = beg; i <= end; i++)
    {
        if (i % 2 == 0)
        {
            sum += a[i];// чет.индк +
        }
        else
        {
            sum -= a[i];// нечет -
        }
    }
    return sum;// возвращаем сумму
}

int main(int argc, char** argv) {

    srand(time(0));// для генератора, чтоб были разные числа при запуске
    int rank, size;// № процесса и кол-во процессов
    int* a;// указатель на массив
    int N;// кол-во эл.тов
    bool fail = false;
    MPI_Status status;//атрибут сообщения
    MPI_Init(&argc, &argv);//иницилизация паралелльной части
    MPI_Comm_size(MPI_COMM_WORLD, &size);//определение общего числа параллельных процессов
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);//определение номера процесса
    cout << "The number of processes: " << size << " my number is " << rank << "\n";// сообщение о № процесса из ...


    MPI_Barrier(MPI_COMM_WORLD);//Блокирует работу процессов, вызвавших данную процедуру, до тех пор, пока все оставшиеся процессы также не выполнят эту процедуру
    double time = MPI_Wtime();// запускам таймер
    if (rank == 0) {// если 0ой ранг 

        do {
            fail = false;
            cout << "Enter size array = ";
            cin >> N;// вводим размер массива
            if (cin.fail() || (N <= 0)) {
                cout << "You entered a non number or a number < 0. Enter again!" << endl;
                fail = true;
            }
            cin.clear();
            cin.ignore();
        } while (fail);



        a = new int[N];// иницилизируем массив, рамзер Эн
        for (int i = 0; i < N; i++)
        {
            a[i] = 100 + rand() % 10000000;// заполняем массив
        }

        

        for (int i = 1; i < size; i++)//передача сообщений между отдельными процессами
        {
            int val = N % size;
            MPI_Send(&val, 1, MPI_INT, i, TAG_N, MPI_COMM_WORLD);                              
        }

        for (int i = 1; i < size; i++)// смотреть выше
        {
            MPI_Send(a, N % size, MPI_INT, i, TAG_A, MPI_COMM_WORLD);
        }


        int sum = calculation(a, N, rank, size);// обявляем перменную, в которой считается наша функция, находим сумму на нулевом ранге
        for (int i = 1; i < size; i++) {
            int s = 0;// перменная для суммы в других рангах
            MPI_Recv(&s, 1, MPI_INT, i, TAG_S, MPI_COMM_WORLD, &status);// принимаем сообщения с других рангов //7-атрибут - статус сообщения
            cout << "status.MPI_SOURCE " << status.MPI_SOURCE << "\n";// сообщение о № процесса
            cout << "status.MPI_TAG " << status.MPI_TAG << "\n";// тег 
            cout << "status.MPI_ERROR " << status.MPI_ERROR << "\n";// ошибка
            sum += s;// складываем все параллельные процессы (к результату 0го ранга прибавляем результаты с других рангов)
        }
        cout << "Znachenie virajenia: " << sum << "\n";// сообщение о результате

    }
    else {// не 0е ранги
        int N1 = 0;// так как принимаем соошения с нулевого, то значение = 0
        MPI_Recv(&N1, 1, MPI_INT, 0, TAG_N, MPI_COMM_WORLD, &status);// принмаем сообщение с 0го ранга
        //1-а адрес перменной
        //2-а кол-во переменой
        //3-а тип данных перменной
        //4-а № ранга (0) откуда получаем сообщение
        //5-а тег переменной
        //6-а коммуникатор
        //7-ой статус сообщения
        int* a1 = new int[N1];// иницилизируем массив 
        MPI_Recv(a1, N1, MPI_INT, 0, TAG_A, MPI_COMM_WORLD, &status);// принимаем сообщение с 0го ранга
        int s = calculation(a1, N1, rank, size);// считаем значение нашей функции
        cout << "Rank: " << rank << " rezult_rank = " << s << "\n";// выводим результата на 1ом ранге
        MPI_Send(&s, 1, MPI_INT, 0, TAG_S, MPI_COMM_WORLD);// передем результат в 0ой ранг
        
    }
    time = MPI_Wtime() - time;// считаем время выполнения
    cout << "Computation time = " << time << "\n";//выводим время

    MPI_Finalize();// останавливаем параллельный процесс
    return 0;
}


