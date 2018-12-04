/*
 * LRUCacheIndex.h
 *
 *  Created on: Dec 4, 2018
 *      Author: weizy
 */

#ifndef TOOLS_HEAD_LRUCACHEINDEX_H_
#define TOOLS_HEAD_LRUCACHEINDEX_H_

#include "lru.h"

#define CACHE_SIZE 10

template<typename key, typename value>
class BPlusTree;

template<typename key, typename value>
class TreeNode;

template<typename key, typename value>
class LRUCacheIndex {
public:
	static LRUCacheIndex * getLruInst();
	static void releaseLruInst();

	LruCache<unsigned long int, TreeNode<key, value>*> * getLruCache();
private:
	LRUCacheIndex();					//单例模式
	static LRUCacheIndex * lruInst;

	LruCache<unsigned long int, TreeNode<key, value>*> * lruCache;
public:
	virtual ~LRUCacheIndex();
};

//==============================================
//==============================================
template<typename key, typename value>
LRUCacheIndex<key, value> * LRUCacheIndex<key, value>::lruInst = nullptr;


template<typename key, typename value>
LRUCacheIndex<key, value>::LRUCacheIndex() {

	lruCache = new LruCache<unsigned long int, TreeNode<key, value>*>(CACHE_SIZE);

}

template<typename key, typename value>
LRUCacheIndex<key, value>::~LRUCacheIndex() {
	delete lruCache;
}

template<typename key, typename value>
LRUCacheIndex<key, value> * LRUCacheIndex<key, value>::getLruInst() {
	if (lruInst == nullptr) {
		lruInst = new LRUCacheIndex<key, value>();
	}
	return lruInst;
}

template<typename key, typename value>
void LRUCacheIndex<key, value>::releaseLruInst() {
	delete lruInst;
}

template<typename key, typename value>
LruCache<unsigned long int, TreeNode<key, value>*> * LRUCacheIndex<key, value>::getLruCache() {
	return lruCache;
}



#endif /* TOOLS_HEAD_LRUCACHEINDEX_H_ */
