#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <vector>
#include <chrono>
#include <random>
using namespace std;

template<typename T>
void BubbleSort(std::vector<T>& vec)  
{
	bool swapped;
	for (size_t i = 0; i < vec.size(); ++i)
	{
		swapped = false;
		for (size_t j = 0; j < vec.size() - i - 1; ++j)
		{
			if (vec[j] > vec[j + 1]) {
				std::swap(vec[j], vec[j + 1]);
				swapped = true;
			}
		}
		if (!swapped) {
			break;
		}
	}
}

/* �������, ������� ����� ��������� ��������� ����� */
void* thread_job(void* arg)
{
	auto start = std::chrono::steady_clock::now();
	int vector_size = 500;
	vector<int> numbers(vector_size);
	for (size_t i = 0; i < vector_size; i++)
	{
		numbers[i] = rand() % vector_size;
	}
	BubbleSort(numbers);
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	//cout << "Thread#" << *(int*)(arg) << " has finished for " 
	//	<< elapsed.count() << " seconds." << endl;
	return NULL;
}
int main()
{
	// ���������� ����������: ������������� ������ � ��� ������
	int err;
	int threads_number;
	// ������ �����
	cout << "Enter number of threads: ";
	cin >> threads_number; 
	pthread_t* threads = new pthread_t[threads_number];
	int* thread_ids = new int[threads_number];
	auto start = std::chrono::steady_clock::now();
	for (size_t n = 0; n < threads_number; n++) {
		thread_ids[n] = n;
		err = pthread_create(&threads[n], NULL, thread_job, &thread_ids[n]);
		// ���� ��� �������� ������ ��������� ������, �������
		// ��������� �� ������ � ���������� ������ ���������
		if (err != 0) {
			cout << "Cannot create a thread: " << strerror(err) << endl;
			exit(-1);
		}
	}
	
	// ������� ���������� ���������� ������ ����� �����������
	// ������ ���������
	auto end1 = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed1 = end1 - start;

	cout << "All Threads have started for " << elapsed1.count() << " seconds." << endl;
	for (size_t n = 0; n < threads_number; n++) {
		pthread_join(threads[n], NULL);
	}

	auto end2 = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed2 = end2 - start;
	cout << "All Threads have finished for " << elapsed2.count() << " seconds." << endl;

	auto start_single = std::chrono::steady_clock::now();
	for (size_t n = 0; n < threads_number; n++) {
		int vector_size = 500;
		vector<int> numbers(vector_size);
		for (size_t i = 0; i < vector_size; i++)
		{
			numbers[i] = rand() % vector_size;
		}
		BubbleSort(numbers);
	}
	auto end_single = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_single = end_single - start_single;
	cout << "Single thread have finished for " << elapsed_single.count() << " seconds." << endl;
}