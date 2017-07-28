#pragma once
#ifndef _RECINFO_H_
#define _RECINFO_H_

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

// line break
const char LB = '\n';

class recinfo
{
private:
	bool occupied;
	std::string key;
	std::streamoff idxOff;
	std::streamoff dataOff;
	std::streamoff nextrec;
	int dataLen;
	char sep;

public:
	recinfo();
	recinfo(const recinfo&);
	recinfo& operator=(const recinfo&);

	recinfo(const std::string& s);
	recinfo(const bool& occ, const std::string& _key, const std::streamoff& _idxOff, 
		const std::streamoff& _dataOff, const int& _dataLen, 
		const std::streamoff& _nextrec, const char& _sep);
	recinfo(const std::string& _key, const std::streamoff& _idxOff, 
		const std::streamoff& _dataOff, const int& _dataLen, 
		const std::streamoff& _nextrec);

	void set(const bool& occ, const std::string& _key, const std::streamoff& _idxOff, 
		const std::streamoff& _dataOff,
		const int& _dataLen, const std::streamoff& _nextrec);
	void set_occupied(const bool& occ);
	void set_key(const std::string& _key);
	void set_idx_off(const std::streamoff& _idxOff);
	void set_data_off(const std::streamoff& _dataOff);
	void set_data_len(const int& _dataLen);
	void set_nextrec(const std::streamoff& _nextrec);

	bool get_occupied() const;
	const std::string get_key() const;
	const std::streamoff get_idx_off() const;
	std::streamoff get_data_off() const;
	int get_data_len() const;
	const std::streamoff get_nextrec() const;

	friend recinfo read_binary(std::fstream& _fidx);
	friend std::fstream& write_binary(std::fstream& fidx, const recinfo&);

	bool operator==(const recinfo& rhs) const;
	bool operator!=(const recinfo& rhs) const;

	std::string str() const;

	~recinfo();
};

#endif