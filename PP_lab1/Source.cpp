#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <vector>
#include <chrono>
#include <random>
#include <locale>
using namespace std;

struct thread_params {
	void (*reduce)(void* arg); 
	void* item;
	int thread_number;
	bool show_params;
};


template<typename T>
void BubbleSort(void* arg)
{
	std::vector<T>* vec = (vector<T>*) arg;
	bool swapped;
	for (size_t i = 0; i < vec->size(); ++i)
	{
		swapped = false;
		for (size_t j = 0; j < vec->size() - i - 1; ++j)
		{
			if ((*vec)[j] > (*vec)[j + 1]) {
				std::swap((*vec)[j], (*vec)[j + 1]);
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
	int err;
	thread_params* params = (thread_params*)arg;
	if (params->show_params) {
		//int detachstate;
		//size_t stacksize;
		//void *stackaddr;
		//pthread_attr_t self_attr;
		//if ((err = pthread_attr_init(&self_attr)) != 0 ||
		//	(err = pthread_attr_getdetachstate(&self_attr, &detachstate)) != 0 ||
		//	(err = pthread_attr_getstackaddr(&self_attr, &stackaddr)) != 0 ||
		//	(err = pthread_attr_getstacksize(&self_attr, &stacksize)) != 0)
		//{
		//	cout << "�� ������� �������� �������� ������: " << strerror(err) << endl;
		//	exit(-1);
		//}
		//cout << "Thread#" << params->thread_number << " has attributes: " << endl <<
		//	"detachstate:	" << detachstate << endl <<
		//	"stackaddr:	" << stackaddr << endl <<
		//	"stacksize:	" << stacksize << endl << endl;
		cout << "Thread#" << params->thread_number << " has parameters: " << endl <<
			"reduce pointer:	" << (int)params->reduce << endl <<
			"item pointer:	" << (int)params->item << endl << endl;
		//pthread_attr_destroy(&self_attr);
	}
	params->reduce(params->item);
	return NULL;
}
int main()
{
	// ���������� ����������
	setlocale(LC_ALL, "Russian");
	int err;
	int threads_number, tmp_show_params;
	bool show_params;
	// ������ � ������������, � ����� ���� � ���������� ������ ���� ������� ������
	cout << "������� ���������� �������: ";
	cin >> threads_number; 
	cout << endl << "������ �� ������ �������� ���������� � ����� ���������� (0/1): ";
	cin >> tmp_show_params;
	show_params = tmp_show_params != 0;
	int detachstate;
	size_t stacksize;
	void* stackaddr;
	cout << "������� �������� detachstate (0 - �� ���������): ";
	cin >> detachstate;
	cout << "������� �������� stackaddr (0 - �� ���������): ";
	cin >> stacksize;
	cout << "������� �������� stacksize (0 - �� ���������): ";
	cin >> stackaddr;
	// �������������� ���������� ������� � ����������
	pthread_t* threads = new pthread_t[threads_number];
	thread_params* params = new thread_params[threads_number];
	auto start = std::chrono::steady_clock::now();
	pthread_attr_t threads_attr;
	if ((err = pthread_attr_init(&threads_attr)) != 0) {
		cout << "�� ���������� ���������������� ���������: " << strerror(err) << endl;
		exit(-1);
	}
	if (stacksize != 0) {
		err = pthread_attr_setstacksize(&threads_attr, stacksize);
		if (err != 0) {
			cout << "Setting stack size attribute failed: " << strerror(err)
				<< endl;
			//exit(-1);
		}
	}
	if (stackaddr != 0) {
		err = pthread_attr_setstackaddr(&threads_attr, stackaddr);
		if (err != 0) {
			cout << "Setting stack addr attribute failed: " << strerror(err)
				<< endl;
			//exit(-1);
		}
	}
	if (detachstate != 0) {
		err = pthread_attr_setdetachstate(&threads_attr, detachstate);
		if (err != 0) {
			cout << "Setting detachstate attribute failed: " << strerror(err)
				<< endl;
			//exit(-1);
		}
	}

	// ������ ��������-��������, ��� �������� ����� ����������� ������
	size_t vector_size = 1000;
	// ������������� ��������-��������
	vector<int>* vectors = new vector<int>[threads_number];
	for (size_t n = 0; n < threads_number; n++){
		vector<int> vector_item(vector_size);
		for (size_t i = 0; i < vector_size; i++)
		{
			vector_item[i] = rand() % vector_size;
		} 
		vectors[n] = vector_item;
	}

	for (size_t n = 0; n < threads_number; n++) {
		// ������������� ����������
		params[n].show_params = show_params;
		params[n].reduce = BubbleSort<int>;
		params[n].item = &(vectors[n]);
		params[n].thread_number = n;
		// �������� ������
		err = pthread_create(&threads[n], &threads_attr, thread_job, &params[n]);
		// ���� ��� �������� ������ ��������� ������, �������
		// ��������� �� ������ � ���������� ������ ���������
		if (err != 0) {
			cout << "�� ���������� ������� �����: " << strerror(err) << endl;
			exit(-1);
		}
	}
	pthread_attr_destroy(&threads_attr);

	// �������� �����, �� ������� ����� ������� ������
	auto end1 = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed1 = end1 - start;
	cout << "��� ������ ���� �������� �� " << elapsed1.count() << " seconds." << endl;

	// ������� ���������� �������
	for (size_t n = 0; n < threads_number; n++) {
		pthread_join(threads[n], NULL);
	}

	// �������� �����, �� ������ ����� ��������� � �������������� ����������������
	auto end2 = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed2 = end2 - start;
	cout << "��� ������ ��������� ���� ������ �� " << elapsed2.count() << " seconds." << endl;

	// �������� �����, �� ������ ����� ��������� � ����� �������� ������
	auto start_single = std::chrono::steady_clock::now();
	for (size_t n = 0; n < threads_number; n++) {
		vector<int> vector_item(vector_size);
		for (size_t i = 0; i < vector_size; i++)
		{
			vector_item[i] = rand() % vector_size;
		}
		BubbleSort<int>(&vector_item);
	};
	auto end_single = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_single = end_single - start_single;
	cout << "�������� ����� �������� ������ �� " << elapsed_single.count() << " seconds." << endl;
}