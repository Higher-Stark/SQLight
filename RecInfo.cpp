#include "RecInfo.h"

using namespace std;

recinfo::recinfo() :occupied(true), key("########"), sep(':'), dataOff(0), dataLen(0)
{}

recinfo::recinfo(const recinfo& other) : occupied(other.occupied), key(other.key), sep(other.sep),
dataOff(other.dataOff), dataLen(other.dataLen)
{}

recinfo& recinfo::operator=(const recinfo& rhs)
{
	occupied = rhs.occupied;
	key = rhs.key;
	sep = rhs.sep;
	dataOff = rhs.dataOff;
	dataLen = rhs.dataLen;
	return *this;
}

// --------------------------

recinfo::recinfo(const string& s)
{
	istringstream iss(s);
	iss >> occupied >> key >> sep >> dataOff >> dataLen;
}

recinfo::recinfo(const bool& occ, const string& _key, const char& _sep, const streamoff& _dataOff, 
	const int& _dataLen)
	:occupied(occ), key(_key), sep(_sep), dataOff(_dataOff), dataLen(_dataLen) {}

recinfo::recinfo(const string& _key, const streamoff& _dataOff, const int& _dataLen)
	: occupied(true), key(_key), sep(':'), dataOff(_dataOff), dataLen(_dataLen)
{}

// ---------------------------

void recinfo::set(const bool& occ, const std::string& _key, const std::streamoff& _dataOff, const int& _dataLen)
{
	occupied = occ;
	key = _key;
	dataOff = _dataOff;
	dataLen = _dataLen;
}

void recinfo::set_occupied(const bool& occ)
{
	occupied = occ;
}

void recinfo::set_key(const string& _key)
{
	key = _key;
}

void recinfo::set_data_off(const streamoff& _dataOff)
{
	dataOff = _dataOff;
}

void recinfo::set_data_len(const int& _dataLen)
{
	dataLen = _dataLen;
}

bool recinfo::get_occupied()
{
	return occupied;
}

const std::string& recinfo::get_key()
{
	return key;
}

streamoff recinfo::get_data_off()
{
	return dataOff;
}

int recinfo::get_data_len()
{
	return dataLen;
}

string recinfo::str()
{
	ostringstream oss;
	oss << occupied << "\n" << key << "\n" << sep << "\n" << dataOff << "\n" << dataLen;
	return oss.str();
}

recinfo::~recinfo()
{}