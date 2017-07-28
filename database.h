#pragma once
#ifndef _DATABASE_H_
#define _DATABASE_H_

#include "RecInfo.h"
#include "B_Plus_Tree.h"
#include <map>
#include <ctime>
#include <tuple>

typedef std::string Data;
typedef std::streamoff IndexOff;
typedef std::streamoff DataOff;
typedef std::multimap<int, std::tuple<IndexOff, DataOff>>::iterator mmi;

class DB
{
private:
	std::string path;
	std::fstream fidx, fdat;
	bptNode* root;
	std::multimap<int, std::tuple<IndexOff, DataOff>> sparePool;

	void read_index();
	//void write_index(const recinfo& rec, const IndexOff& idxOff = 0);
	Data read_data(const recinfo& rec);
	void write_data(const Data& data);
	void write_data(const Data& data, const DataOff& off);
	void rewrite();

	void remove(const Data& data, mmi& spare);
	void add_spare(const recinfo&);

	bool replace(const Key& key, const Data& data);

public:
	DB();
	DB(const std::string& _path, const int& size = 3);
	DB(const DB&) = delete;
	DB& operator=(const DB&) = delete;

	/*
	 * DB& open(const std::string& _path, const int& mode = 0)
	 * mode = 0   open database
	 * mode = 1   create database
	 * mode = 2   truncate database
	 */
	DB& open(const std::string& _path, const int& mode = 0);
	bool close();

	/*
	 * const std::string fetch(const Key& key)
	 * return data
	 */
	const Data fetch(const Key& key);

	/*
	 * bool store(const Key& key, const Data& data)
	 * store data into data file
	 * return true if succeed
	 */
	bool store(const Key& key, const Data& data);

	/*
	 * bool del(const Key& key)
	 * del key from index file and data file
	 * return true if succeed
	 */
	bool del(const Key& key);

	/*
	 * const Data rewind()
	 * return first piece of index
	 */
	const Key rewind();

	/*
	 * const Key nextrec()
	 * return next record's key
	 */
	const Key nextrec(const Key&);

	/*
	 * DB& sync()
	 * sync cache and file on disk
	 * return time cost to sync the database
	 */
	DB& sync();

	~DB();
};

#endif