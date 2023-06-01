#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <mutex>
#include <ctime>
#include <array>
#pragma warning(disable : 4996)

std::default_random_engine gen(std::time(0));
std::uniform_int_distribution<long int> dist(1, 1000000);
std::mutex mtx;

class Event
{
public:
	long int numberEv = randGen();
	time_t timeEv = getDate();

	long int randGen()
	{
		long int num;
		long int randomNumber = dist(gen);
		num = randomNumber;
		std::lock_guard<std::mutex> guard(mtx);
		return num;

		
	}
	time_t getDate()
	{
		//Была мысль сохранять временную метку в виде массива *char в формате Д.М.Г Ч:М,
		//но так как вывода на консоль этих данных не требудется оставил просто дату в милисекундах
		
		/*std::time_t timeEv;
		struct tm* timeinfo;
		char buffer[80];
		time(&timeEv);
		timeinfo = localtime(&timeEv);
		strftime(buffer, 80, "Сегодня %d.%m.%y, время: %H:%M", timeinfo);
		return timeEv;*/

		std::time_t ms = std::time(nullptr);
		std::lock_guard<std::mutex> guard(mtx);
		return ms;

	}

	
};



bool is_prime(long int num)
{
	//std::lock_guard<std::mutex> guard(mtx);
	if (num < 2) { return false; }
	for (long int i = 2; i * i <= num; i++)
	{
		if (num % i == 0) { return false; }
	}
	return true;
}



int main()
{
	setlocale(LC_ALL, "Russian");
	long int ct_gen;
	long int ct_hand;
	long int ct_even;
	std::cout << "Укажите количество потоков генераторов событий: "; std::cin >> ct_gen;
	std::cout << "Укажите количество событий, которые необходимо сгенерировать на каждом из потоков: "; std::cin >> ct_even;
	std::cout << "Укажите количество потоков обработчиков событий: "; std::cin >> ct_hand;

	Event** ev = new Event * [ct_gen*ct_even];
	long int every_count = 0;
	long int count_prime_num = 0;
	std::thread* thr_gen = new std::thread[ct_gen];
	std::thread* thr_hand = new std::thread[ct_even];
	std::cout << "\n" << "===============Всего чисел============" << "\n";
	for (int i = 0; i < ct_gen; i++)
	{
		thr_gen[i] = std::thread([&]() {
			for (int j = 0; j < ct_even; j++)
			{
				Event* mean = new Event;
				mean->getDate();
				mean->randGen(); 
				mtx.lock();
				ev[j] = mean;
				mtx.unlock();
				every_count++;
			}
			});
		thr_gen[i].join();
		
	}
	std::cout << every_count;
	std::cout << "\n" << "===============Количество простых чисел============"<<"\n";
	for (int i = 0; i < ct_hand; i++)
	{
		
		thr_hand[i] = std::thread([&]() {
			for (int j = 0; j< ct_even; j++)
			{
				mtx.lock();
				if (is_prime(ev[j]->numberEv))
					count_prime_num++;
				mtx.unlock();
			}
		
			});
		thr_hand[i].join();
	}
	std::cout << count_prime_num<<"\n";

	system("pause");
	return 0;
}