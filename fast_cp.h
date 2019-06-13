#include <fstream>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>

using namespace std;
#ifndef FAST_COPY_FAST_CP_H
#define FAST_COPY_FAST_CP_H


class CVFile {
public:
	CVFile(size_t buffer_size, int thread, std::string &in_file, std::string &out_file) {                     //传入缓存大小的构造函数
		buffer_size_ = buffer_size;
		thread_ = thread;
		buf_size_ = buffer_size_ / thread;
		in_file_ = in_file;
		out_file_ = out_file;
	}
	explicit CVFile(int thread, std::string &in_file, std::string &out_file) {                               //不传入缓存大小的构造函数
		thread_ = thread;
		buf_size_ = buffer_size_ / thread;
		in_file_ = in_file;
		out_file_ = out_file;
	}

	size_t buffer_size_;
	int thread_;
	size_t buf_size_;                              //单次处理的数据量

	std::string in_file_;
	long long file_size_;
	std::string out_file_;

	void Read(vector<char> buf, int i, int j) {                                  //读入数据
		in_.seekg(i*buffer_size_ + j*buf_size_, std::ios::beg);
		in_.read(&buf[0], buf.size());           //将数据读入buf中   
	}

	void ReadRest(vector<char> buf, int i, int j) {                                  //读入剩余数据
		in_.seekg(i*buffer_size_ + j*buf_size_, std::ios::beg);
		auto x = in_.tellg();
		buf.resize(file_size_ - x);
		in_.read(&buf[0], buf.size());           //将数据读入buf中
	}

	void Write(vector<char> buf) {	                       
		out_.write(&buf[0], buf.size());
		out_.flush();
		buf.clear();         
	}

	void Prepare() {                  //将用户输入转化为文件流
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
	//vector<vector<char> > buffers;

};

void run_fast_cp(size_t buffer_size, int thread, std::string &in_file, std::string &out_file) {
	CVFile cvFile(buffer_size, thread, in_file, out_file);          //初始化
	cvFile.Prepare();
	std::vector<std::thread> workers;
	workers.reserve(thread);
	int deal_num = cvFile.file_size_ / cvFile.buffer_size_ + 1;
	vector<vector<char> > buffers;
	buffers.resize(thread);
	for (int i = 0; i < thread; i++)
	{
		buffers[i].resize(cvFile.buf_size_);
	}

	cout << cvFile.file_size_ << endl;
	cout << cvFile.buffer_size_ << endl;
	cout << deal_num << endl;
	if (deal_num > 1) {
		for (int i = 0; i < deal_num - 1; i++)
		{
			for (int j = 0; j< thread; j++) {
				workers.emplace_back(std::thread(&CVFile::Read, &cvFile, buffers[j], i, j));
			}
			for (auto && worker : workers)
			{
				worker.join();
			}
			for (int m = 0; m < thread; m++)
			{
				cvFile.Write(buffers[m]);
			}
			workers.clear();
		}
	}

	long long rest_len = cvFile.file_size_ - (deal_num - 1)*cvFile.buffer_size_;
	int deal_rest_num = rest_len / cvFile.buf_size_ + 1;
	//cout << deal_rest_num << endl;

	if (deal_rest_num > 1) {
		for (int j = 0; j< deal_rest_num - 1; j++) {
			workers.emplace_back(std::thread(&CVFile::Read, &cvFile, buffers[j], deal_num - 1, j));
		}
	}
	workers.emplace_back(std::thread(&CVFile::ReadRest, &cvFile, buffers[deal_rest_num - 1], deal_num - 1, deal_rest_num - 1));
	for (auto && worker : workers)
	{
		worker.join();
	}
	for (int m = 0; m < deal_rest_num; m++)
	{
		cvFile.Write(buffers[m]);
	}
	workers.clear();



	cvFile.End();
}

#endif //FAST_COPY_FAST_CP_H
#pragma once
