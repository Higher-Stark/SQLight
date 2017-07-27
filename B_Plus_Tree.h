#pragma once
#ifndef _B_PLUS_TREE_H_
#define _B_PLUS_TREE_H_

#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <cassert>

typedef std::string Value;
typedef std::string Key;

class bptNode
{
private:
	int maxSize;
	bool isLeaf;
	bptNode* parent;
	std::vector<std::string> keys;
	std::vector<bptNode*> children;
	std::vector<std::string> values;

	bptNode* split();
	// ---------------------------------------
	bptNode* bm();

	bptNode* borrow_left(bptNode* &left, const size_t& parent_pos);
	bptNode* merge_left(bptNode* & left, const size_t& parent_pos);
	
	bptNode* borrow_right(bptNode* &right, const size_t& parent_pos);
	bptNode* merge_right(bptNode* & right, const size_t& parent_pos);
	// ---------------------------------------
	size_t find_pos(const std::string&);
	size_t find_pos(const std::string&)const;

	void remove(const size_t& pos);

public:
	bptNode(const bool& isleaf = true, bptNode* p = nullptr,const int& size = 3);
	bptNode(const bool& isleaf = true, const int& size = 3);
	bptNode(const bptNode&);
	bptNode& operator=(const bptNode&);
	~bptNode();

	void set_parent(bptNode* p);
	void set_max_size(const int &);

	Key leftest();

	//std::string predecessor(const std::string&);
	Key successor(const Key&);

	bool find(const Key& key) const;
	// can be modified
	// return  > value <
	Value& search(const Key& key);

	bptNode* insert(const Key& key, const Value& value);

	bptNode* del(const Key& key);

	bool empty();

	bptNode* only_child();

	std::string traverse();

	friend bptNode* inherit(bptNode* &_root);
};

#endif