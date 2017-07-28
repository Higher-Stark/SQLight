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
	iss >> occupied >> key >> sep >> idxOff >> dataOff >> dataLen >> nextrec;
}

recinfo::recinfo(const bool& occ, const string& _key, const streamoff& _idxOff, const streamoff& _dataOff, 
	const int& _dataLen, const streamoff& _nextrec, const char& _sep)
	:occupied(occ), key(_key), idxOff(_idxOff), dataOff(_dataOff), dataLen(_dataLen), nextrec(_nextrec), sep(_sep) {}

recinfo::recinfo(const string& _key, const streamoff& _idxOff, const streamoff& _dataOff, const int& _dataLen, const streamoff& _nextrec)
	: occupied(true), key(_key), idxOff(_idxOff), dataOff(_dataOff), dataLen(_dataLen), nextrec(_nextrec), sep(':')
{}

// ---------------------------

void recinfo::set(const bool& occ, const std::string& _key, const streamoff& _idxOff, const std::streamoff& _dataOff, const int& _dataLen, const streamoff& _nextrec)
{
	occupied = occ;
	key = _key;
	idxOff = _idxOff;
	dataOff = _dataOff;
	dataLen = _dataLen;
	nextrec = _nextrec;
}

void recinfo::set_occupied(const bool& occ)
{
	occupied = occ;
}

void recinfo::set_key(const string& _key)
{
	key = _key;
}

void recinfo::set_idx_off(const streamoff& _idxOff)
{
	idxOff = _idxOff;
}

void recinfo::set_data_off(const streamoff& _dataOff)
{
	dataOff = _dataOff;
}

void recinfo::set_data_len(const int& _dataLen)
{
	dataLen = _dataLen;
}

void recinfo::set_nextrec(const streamoff& _nextrec)
{
	nextrec = _nextrec;
}

bool recinfo::get_occupied()const
{
	return occupied;
}

const std::string recinfo::get_key()const
{
	return key;
}

const streamoff recinfo::get_idx_off()const
{
	return idxOff;
}

streamoff recinfo::get_data_off()const
{
	return dataOff;
}

int recinfo::get_data_len()const
{
	return dataLen;
}

const streamoff recinfo::get_nextrec()const
{
	return nextrec;
}

recinfo read_binary(fstream& fidx)
{
	streamoff currOff = fidx.tellg();

	//   > read occupied
	bool occ;
	fidx.read((char*)&occ, sizeof(bool));

	//   > read seperator
	char sep;
	fidx.read((char*)&sep, sizeof(char));

	//   >read key length
	int keyLen;
	fidx.read((char*)&keyLen, sizeof(int));
	fidx.read((char*)&sep, sizeof(char));

	//   > read key
	char* key = new char[keyLen + 1];
	fidx.read(key, sizeof(char)*keyLen);
	key[keyLen] = '\0';
	fidx.read((char*)&sep, sizeof(char));

	//   > read data offset
	streamoff datOff;
	fidx.read((char*)&datOff, sizeof(streamoff));
	fidx.read((char*)&sep, sizeof(char));

	//   > read data length
	int datLen;
	fidx.read((char*)&datLen, sizeof(int));
	fidx.read((char*)&sep, sizeof(char));

	//   > read next record
	streamoff nextrec;
	fidx.read((char*)&nextrec, sizeof(streamoff));

	//   > read line break
	char lb;
	fidx.read((char*)&lb, sizeof(char));

	//   > check index validation
	if (lb != LB) throw runtime_error("Index error.");
	return recinfo(occ, key, currOff, datOff, datLen, nextrec, sep);
}

fstream& write_binary(fstream& fidx, const recinfo& rec)
{
	//   > write occupied
	fidx.write((char*)&rec.occupied, sizeof(char));
	//   > write sep
	fidx.write((char*)&rec.sep, sizeof(char));
	//   > write key length
	int keyLen = rec.key.size();
	fidx.write((char*)&keyLen, sizeof(int));
	fidx.write((char*)&rec.sep, sizeof(char));
	//   > write key
	fidx.write(rec.key.c_str(), sizeof(char) * 8);
	fidx.write((char*)&rec.sep, sizeof(char));
	//   > write data offset
	fidx.write((char*)&rec.dataOff, sizeof(streamoff));
	fidx.write((char*)&rec.sep, sizeof(char));
	//   > write data length
	fidx.write((char*)&rec.dataLen, sizeof(int));
	fidx.write((char*)&rec.sep, sizeof(char));
	//   > write next record offset
	fidx.write((char*)&rec.nextrec, sizeof(streamoff));
	//   > write line break
	fidx.write((char*)&LB, sizeof(char));
	return fidx;
}

string recinfo::str() const
{
	ostringstream oss;
	oss << occupied << "\n" << key << "\n" << sep << "\n" << idxOff << "\n"
		<< dataOff << "\n" << dataLen << "\n" << nextrec;
	return oss.str();
}

bool recinfo::operator==(const recinfo& rhs) const
{
	bool ret = true;
	ret = ret && (occupied == rhs.get_occupied()) && (key == rhs.get_key())
		&& (idxOff == rhs.get_idx_off())
		&& (dataOff == rhs.get_data_off())
		&& (nextrec == rhs.get_nextrec())
		&& (dataLen == rhs.get_data_len())
		&& (sep == rhs.sep);
	return ret;
}

bool recinfo::operator!=(const recinfo& rhs) const
{
	if (*this == rhs) return false;
	else
		return true;
}

recinfo::~recinfo()
{}