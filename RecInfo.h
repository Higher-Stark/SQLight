#pragma once
#ifndef _RECINFO_H_
#define _RECINFO_H_

#include <string>
#include <iostream>
#include <sstream>

class recinfo
{
private:
	bool occupied;
	std::string key;
	char sep;
	std::streamoff dataOff;
	// char sep
	int dataLen;
	// line break
	const char lb = '\n';

public:
	recinfo();
	recinfo(const recinfo&);
	recinfo& operator=(const recinfo&);

	recinfo(const std::string& s);
	recinfo(const bool& occ, const std::string& _key, const char& _sep, const std::streamoff& _dataOff, const int& _dataLen);
	recinfo(const std::string& _key, const std::streamoff& _dataOff, const int& _dataLen);

	void set(const bool& occ, const std::string& _key, const std::streamoff& _dataOff, const int& _dataLen);
	void set_occupied(const bool& occ);
	void set_key(const std::string& _key);
	void set_data_off(const std::streamoff& _dataOff);
	void set_data_len(const int& _dataLen);

	bool get_occupied();
	const std::string& get_key();
	std::streamoff get_data_off();
	int get_data_len();

	std::string str();

	~recinfo();
};

#endif