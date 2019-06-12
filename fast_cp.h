#include <fstream>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <algorithm>
#include <queue>
#include <thread>

using namespace std;
#ifndef FAST_COPY_FAST_CP_H
#define FAST_COPY_FAST_CP_H


class CVFile {
public:
	CVFile(size_t buffer_size, int thread, std::string &in_file, std::string &out_file) {                     //���뻺���С�Ĺ��캯��
		buffer_size_ = buffer_size;
		thread_ = thread;
		buf_size_ = buffer_size_ / thread;
		in_file_ = in_file;
		out_file_ = out_file;
	}
	explicit CVFile(int thread, std::string &in_file, std::string &out_file) {                               //�����뻺���С�Ĺ��캯��
		thread_ = thread;
		buf_size_ = buffer_size_ / thread;
		in_file_ = in_file;
		out_file_ = out_file;
	}

	size_t buffer_size_;
	int thread_;
	size_t buf_size_ = 0;                              //���δ����������

	std::string in_file_;
	long long file_size_;
	std::string out_file_;

	void Read(int i, int j) {                                  //��������
		in_.seekg(i*buffer_size_ + j*buf_size_, std::ios::beg);
		std::vector<char > buf(buf_size_);
		in_.read(&buf[0], buf.size());           //�����ݶ���buf��
		buf.push_back(char(j));
		buffers.push_back(buf);                     //��buf����buffers�У��Ա�write�������д���
	}

	void ReadRest(int i, int j) {                                  //����ʣ������
		in_.seekg(i*buffer_size_ + j*buf_size_, std::ios::beg);
		auto x = in_.tellg();
		std::vector<char > buf(file_size_ - x);
		in_.read(&buf[0], buf.size());           //�����ݶ���buf��
		buf.push_back(char(j));
		buffers.push_back(buf);                     //��buf����buffers�У��Ա�write�������д���
	}

	void Write() {
		while (!buffers.empty())
		{
			std::vector<char > buf = buffers.front();             //ȡ����ǰ������������������		                       
			out_.write(&buf[0], buf.size() - 1);
			out_.flush();
			buffers.erase(buffers.begin());
			buf.clear();              //�ͷ��ڴ�
		}
	}

	void Prepare() {                  //���û�����ת��Ϊ�ļ���
		in_.open(in_file_, std::ios::in | std::ios::binary);
		if (!in_.is_open()) {
			std::cout << "cannot open copy file!" << std::endl;
		}
		out_.open(out_file_, std::ios::out | std::ios::binary);
		file_size_ = in_.seekg(0, std::ios::end).tellg();
		in_.seekg(0, std::ios::beg);
	}
	void End() {
		in_.close();
		out_.close();
	}
	~CVFile() {}

private:
	std::ifstream in_;
	std::ofstream out_;
	//std::queue<std::vector<char>> buffers;
	vector<vector<char> > buffers;

};

void run_fast_cp(size_t buffer_size, int thread, std::string &in_file, std::string &out_file) {
	CVFile cvFile(buffer_size, thread, in_file, out_file);          //��ʼ��
	cvFile.Prepare();
	std::vector<std::thread> workers;
	workers.reserve(thread);
	int deal_num = cvFile.file_size_ / cvFile.buffer_size_ + 1;
	cout << cvFile.file_size_ << endl;
	cout << cvFile.buffer_size_ << endl;
	cout << deal_num << endl;
	if (deal_num > 1) {
		for (int i = 0; i < deal_num - 1; i++)
		{
			for (int j = 0; j< thread; j++) {
				workers.emplace_back(std::thread(&CVFile::Read, &cvFile, i, j));
			}
			for (auto && worker : workers)
			{
				worker.join();
			}
			cvFile.Write();
			workers.clear();
		}
	}

	long long rest_len = cvFile.file_size_ - (deal_num - 1)*cvFile.buffer_size_;
	int deal_rest_num = rest_len / cvFile.buf_size_ + 1;
	//cout << deal_rest_num << endl;

	if (deal_rest_num > 1) {
		for (int j = 0; j< deal_rest_num - 1; j++) {
			workers.emplace_back(std::thread(&CVFile::Read, &cvFile, deal_num - 1, j));
		}
	}
	workers.emplace_back(std::thread(&CVFile::ReadRest, &cvFile, deal_num - 1, deal_rest_num - 1));
	for (auto && worker : workers)
	{
		worker.join();
	}
	cvFile.Write();
	workers.clear();



	cvFile.End();
}

#endif //FAST_COPY_FAST_CP_H
#pragma once
