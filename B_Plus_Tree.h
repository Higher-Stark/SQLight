#pragma once
#ifndef _B_PLUS_TREE_H_
#define _B_PLUS_TREE_H_

#include <vector>
#include <string>
#include <stdexcept>
#include <utility>

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

	bptNode* borrow_left(bptNode* &left);
	bptNode* merge_left(bptNode* & left);
	
	bptNode* borrow_right(bptNode* &right);
	bptNode* merge_right(bptNode* & right);
	// ---------------------------------------
	size_t find_pos(const std::string&);
	size_t find_pos(const std::string&)const;

	void del(const size_t& pos);

public:
	bptNode(const bool& isleaf = true, bptNode* p = nullptr,const int& size = 3);
	bptNode(const bool& isleaf = true, const int& size = 3);
	bptNode(const bptNode&);
	bptNode& operator=(const bptNode&);
	~bptNode();

	void set_parent(bptNode* p);
	void set_max_size(const int &);

	std::string leftest();

	//std::string predecessor(const std::string&);
	std::string successor(const std::string&);

	bool find(const std::string& key) const;
	// can be modified
	std::string& search(const std::string& key);

	bptNode* insert(const std::string& key, const std::string& value);

	bptNode* del(const std::string& key);
};

#endif