#include "database.h"

using namespace std;

DB::DB() :path(""), root(new bptNode(true, nullptr, 3))
{}

DB::DB(const string& _path, const int& size) : path(_path), 
fidx(_path + ".idx", ios::in|ios::out|ios::binary),
fdat(_path + ".dat", ios::in|ios::out|ios::binary), root(new bptNode(true, nullptr, size))
{
	if (!fidx || !fidx.is_open() || !fdat || !fdat.is_open()) {
		throw runtime_error("Database open failed.");
	}
	if (size < 3) {
		root->set_max_size(3);
		throw runtime_error("B+ tree node maxSize too small.");
	}
	read_index();
}

DB& DB::open(const string& _path, const int& mode)
{
	path = _path;
	if (fidx.is_open() || fdat.is_open()) {
		close();
	}
	switch (mode) {
	case 0:
		fidx.open(path + ".idx", ios::in | ios::out | ios::binary);
		fdat.open(path + ".dat", ios::in | ios::out | ios::binary);
		if (!fidx || !fidx.is_open() || !fdat || !fdat.is_open()) {
			throw runtime_error("Database doesn't exist");
		}
		root = new bptNode(true, nullptr, 3);
		read_index();
		break;
	case 1:{
		fidx.open(path + ".idx", ios::in);
		fdat.open(path + ".dat", ios::in);
		if (fidx || fdat) {
			throw runtime_error("Database exist, exist");
		}
		fidx.close();
		fdat.close();
		fidx.open(path + ".idx", ios::out | ios::binary);
		fdat.open(path + ".dat", ios::out | ios::binary);
		fidx.close();
		fdat.close();
		fidx.open(path + ".idx", ios::in | ios::out | ios::binary);
		fdat.open(path + ".dat", ios::in | ios::out | ios::binary);
		if (!fidx || !fidx.is_open() || !fdat || !fdat.is_open()) {
			fidx.close();
			fdat.close();
			throw runtime_error("Database doesn't exist");
		}
		streamoff offset = 0;
		fidx.write((char*)&offset, sizeof(streamoff));
		root = new bptNode(true, nullptr, 3);
		break;
	}
	case 2:
		fidx.open(path + ".idx", ios::in | ios::out | ios::trunc | ios::binary);
		fdat.open(path + ".dat", ios::in | ios::out | ios::trunc | ios::binary);
		if (!fidx || !fidx.is_open() || !fdat || !fdat.is_open()) {
			fidx.close();
			fdat.close();
			throw runtime_error("Database doesn't exist");
		}
		root = new bptNode(true, nullptr, 3);
		break;
	default:
		cout << "Undefined action" << endl;
	}
	return *this;
}

bool DB::close()
{
	/* temparory implement */
	if (fidx.is_open() && fdat.is_open()) {
		fidx.close();
		fdat.close();
	}
	else {
		//cout << "Database error. Close failed." << endl;
		return false;
	}
	delete root;
	root = nullptr;
	sparePool.clear();
	return true;
}

const Data DB::fetch(const Key& key)
{
	if (!fidx || !fidx.is_open() || !fdat || !fdat.is_open()) {
		throw runtime_error("Open a Database first.");
	}
	if (root->find(key)) {
		string res = root->search(key);
		recinfo rec(res);
		fdat.seekg(rec.get_data_off(), ios::beg);
		char* ret_str = new char[rec.get_data_len() + 1];
		fdat.read(ret_str, sizeof(char) * rec.get_data_len());
		string ret(ret_str);
		delete ret_str;
		return ret;
	}
	else {
		return "";
	}
}

bool DB::store(const Key& key, const Data& data)
{
	if (!fidx || !fidx.is_open() || !fdat || !fdat.is_open()) {
		throw runtime_error("Open a Database first.");
	}
	if (root->find(key)) replace(key, data);
	if (sparePool.find(key.size()+sizeof(char)) == sparePool.end()) {
		if (sparePool.upper_bound(key.size()+sizeof(char)) == sparePool.end()) {
			fdat.seekp(0, ios::end);
			fdat.write(data.c_str(), sizeof(char) * data.size());
			fdat.write((char*)&LB, sizeof(char));
			streamoff dataOff = fdat.tellp();
			fidx.seekp(0, ios::end);
			streamoff idxOff = fidx.tellp();
			recinfo rec(true, key, idxOff, dataOff, data.size(), 0, ':');
			IndexOff diff = sizeof(streamoff) + sizeof(char);
			fidx.seekp(-diff, ios::cur);
			streamoff relaOff = idxOff - fidx.tellp();
			fidx.write((char*)&relaOff, sizeof(streamoff));
			fidx.seekp(sizeof(char), ios::cur);
			string idx(rec.str());
			fidx.write(idx.c_str(), sizeof(char) * idx.size());
			root->insert(key, idx);
			return true;
		}
		else {
			mmi spare = sparePool.upper_bound(key.size());
			streamoff idxOff(get<0>(spare->second)), dataOff(get<1>(spare->second));
			if (idxOff == -1) {
				fidx.seekg(0, ios::end);
				idxOff = fidx.tellg();
				IndexOff diff = sizeof(streamoff) + sizeof(char);
				fidx.seekp(diff, ios::end);
				streamoff rela_off = idxOff - fidx.tellp();
				fidx.write((char*)&rela_off, sizeof(streamoff));
				fidx.seekp(sizeof(char), ios::cur);
				recinfo rec(true, key, idxOff, dataOff, data.size(), 0, ':');
				write_binary(fidx, rec);
				root->insert(key, rec.str());
				remove(data, spare);
			}
			else {
				fidx.seekg(idxOff, ios::beg);
				recinfo rec;
				rec = read_binary(fidx);
				rec.set_occupied(true);
				rec.set_data_len(data.size());
				rec.set_key(key);
				fidx.seekp(idxOff, ios::beg);
				write_binary(fidx, rec);
				root->insert(key, rec.str());
				remove(data, spare);
			}
			fdat.seekp(dataOff, ios::beg);
			fdat.write(data.c_str(), sizeof(char) * data.size());
			return true;
		}
	}
	else {
		mmi spare = sparePool.find(key.size());
		streamoff idxOff(get<0>(spare->second)), dataOff(get<1>(spare->second));
		fidx.seekg(idxOff, ios::beg);
		recinfo rec;
		rec = read_binary(fidx);
		rec.set_occupied(true);
		rec.set_key(key);
		fidx.seekp(idxOff, ios::beg);
		write_binary(fidx, rec);
		fdat.seekp(dataOff, ios::beg);
		fdat.write(data.c_str(), sizeof(char) * data.size());
		fdat.write((char*)&LB, sizeof(char));
		root->insert(key, rec.str());
		remove(data, spare);
		return true;
	}
}

bool DB::del(const Key& key)
{
	if (!fidx || !fidx.is_open() || !fdat || !fdat.is_open()) {
		throw runtime_error("Open a Database first.");
	}
	if (root->find(key)) {
		string res = root->search(key);
		recinfo rec(res);
		//   > delete from index tree
		root = root->del(key);
		root = inherit(root);
		//   > overwrite index file
		rec.set_occupied(false);
		fidx.seekp(rec.get_idx_off(), ios::beg);
		write_binary(fidx, rec);
		//   > add spare space to sparePool
		add_spare(rec);
		return true;
	}
	else {
		cout << key << " doesn't exist." << endl;
		return false;
	}
}

const Key DB::rewind()
{
	if (!fidx || !fidx.is_open() || !fdat || !fdat.is_open()) {
		throw runtime_error("Open a Database first.");
	}
	return root->leftest();
}

const Key DB::nextrec(const Key& curr)
{
	if (!fidx || !fidx.is_open() || !fdat || !fdat.is_open()) {
		throw runtime_error("Open a Database first.");
	}
	return root->successor(curr);
}

DB& DB::sync()
{
	if (!fidx || !fidx.is_open() || !fdat || !fdat.is_open()) {
		throw runtime_error("Open a Database first.");
	}
	//cout << "SQLight Sync ..." << endl;
	clock_t beginStamp = clock();
	fidx.close();
	fdat.close();

	/* manage sparePool */

	fidx.open(path + ".idx", ios::in | ios::out | ios::binary);
	fdat.open(path + ".dat", ios::in | ios::out | ios::binary);
	if (!fidx || !fidx.is_open() || !fdat || !fdat.is_open()) {
		fidx.close();
		fdat.close();
		throw runtime_error("Open error");
	}
	clock_t endStamp = clock();
	ostringstream oss;
	cout << "SQLight sync complete!\nCost time: ";
	cout << (endStamp - beginStamp) * 1.0 / CLOCKS_PER_SEC * 1000 << "ms" << endl;
	return *this;
}

DB::~DB()
{
	if (fidx.is_open()) fidx.close();
	if (fdat.is_open()) fdat.close();
	delete root;
}

/* --------------   Private Method    --------------------- */

void DB::remove(const Data& data, mmi& spare)
{
	if (data.size() == spare->first) {
		sparePool.erase(spare);
	}
	else {
		int rest = spare->first - data.size() - sizeof(char);
		streamoff remainDataOff = get<1>(spare->second) + sizeof(data.size()) + sizeof(char);
		sparePool.erase(spare);
		sparePool.insert(pair<int, tuple<IndexOff, DataOff>>(rest, make_tuple(-1, remainDataOff)));
	}
}

void DB::add_spare(const recinfo& rec)
{
	int dataLen = rec.get_data_len()+sizeof(char);
	streamoff idxOff(rec.get_idx_off()), dataOff(rec.get_data_off());
	sparePool.insert(pair<int, tuple<IndexOff, DataOff>>(dataLen, make_tuple(idxOff, dataOff)));
}

void DB::read_index()
{
	if (!fidx || !fidx.is_open() || !fdat || !fdat.is_open()) {
		throw runtime_error("Open a Database first.");
	}
	
	fidx.seekg(0, ios::beg);
	streamoff firstIdxOff;
	fidx.read((char*)&firstIdxOff, sizeof(streamoff));
	if (firstIdxOff == 0) return;
	fidx.seekp(firstIdxOff, ios::beg);
	recinfo rec;
	//   > read index
	rec = read_binary(fidx);
	while (rec.get_nextrec() != 0) {
		if (rec.get_occupied()) {
			if (root->find(rec.get_key())) throw runtime_error("Same key exists");
			root->insert(rec.get_key(), rec.str());
		}
		else {
			add_spare(rec);
		}
		rec = read_binary(fidx);
	}
	// done
}

Data DB::read_data(const recinfo& rec)
{
	if (!fidx || !fidx.is_open() || !fdat || !fdat.is_open()) {
		throw runtime_error("Open a Database first.");
	}

	fdat.seekg(rec.get_data_off(), ios::beg);
	char* data = new char[rec.get_data_len() + 1];
	fdat.read(data, sizeof(char) * rec.get_data_len());
	char lb;
	fdat.read((char*)&lb, sizeof(char));
	if (lb != LB) {
		delete data;
		throw runtime_error("Data error");
	}
	string ret(data);
	delete data;
	return ret;
}

void DB::write_data(const Data& data)
{
	if (!fidx || !fidx.is_open() || !fdat || !fdat.is_open()) {
		throw runtime_error("Open a Database first.");
	}

	fdat.write(data.c_str(), sizeof(char) * data.size());
	fdat.write((char*)&LB, sizeof(char));
}

void DB::write_data(const Data& data, const DataOff& off)
{
	if (!fidx || !fidx.is_open() || !fdat || !fdat.is_open()) {
		throw runtime_error("Open a Database first.");
	}

	fdat.seekp(off, ios::beg);
	fdat.write(data.c_str(), sizeof(char) * data.size());
	fdat.write((char*)&LB, sizeof(char));
}

void DB::rewrite()
{
	if (!fidx || !fidx.is_open() || !fdat || !fdat.is_open()) {
		throw runtime_error("Open a Database first.");
	}

	map<Key, Data> key_data;
	KV res = root->leftest(1);
	while (get<0>(res) != "") {
		recinfo rec(get<1>(res));
		key_data.insert(pair<Key, Data>(get<0>(res), read_data(rec)));
		res = root->successor(get<0>(res), 1);
	}
	fdat.close();
	fdat.open(path + ".dat", ios::trunc|ios::binary);
	fdat.close();
	fdat.open(path + ".dat", ios::in | ios::out | ios::binary);

	fidx.close();
	fidx.open(path + "idx", ios::trunc | ios::binary);
	fidx.close();
	fidx.open(path + ".idx", ios::in | ios::out | ios::binary);
	if (key_data.empty()) {
		IndexOff off = 0;
		fidx.write((char*)&off, sizeof(streamoff));
		sparePool.clear();
		return;
	}
	IndexOff curIdxOff = sizeof(streamoff);
	fidx.write((char*)&curIdxOff, sizeof(IndexOff));
	//   > rewrite data file
	string key;
	key = root->leftest();

	Value& v = root->search(key);
	DataOff curDataOff = fdat.tellp();
	write_data(key_data[key]);
	curIdxOff = fidx.tellp();
	recinfo rec(true, key, curIdxOff, curDataOff, key_data[key].size(), 0, ':');
	v = rec.str();
	write_binary(fidx, rec);

	key = root->successor(key);

	while (key != "") {
		v = root->search(key);
		curDataOff = fdat.tellp();
		write_data(key_data[key]);
		curIdxOff = fidx.tellp();
		rec.set(true, key, curIdxOff, curDataOff, key_data[key].size(), 0);
		v = rec.str();
		IndexOff diff = sizeof(streamoff) + sizeof(char);
		fidx.seekp(diff, ios::cur);
		diff = curIdxOff - fidx.tellp();
		fidx.write((char*)&diff, sizeof(streamoff));
		fidx.seekp(sizeof(char), ios::cur);
		write_binary(fidx, rec);
		key = root->successor(key);
	}
	sparePool.clear();
}

bool DB::replace(const Key& key, const Data& data)
{
	if (!fidx || !fidx.is_open() || !fdat || !fdat.is_open()) {
		throw runtime_error("Open a Database first.");
	}

	Value& v = root->search(key);
	recinfo rec(v);
	if (rec.get_data_len() == data.size()) {
		fdat.seekp(rec.get_data_off());
		write_data(data);
		return true;
	}
	else if (rec.get_data_len() > data.size()) {
		int rest = rec.get_data_len() - data.size() + sizeof(char);
		sparePool.insert(pair<int, tuple<IndexOff, DataOff>>(rest,
			make_tuple(-1, rec.get_data_off() + sizeof(char) * data.size() + sizeof(char))));
		rec.set_data_len(data.size());
		fidx.seekp(rec.get_idx_off(), ios::beg);
		write_binary(fidx, rec);
		v = rec.str();
		return true;
	}
	else {
		del(key);
		store(key, data);
		return true;
	}
}