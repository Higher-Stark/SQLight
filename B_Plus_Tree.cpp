#include "B_Plus_Tree.h"

using namespace std;

bptNode* bptNode::split()
{
	bptNode* pa = new bptNode(false, maxSize);
	bptNode *lhc = new bptNode(isLeaf, pa, maxSize), *rhc = new bptNode(isLeaf, pa, maxSize);
	//   > split leaf node
	if (isLeaf) {
		for (size_t i = 0; i != keys.size() / 2; ++i) {
			//   > do NOT move key, for future comparing
			lhc->keys.push_back(keys[i]);
			//   > move or not move, either is OK
			lhc->values.push_back(std::move(values[i]));
		}
		for (size_t i = keys.size() / 2; i != keys.size(); ++i) {
			rhc->keys.push_back(keys[i]);
			rhc->values.push_back(std::move(values[i]));
		}

		if (parent != nullptr) {
			size_t parent_pos = parent->find_pos(rhc->keys[0]);
			parent->keys.insert(parent->keys.begin() + parent_pos, rhc->keys[0]);
			//   delete parent->children[parent_pos]  >> delete this
			lhc->set_parent(parent);
			rhc->set_parent(parent);
			parent->children[parent_pos] = rhc;
			parent->children.insert(parent->children.begin() + parent_pos, lhc);
			//pa->children.clear();
			//   > free temparory parent node pointer
			delete pa;
			return parent;
		}
		else {
			pa->keys.push_back(rhc->keys[0]);
			pa->children.push_back(lhc);
			pa->children.push_back(rhc);
			return pa;
		}
	}
	// split nonleaf node
	else {
		for (size_t i = 0; i != keys.size() /2; i++) {
			//   > reserve key for future comparing
			lhc->keys.push_back(keys[i]);
			//   > move pointer
			//   > keep memory from freed
			//   > reset hanging pointer to nullptr
			lhc->children.push_back(std::move(children[i]));
			children[i] = nullptr;
			lhc->children[i]->set_parent(lhc);
		}
		lhc->children.push_back(std::move(children[keys.size() / 2]));
		children[keys.size() / 2] = nullptr;
		lhc->children[keys.size() / 2]->set_parent(lhc);

		for (size_t i = keys.size() / 2 + 1; i != keys.size(); i++) {
			rhc->keys.push_back(keys[i]);
			rhc->children.push_back(std::move(children[i]));
			children[i] = nullptr;
			rhc->children[i - keys.size() / 2 - 1]->set_parent(rhc);
		}
		rhc->children.push_back(std::move(children[keys.size()]));
		children[keys.size()] = nullptr;
		rhc->children[keys.size() - keys.size() / 2 - 1]->set_parent(rhc);
		
		if (parent != nullptr) {
			string& tmp = keys[keys.size() / 2];
			size_t parent_pos = parent->find_pos(tmp);
			parent->keys.insert(parent->keys.begin() + parent_pos, tmp);
			parent->children[parent_pos] = rhc;
			parent->children.insert(parent->children.begin() + parent_pos, lhc);
			delete pa;
			return parent;
		}
		else {
			pa->keys.push_back(keys[keys.size() / 2]);
			pa->children.push_back(lhc);
			pa->children.push_back(rhc);
			return pa;
		}
	}
}

//   doesn't merge directly, first check whether can borrow
bptNode* bptNode::bm()
{
	size_t parent_pos = 0;
	while (parent_pos != parent->children.size() && parent->children[parent_pos] != this) {
		parent_pos++;
	}
	if (parent_pos == 0) {
		bptNode* &right = parent->children[parent_pos + 1];
		if (right->keys.size() > maxSize / 2 + 1) {
			return borrow_right(right);
		}
		else 
			return merge_right(parent->children[parent_pos + 1]);
	}
	else if (parent_pos == parent->children.size() - 1) {
		bptNode* &left = parent->children[parent_pos - 1];
		if (left->keys.size() > maxSize / 2 + 1) {
			return borrow_left(left);
		}
		else
			return merge_left(parent->children[parent_pos - 1]);
	}
	else {
		bptNode* &left = parent->children[parent_pos - 1];
		bptNode* &right = parent->children[parent_pos + 1];
		if (left->keys.size() > right->keys.size()) {
			if (left->keys.size() > maxSize / 2 + 1) {
				return borrow_left(left);
			}
			else
				return merge_left(left);
		}
		else 
			if (right->keys.size() > maxSize / 2 + 1) {
				return borrow_right(right);
			}
			else
				return merge_right(right);
	}
}

/*
 *  >> WARN: change the parent pointer when move pointer in children <<
 */

bptNode* bptNode::borrow_left(bptNode* &left)
{
	/*
	*  >> WARN: change the parent pointer when move pointer in children <<
	*/

	/* TODO */
}

bptNode* bptNode::merge_left(bptNode* &left)
{
	/*
	*  >> WARN: change the parent pointer when move pointer in children <<
	*/
	
	/* TODO */
}

bptNode* bptNode::borrow_right(bptNode* &right)
{
	/*
	*  >> WARN: change the parent pointer when move pointer in children <<
	*/
	
	/* TODO */
}

bptNode* bptNode::merge_right(bptNode* &right)
{
	/*
	*  >> WARN: change the parent pointer when move pointer in children <<
	*/
	
	/* TODO */
}

size_t bptNode::find_pos(const string& key)
{
	size_t i = 0;
	while (i != keys.size() && key > keys[i]) {
		++i;
	}
	return i;
}

size_t bptNode::find_pos(const string& key)const
{
	size_t i = 0;
	while (i != keys.size() && key > keys[i]) {
		++i;
	}
	return i;
}

//   > private method
void bptNode::del(const size_t& pos)
{
	if (!isLeaf) throw runtime_error("this is a nonleaf node. Action refused");
	keys.erase(keys.begin() + pos);
	values.erase(values.begin() + pos);
}

bptNode::bptNode(const bool& isleaf = true, bptNode* p = nullptr, const int& size = 3) 
	: isLeaf(isleaf), parent(p), maxSize(size)
{
	if (maxSize < 3) 
		throw runtime_error("B+Tree max size too small");
}

bptNode::bptNode(const bool& isleaf = true, const int& size = 3)
	: isLeaf(isleaf), parent(nullptr), maxSize(size)
{
	if (maxSize < 3)
		throw runtime_error("B+Tree max size too small");
}

bptNode::bptNode(const bptNode& other) : isLeaf(other.isLeaf), parent(other.parent), 
keys(other.keys), children(other.children), values(other.values), maxSize(3)
{
	if (maxSize < 3)
		throw runtime_error("B+Tree max size too small");
}

bptNode& bptNode::operator=(const bptNode& rhs)
{
	maxSize = rhs.maxSize;
	isLeaf = rhs.isLeaf;
	parent = rhs.parent;
	keys = rhs.keys;
	children = rhs.children;
	values = rhs.values;
	return *this;
}

bptNode::~bptNode()
{
	for (vector<bptNode*>::iterator it = children.begin(); it != children.end(); it++) {
		delete *it;
	}
	parent = nullptr;
}

void bptNode::set_parent(bptNode* p)
{
	parent = p;
}

void bptNode::set_max_size(const int& size)
{
	if (size < 3)
		throw runtime_error("B+Tree max size too small");
	maxSize = size;
}

string bptNode::leftest()
{
	if (isLeaf) {
		if (values.empty()) return "";
		else return *values.begin();
	}
	else {
		return (*children.begin())->leftest();
	}
}

string bptNode::successor(const string& curr)
{
	if (isLeaf) {
		vector<string>::iterator it = keys.begin();
		for (; it != keys.end(); ++it) {
			if (*it == curr) break;
		}
		if (it == keys.end()) {
			throw runtime_error(curr + " Not found.");
		}
		//   > curr is the last element in content
		if (it + 1 == keys.end()) {
			bptNode* curr = this;
			bptNode* up = parent;
			size_t pos = 0;
			//   > if parent doesn't exist
			if (up == nullptr) return "";

			for (; pos != up->children.size(); ++pos) {
				if (up->children[pos] == curr) break;
			}
			while (pos == up->keys.size()) {
				curr = up;
				up = up->parent;
				if (parent == nullptr) return "";
				pos = 0;
				for (; pos != up->children.size(); ++pos) {
					if (up->children[pos] == curr) break;
				}
			}
			return up->keys[pos];
		}
		//   > curr is not the last one
		else {
			return *(it + 1);
		}
	}
	else {
		size_t pos = 0;
		for (; pos != keys.size(); ++pos) {
			if (keys[pos] > curr) break;
		}
		return children[pos]->successor(curr);
	}
}

bool bptNode::find(const std::string& key) const
{
	if (isLeaf) {
		for (vector<string>::const_iterator it = keys.cbegin(); it != keys.cend(); ++it) {
			if (*it == key) return true;
		}
		return false;
	}
	else {
		size_t pos = find_pos(key);
		return children[pos]->find(key);
	}
}

string& bptNode::search(const string& key)
{
	if (isLeaf) {
		for (size_t i = 0; i != keys.size(); i++) {
			if (keys[i] == key) return values[i];
		}
		throw runtime_error(key + " not found");
	}
	else {
		size_t pos = find_pos(key);
		return children[pos]->search(key);
	}
}

bptNode* bptNode::insert(const string& key, const string& value)
{
	if (isLeaf) {
		size_t pos = find_pos(key);
		keys.insert(keys.begin() + pos, key);
		values.insert(values.begin() + pos, value);
		if (keys.size >= maxSize) {
			bptNode* ret = split();
			delete this;
			return ret;
		}
		return this;
	}
	else {
		size_t pos = find_pos(key);
		children[pos]->insert(key, value);
		if (keys.size >= maxSize) {
			bptNode* ret = split();
			delete this;
			return ret;
		}
		return this;
	}
}

bptNode* bptNode::del(const string& key)
{
	if (isLeaf) {
		size_t pos = find_pos(key);
		if (pos <= 0 || pos == keys.size())
			throw runtime_error(key + " not found. Delete failed!");
		pos--;
		if (keys[pos] != key)
			throw runtime_error(key + " not found. Delete failed!");
		del(pos);
		if (parent != nullptr && keys.size() < maxSize / 2) 
			return bm();    // should we delete this ?
		return this;
	}
	else {
		size_t pos = find_pos(key);
		if (pos > 0 && pos <= keys.size()) {
			if (key == keys[pos - 1]) {
				keys[pos - 1] = successor(key);
			}
		}
		bool ret = children[pos]->del(key);
		if (parent != nullptr && keys.size() < maxSize / 2)
			return bm();    // should we delete this ?
		return this;
	}
}