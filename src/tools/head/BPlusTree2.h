/*
 * Bplustree.h
 *
 *  Created on: Dec 11, 2018
 *      Author: weizy
 */

#ifndef TOOLS_HEAD_Bplustree_H_
#define TOOLS_HEAD_Bplustree_H_


#include "BPlusTree2.h"

#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <exception>
#include <typeinfo>
#include <vector>
#include <set>

#include "../../exception/head/KeyNotFoundException.h"
//#include "LRUCacheIndex.h"
#include "lru.h"
#include "../../exception/head/FileNotFoundException.h"
#include "../../head/Global.h"

#define CACHE_SIZE 10

using namespace std;

#define BLOCK_SIZE 				1024			//4*1024
#define TREE_NODE_HEAD_SIZE		16
#define TREE_NODE_DATA_SIZE 	(1024-16)		//(4*1024-16)
#define OFFSET_LENGTH			(2*sizeof(unsigned long int))

template<typename key, typename value>
class Treenode;

template<typename key, typename value>
class Bplustree {
public:
	Bplustree(const char * indexFileName, int keyLen, int valueLen, bool create);
	virtual ~Bplustree();

public:
	void init();					//用于初始化文件中已经存在了的b+树
	void createIndex();				//发布create index后用于初始化b+树
	void put(key k, value v);
	set<value> get(key k);				//应该是unsigned long int的，但是因为找不到的时候返回-1，索引只能是有符号的整数了
									//后期可改为抛出异常进行处理
	void remove(key k, value v);
public:
	void printTree();
public:
	void test(unsigned long int self);
private:
	Treenode<key, value> * getLeafNode(key k);
	Treenode<key, value> * getParent(key k, value childId);
	void split(Treenode<key, value> *);
	void handleDel(Treenode<key, value> * leafNode);
	Treenode<key, value> * getLeftNode(Treenode<key, value> * node, Treenode<key, value> * parent);
	Treenode<key, value> * getLeftNodeAll(Treenode<key, value> * node, Treenode<key, value> * parent);
	Treenode<key, value> * getRightNode(Treenode<key, value> * node, Treenode<key, value> *);
	void borrowLeft(Treenode<key, value> * leftNode, Treenode<key, value> * node, Treenode<key, value> * parent);
	void borrowRight(Treenode<key, value> * rightNode, Treenode<key, value> * node, Treenode<key, value> * parent);
	void mergeLeft(Treenode<key, value> * leftNode, Treenode<key, value> * leafNode, Treenode<key, value> * parent);
	void mergeRight(Treenode<key, value> * rightNode, Treenode<key, value> * leafNode, Treenode<key, value> * parent);
	Treenode<key, value> * getLastTreeNode(unsigned long int totalBlock);
	Treenode<key, value> * getTreeNode(unsigned long int self);
private:
	char * indexFileName;
	int keyLen;
	int valueLen;
	int treeNodeMaxSize;			//结点所能存放数据的做大值
	char * head;					//索引文件头部保存的信息 以下定义的内容
	unsigned long int totalBlock;	//叶节点和内节点的总数
	unsigned long int root;			//根节点所在位置

	//rootNode
	Treenode<key, value> * rootNode;
private:
	Treenode<key, value> * getRootNode();
private:
	LruCache<unsigned long int, Treenode<key, value>*> * lruCache;

};

template<typename key, typename value>
class Treenode {
private:
	//块中的数据域
	char * data;				//包含key和Value  大小BLOCK_SIZE	value为key所在的块号,key的长度应该是固定的
	//-------------------data数据的存放方式-----------------------------------
	//point key| point key|...|point key| point		最后面这个point指向下一个叶节点
	//----------------------------------------------------------------------
	//块头的定义
	unsigned long int self;		//本身在第几块
//	unsigned long int parent;	//该节点的父节点	-1表示根节点
//	unsigned long int next;		//if(type==1) next为下一叶节点 0为内节点的下一节点 -1表示没有下一节点
	int type;					//叶节点： type==1	非叶节点：type==0
	unsigned int count;			//该块数据域中有多少个数据
	int treeNodeMaxSize;		//块最大数据个数

	int keyLen;					//key的长度
	int valueLen;				//value的长度
	const char * indexFileName;

	bool change;				//更改标志
public:
	Treenode(int keyLen, int valueLen, unsigned long int self, int type, const char * indexFileName);
	Treenode(const char * block, int keyLen, int valueLen, const char * indexFileName);
	~Treenode();
public:
	char * getData();
	unsigned long int getSelf();
	void setSelf(unsigned long int self);
	long int getNext();
	void setNext(long int next);
	int getType();
	void setType(int type);
	int getCount();
	void setCount(int count);
	void setChange(bool change);
	bool getChange();
	key getKey(int index);
	value getValue(int index);
public:
	void addData(key k, value v);
	void delData(key k, value v);			//将键值为k的项删除（叶结点）
	key delInnerData(value v);				//将值为v的项删除
	void addInnerData(key, value);

	void addRightNodeData(Treenode<key, value> * rightNode);	//将右边的结点合并到自个身上
	void addLeftNodeData(Treenode<key, value> * leftNode);	//将左边的结点合并到自个身上

	void addRightNodeData_Inner(Treenode<key, value> * rightNode, key k);
	void addLeftNodeData_Inner(Treenode<key, value> * leftNode, key k);

	void addFirstInnerData(value left, key k, value right);

	int binarySearch(key k);
	unsigned long int getNextChild(key k);
	pair<key, value> splitData(Treenode<key, value> * right);		//叶结点分裂  返回右边数据的个数
	pair<key, value> splitInnerData(Treenode<key, value> * right);	//非叶结点分裂
	void moveRight(Treenode<key, value> * leftNode);				//从左边结点移动一位数据到右边
	void moveLeft(Treenode<key, value> * rightNode);				//从右边结点移动一位数据到左边
	void updataKey(key k, value v);									//将值为v的项的索引置为k
	void updataValue(value v0, value v1);							//将v0改成v1
	void copyData(Treenode<key, value> * node);						//将Node的数据复制到自身
private:

public:
	void parsed(const char * block);	//将从磁盘读取的块进行转换
	void writeBack();			//写回磁盘
public:
	void printTreeNode();

};

//============================================
//============================================
template<typename key, typename value>
Bplustree<key, value>::Bplustree(const char * indexFileName, int keyLen, int valueLen, bool create) {
	this->indexFileName = (char*)malloc(Global::INDEX_FILE_PATH_LENGTH);
	strcpy(this->indexFileName, indexFileName);
	this->keyLen = keyLen;
	this->valueLen = valueLen;
	head = (char*)malloc(sizeof(totalBlock) + sizeof(root));
	treeNodeMaxSize = (TREE_NODE_DATA_SIZE - valueLen)/(keyLen + valueLen);
//	cout << "Bplustree::Bplustree() treeNodeMaxSize = " << treeNodeMaxSize << endl;

	lruCache = new LruCache<unsigned long int, Treenode<key, value>*>(CACHE_SIZE);

	if (create) {
		createIndex();
	} else {
		init();
	}

}
template<typename key, typename value>
Bplustree<key, value>::~Bplustree() {
	char * h = head;
	memcpy(h, (char*)&totalBlock, sizeof(totalBlock));
	h += sizeof(totalBlock);
	memcpy(h, (char*)&root, sizeof(root));

	FILE * indexFile;
	indexFile = fopen(indexFileName, "rb+");

	fwrite(head, sizeof(totalBlock) + sizeof(root), 1, indexFile);
	fclose(indexFile);

	free(head);
	rootNode->writeBack();
//	delete rootNode;
//	LRUCacheIndex<key, value>::releaseLruInst();
	delete lruCache;
	free(indexFileName);
}

template<typename key, typename value>
void Bplustree<key, value>::init() {
	FILE * indexFile;
	if ((indexFile = fopen(indexFileName, "rb")) == NULL) {
		throw FileNotFoundException(indexFileName);
	}
	fread(head, sizeof(totalBlock) + sizeof(root), 1, indexFile);
	fclose(indexFile);

	char * h = head;
	memcpy((char*) &totalBlock, h, sizeof(totalBlock));
	h += sizeof(totalBlock);
	//根节点的位置
	memcpy((char*) &root, h, sizeof(root));

	rootNode = getRootNode();

//	cout << "Bplustree::init() root = " << root << endl;
//	cout << "Bplustree::init() rootNode->self() = " << rootNode->getSelf() << endl;
//	cout << "Bplustree::init() rootNode->type() = " << rootNode->getType() << endl;
//	cout << "Bplustree::init() rootNode:" << endl;
//	rootNode->printTreeNode();
}

template<typename key, typename value>
void Bplustree<key, value>::createIndex() {
//	cout << "Bplustree::createIndex()" << endl;
	//添加头信息	添加一个空块 root
	totalBlock = 1;
	root = 0;

	//初始化是根节点为叶节点（type=1)
	//int keyLen, int valueLen, unsigned long int self, int type, const char * indexFileName
	rootNode = new Treenode<key, value>(keyLen, valueLen, 0, 1, indexFileName);
//	TreeNode<key, value> * t = LRUCacheIndex<key, value>::getLruInst()->getLruCache()->put(root, rootNode);
	Treenode<key, value> * t = lruCache->put(root, rootNode);
	if (t) {
		delete t;
	}

	FILE * indexFile;
	if ((indexFile = fopen(indexFileName, "wb")) == NULL) {
		throw FileNotFoundException(indexFileName);
	}
	char * h = head;
	memcpy(h, (char*)&totalBlock, sizeof(totalBlock));
	h += sizeof(totalBlock);
	memcpy(h, (char*)&root, sizeof(root));
	fwrite(head, sizeof(totalBlock) + sizeof(root), 1, indexFile);
	fclose(indexFile);

	rootNode->writeBack();

}
/**
 * 根结点
 */
template<typename key, typename value>
Treenode<key, value> * Bplustree<key, value>::getRootNode() {
	Treenode<key, value> * node = nullptr;
	try {
		node = lruCache->get(root);
		if (node->getChange()) {
			node->writeBack();
		}
	} catch (exception & e) {
		FILE * indexFile;
		if ((indexFile = fopen(indexFileName, "rb")) == NULL) {
			throw FileNotFoundException("indexFileName");
		}
		char * block = (char*)malloc(BLOCK_SIZE);
		fseek(indexFile, root*BLOCK_SIZE + OFFSET_LENGTH, SEEK_SET);
		fread(block, BLOCK_SIZE, 1, indexFile);
		fclose(indexFile);

		node = new Treenode<key, value>(block, keyLen, valueLen, indexFileName);
		free(block);
		Treenode<key, value> * t = lruCache->put(root, node);
		if (t) {
			delete t;
		}
	}
	return node;
}
/**
 * 插入结点
 */
template<typename key, typename value>
void Bplustree<key, value>::put(key k, value v) {
//	cout << "Bplustree::put()" << endl;
	//找到应该包含k值的叶结点
	Treenode<key, value> * leafNode = getLeafNode(k);
	//往叶结点中插入数据k，v
	leafNode->addData(k, v);
	if (leafNode->getCount() >= treeNodeMaxSize) {
		//分裂
		split(leafNode);
	}

}
/**
 * 删除结点
 */
template<typename key, typename value>
void Bplustree<key, value>::remove(key k, value v) {
	Treenode<key, value> * leafNode = getLeafNode(k);
//	try {
//		leafNode->delData(k, v);
//	} catch (exception & e) {
//		cout << "KeyNotFoundException:key " << k << " not found !" << endl;
//		return;
//	}
	bool flag = false;
	while (true) {
		unsigned int index = 0;
		unsigned int count = leafNode->getCount();
		while (index < count && leafNode->getKey(index) != k) {
			index++;
		}
		//说明该结点中没有key值为K的元素，后面也不可能有了，直接返回
		if (index >= count) {
			return;
		} else {
			//从该结点中查找key值为k的，比较value是否为v
			while (index < count && leafNode->getKey(index) == k) {
				value va = leafNode->getValue(index);
				if (va == v) {
					flag = true;
					break;
				}
				index++;
			}
			//找到了，跳出最外层循环
			if (flag) {
				break;
			}
			//如果index == count 说明下一个结点还可能有key为k的出现
			if (index == count) {
				long int nextId = leafNode->getNext();
				if (nextId == -1) {
					break;
				} else {
					leafNode = getTreeNode(nextId);
				}
			}
		}
	}
	if (flag) {
		try {
			leafNode->delData(k, v);
		} catch (exception & e) {
			return;
		}
	} else {
		return;
	}

	int minLeafData = (int)((treeNodeMaxSize - 1)*1.0 / 2 + 0.5);	//向上取整 等价于ceil((treeNodeMaxSize-1))/2
	if (leafNode->getCount() < minLeafData) {
		handleDel(leafNode);
	}
}

template<typename key, typename value>
void Bplustree<key, value>::handleDel(Treenode<key, value> * leafNode) {
	rootNode = getRootNode();
	if (leafNode->getSelf() == rootNode->getSelf()) {	//只有根结点，就算再少也不作处理了
		return;
	}
	int minLeafData = (int)((treeNodeMaxSize - 1)*1.0 / 2 + 0.5);
//	int minInnerData = (int)(treeNodeMaxSize*1.0 / 2 + 0.5);
	//父结点
	Treenode<key, value> * parent = this->getParent(leafNode->getKey(0), leafNode->getSelf());
	if (parent->getSelf() == leafNode->getSelf()) {		//只有一个结点
		return;
	}
	Treenode<key, value> * t = lruCache->put(parent->getSelf(), parent);
	if (t) {
		delete t;
	}
	//leafNode的左兄弟结点
	Treenode<key, value> * leftNode = getLeftNode(leafNode, parent);
	//leafNode的右兄弟结点
	Treenode<key, value> * rightNode = getRightNode(leafNode, parent);

	//===============================
//	cout << "print 1" << endl;
//	this->printTree();

	//borrowLeft(leftNode, node, parent);
	if (leftNode != nullptr && leftNode->getCount() > minLeafData) {	//先看能否向左借
		cout << "Bplustree::handleDel() borrowLeft" << endl;
		borrowLeft(leftNode, leafNode, parent);

	} else if (rightNode != nullptr && rightNode->getCount() > minLeafData) {	//是否能向右借
		cout << "Bplustree::handleDel() borrowRight" << endl;
		borrowRight(rightNode, leafNode, parent);

	} else if(leftNode != nullptr) {	//左右都不能借了，如果左边不为空，向左边合并
		cout << "Bplustree::handleDel() mergeLeft" << endl;
		mergeLeft(leftNode, leafNode, parent);
	} else {	//只能向右边合并
		cout << "Bplustree::handleDel() mergeRight" << endl;
		mergeRight(rightNode, leafNode, parent);
	}
}
/**
 * 向左边合并
 */
template<typename key, typename value>
void Bplustree<key, value>::mergeLeft(Treenode<key, value> * leftNode, Treenode<key, value> * leafNode, Treenode<key, value> * parent) {
	cout << "Bplustree::mergeLeft start" << endl;
	//1. 把父结点value为leafNode->self()的项删除
	parent->delInnerData(leafNode->getSelf());
	parent->writeBack();

	//2. 合并结点 将leafNode合并到leftNode中
	leftNode->addRightNodeData(leafNode);

	//3. 把文件中的最后一项用来填补被合并的结点，并将其父结点关于该项的value改为当前值，totalBlock -= 1
	Treenode<key, value> * lastNode = getLastTreeNode(totalBlock);
	Treenode<key, value> * lastNodeParent = this->getParent(lastNode->getKey(0), lastNode->getSelf());
	cout << "Bplustree::mergeLeft lastNode->type = " << lastNode->getType();
	cout << "  self = " << lastNode->getSelf() << endl;
	cout << "lastNode data:" << endl;
	lastNode->printTreeNode();

	//========================
//	cout << "print 2" << endl;
//	this->printTree();

	if (leafNode->getSelf() != totalBlock - 1) {
		cout << "Bplustree::mergeLeft() copy data" << endl;

		char * leafNodeData = leafNode->getData();
		char * lastNodeData = lastNode->getData();
//		unsigned int lastNodeCount = lastNode->getCount();

		memcpy(leafNodeData, lastNodeData, TREE_NODE_DATA_SIZE);
		leafNode->setChange(true);
		leafNode->setCount(lastNode->getCount());
		leafNode->setType(lastNode->getType());

		leafNode->writeBack();

		//如果最后一个结点是根结点所在的话
		rootNode = getRootNode();
		if (rootNode->getSelf() == lastNode->getSelf()) {
//			rootNode = leafNode;
			root = leafNode->getSelf();
		} else {
//			cout << "lastNodeParent data(before update):" << endl;
//			lastNodeParent->printTreeNode();
//			cout << "print 3" << endl;
//			this->printTree();

			if (lastNode->getType() == 1) {
				cout << "Bplustree::mergeLeft() lastNode->type == 1" << endl;
				Treenode<key, value> * lastNodeLeft = this->getLeftNodeAll(lastNode, lastNodeParent);
				lastNodeLeft->setNext(leafNode->getSelf());
				lastNodeLeft->writeBack();
			}
			//这个更新必须放在getLeftNodeAll之后
			lastNodeParent->updataValue(lastNode->getSelf(), leafNode->getSelf());
//			cout << "lastNodeParent data(after update):" << endl;
//			lastNodeParent->printTreeNode();

			lastNodeParent->writeBack();

//			cout << "print 4" << endl;
//			this->printTree();

//			TreeNode<key, value> * t = LRUCacheIndex<key, value>::getLruInst()->getLruCache()
//					->put(leafNode->getSelf(), leafNode);
//			if (t) {
//				delete t;
//			}
//			t = LRUCacheIndex<key, value>::getLruInst()->getLruCache()
//					->put(lastNodeParent->getSelf(), lastNodeParent);
//			if (t) {
//				delete t;
//			}

//			cout << "print 5" << endl;
//			this->printTree();
		}
//		delete lastNode;
		totalBlock -= 1;
	} else {
		cout << "else else else else else else else else else" << endl;
		if (lastNode->getType() == 1) {
			Treenode<key, value> * lastNodeLeft = this->getLeftNodeAll(lastNode, lastNodeParent);
			lastNodeLeft->setNext(-1);
			lastNodeLeft->writeBack();
		}
		totalBlock -= 1;
	}


	//4. 递归对父结点进行该过程。。。
	int minInnerCount = (int)(treeNodeMaxSize*1.0/2 + 0.5);
	cout << "Bplustree::mergeLeft middle" << endl;
	while(parent->getCount() < minInnerCount) {
		cout << "Bplustree::mergeLeft while" << endl;
		rootNode = getRootNode();
		if (parent->getSelf() == rootNode->getSelf()) {	//parent为根的情况
			if (parent->getCount() == 0) {
				if (parent->getSelf() != totalBlock - 1) {
					//把索引文件中的最后一个结点用来填rootNode所在的位置
					Treenode<key, value> * lastNode = getLastTreeNode(totalBlock);
					parent->setCount(lastNode->getCount());
					parent->setType(lastNode->getType());
					parent->setChange(true);
					char * d0 = parent->getData();
					char * d1 = lastNode->getData();
//					unsigned int count = lastNode->getCount();
					memcpy(d0, d1, TREE_NODE_DATA_SIZE);
					Treenode<key, value> * t = lruCache->put(parent->getSelf(), parent);
					if (t) {
						delete t;
					}

					//重置rootNode
					root = leftNode->getSelf();
					totalBlock -= 1;		//树变矮一层
					return;
				} else {	//rootNode为文件中的最后一个结点
					root = leftNode->getSelf();
					totalBlock -= 1;		//树变矮一层
					return;
				}
			} else {
				return;
			}
		} else {	//parent不为根的情况
			bool merge = false;
			Treenode<key, value> * node = parent;
			parent = this->getParent(parent->getKey(0), parent->getSelf());
			Treenode<key, value> * tt = lruCache->put(parent->getSelf(), parent);
			if (tt) {
				delete tt;
			}
			leftNode = getLeftNode(node, parent);
			Treenode<key, value> * rightNode = getRightNode(node, parent);
			if (leftNode != nullptr && leftNode->getCount() > minInnerCount) {
				//从左边借一项
				this->borrowLeft(leftNode, node, parent);
				//1. parent value为node的key要改变  borrowLeft已完成该功能了
				//2. node的key0应该是node中value1对应块的key0
				Treenode<key, value> * nextNode = getTreeNode(node->getValue(1));
				key k = nextNode->getKey(0);
				node->updataKey(k, nextNode->getSelf());
				node->writeBack();
			} else if (rightNode != nullptr && rightNode->getCount() > minInnerCount) {
				//从右边借一项
				this->borrowRight(rightNode, node, parent);
				Treenode<key, value> * nextNode = getTreeNode(node->getValue(node->getCount()));
				key k = nextNode->getKey(0);
				node->updataKey(k, nextNode->getSelf());
				node->writeBack();
			} else if (leftNode != nullptr) {
				//向左合并
				//获取值为v的k
				key k = parent->delInnerData(node->getSelf());
				parent->writeBack();
				leftNode->addRightNodeData_Inner(node, k);
				merge = true;
			} else {
				//向右合并
				key k = parent->delInnerData(node->getSelf());
				parent->writeBack();
				rightNode->addLeftNodeData_Inner(node, k);
				merge = true;
			}

			if (merge) {		//如果发生合并，则将文件中的最后一块填到被合并的块中
				Treenode<key, value> * lastNode = getLastTreeNode(totalBlock);
				cout << "Bplustree::mergeLeft() lastNode->self = " << lastNode->getSelf() << endl;
				node->setCount(lastNode->getCount());
				node->setType(lastNode->getType());
				node->setChange(true);

				char * nodeData = node->getData();
				char * lastNodeData = lastNode->getData();
				memcpy(nodeData, lastNodeData, TREE_NODE_DATA_SIZE);

				Treenode<key, value> * t = lruCache->put(node->getSelf(), node);
				if (t) {
					delete t;
				}
				//重新设置node的parent
				rootNode = getRootNode();
				if (rootNode->getSelf() == lastNode->getSelf()) {
//					rootNode = node;
					root = node->getSelf();
				} else {
					Treenode<key, value> * lastNodeParent = this->getParent(lastNode->getKey(0), lastNode->getSelf());

					if (lastNode->getType() == 1) {
						Treenode<key, value> * lastNodeLeft = this->getLeftNodeAll(lastNode, lastNodeParent);
						lastNodeLeft->setNext(node->getSelf());
						lastNodeLeft->writeBack();
					}
					lastNodeParent->updataValue(lastNode->getSelf(), node->getSelf());
					lastNodeParent->writeBack();
					t = lruCache->put(lastNodeParent->getSelf(), lastNodeParent);
					if (t) {
						delete t;
					}
				}
//				delete lastNode;
				totalBlock--;
			}
		}
	}
	cout << "Bplustree::mergeLeft end" << endl;
}
/**
 * 向右边合并
 */
template<typename key, typename value>
void Bplustree<key, value>::mergeRight(Treenode<key, value> * rightNode, Treenode<key, value> * leafNode, Treenode<key, value> * parent) {
	cout << "Bplustree::mergeRight start" << endl;
	//1. 把父结点value为leafNode->self()的项删除
	parent->delInnerData(leafNode->getSelf());
	parent->writeBack();

	//2. 合并结点 将leafNode合并到rightNode中
	rightNode->addLeftNodeData(leafNode);

	//将rightNode中的数据复制到leaftNode中
	leafNode->copyData(rightNode);
	leafNode->writeBack();

	//父结点需要改变	将 v0 改成 v1
	parent->updataValue(rightNode->getSelf(), leafNode->getSelf());
	parent->writeBack();
	//将leafNode重新赋值
	leafNode = rightNode;

	//3. 把文件中的最后一项用来填补被合并的结点，并将其父结点关于该项的value改为当前值，totalBlock -= 1
	Treenode<key, value> * lastNode = getLastTreeNode(totalBlock);
	Treenode<key, value> * lastNodeParent = this->getParent(lastNode->getKey(0), lastNode->getSelf());

//	cout << "Bplustree::mergeRight() lastNodeLeft data :" << endl;
//	lastNodeLeft->printTreeNode();

	if (leafNode->getSelf() != totalBlock - 1) {
		char * leafNodeData = leafNode->getData();
		char * lastNodeData = lastNode->getData();

		memcpy(leafNodeData, lastNodeData, TREE_NODE_DATA_SIZE);
		leafNode->setChange(true);
		leafNode->setCount(lastNode->getCount());
		leafNode->setType(lastNode->getType());
		leafNode->writeBack();

		//如果最后一个结点刚好是根结点的话
		rootNode = getRootNode();
		if (rootNode->getSelf() == lastNode->getSelf()) {
//			rootNode = leafNode;
			root = leafNode->getSelf();
		} else {
			if (lastNode->getType() == 1) {
				Treenode<key, value> * lastNodeLeft = this->getLeftNodeAll(lastNode, lastNodeParent);
				lastNodeLeft->setNext(leafNode->getSelf());
				lastNodeLeft->writeBack();
			}
			//这个必须放在getLeftNodeAll之后
			lastNodeParent->updataValue(lastNode->getSelf(), leafNode->getSelf());
			lastNodeParent->writeBack();

			leafNode->writeBack();
			Treenode<key, value> * t = lruCache->put(leafNode->getSelf(), leafNode);
			if (t) {
				delete t;
			}
			t = lruCache->put(lastNodeParent->getSelf(), lastNodeParent);
			if (t) {
				delete t;
			}
		}
//		delete lastNode;
		totalBlock -= 1;
	} else {
		//这里不用了，但是上面的mergeLeft还是要用的
//		if (lastNode->getType() == 1) {
//			TreeNode<key, value> * lastNodeLeft = this->getLeftNodeAll(lastNode, lastNodeParent);
//			lastNodeLeft->setNext(-1);
//			lastNodeLeft->writeBack();
//		}
		totalBlock -= 1;
	}
//	lastNodeLeft->writeBack();
//	TreeNode<key, value> * t = LRUCacheIndex<key, value>::getLruInst()->getLruCache()
//			->put(lastNodeLeft->getSelf(), lastNodeLeft);
//	if (t) {
//		delete t;
//	}
	//4. 递归进行。。。
	int minInnerCount = (int)(treeNodeMaxSize*1.0/2 + 0.5);
	while (parent->getCount() < minInnerCount) {
		cout << "Bplustree::mergeRight() while" << endl;
		rootNode = getRootNode();
		if (parent->getSelf() == rootNode->getSelf()) {		//parent刚好是根
			if (parent->getCount() == 0) {					//说明树根只剩一个子结点了,数个高度需要改变
				if (parent->getSelf() != totalBlock - 1) {	//根结点不是最后一个结点的情况
					//把索引文件中的最后一个结点用来填parent所在的位置,即rootNode的位置
					Treenode<key, value> * lastNode = getLastTreeNode(totalBlock);	//此时lastNode就是上面的leafNode
					char * parentData = parent->getData();
					char * lastData = lastNode->getData();
					memcpy(parentData, lastData, TREE_NODE_DATA_SIZE);

					parent->setCount(lastNode->getCount());
					parent->setChange(true);
					parent->writeBack();
					Treenode<key, value> * t = lruCache->put(parent->getSelf(), parent);
					if (t) {
						delete t;
					}
//					delete lastNode;
					//重置rootNode
					root = rightNode->getSelf();
					totalBlock -= 1;
					return;
				} else {	//根结点更好是最后一个结点
					root = rightNode->getSelf();
					totalBlock -= 1;
					return;
				}
			} else {	//根结点还有别的子结点
				return;
			}
		} else {	//parent不是根的情况
			bool merge = false;
			Treenode<key, value> * node = parent;
			parent = this->getParent(parent->getKey(0), parent->getSelf());

			Treenode<key, value> * leftNode = getLeftNode(node, parent);
			rightNode = getRightNode(node, parent);
			if (leftNode != nullptr && leftNode->getCount() > minInnerCount) {
				//从左边借一项
				this->borrowLeft(leftNode, node, parent);
				Treenode<key, value> * nextNode = getTreeNode(node->getValue(1));
				key k = nextNode->getKey(0);
				node->updataKey(k, nextNode->getSelf());
				node->writeBack();
			} else if (rightNode != nullptr && rightNode->getCount() > minInnerCount) {
				//从右边借一项
				this->borrowRight(rightNode, node, parent);
				Treenode<key, value> * nextNode = getTreeNode(node->getValue(node->getCount()));
				key k = nextNode->getKey(0);
				node->updataKey(k, nextNode->getSelf());
				node->writeBack();
			} else if (leftNode != nullptr) {
				//向左合并
				key k = parent->delInnerData(node->getSelf());
				parent->writeBack();
				leftNode->addRightNodeData_Inner(node, k);
				leftNode->writeBack();
				merge = true;
			} else {
				key k = parent->delInnerData(node->getSelf());
				parent->writeBack();
				rightNode->addLeftNodeData_Inner(node, k);
				rightNode->writeBack();
				merge = true;
			}
			if (merge) {
				if (node->getSelf() == totalBlock - 1) {	//被合并的块刚好处在最后一块
					totalBlock -= 1;
				} else {
					Treenode<key, value> * lastNode = getLastTreeNode(totalBlock);
					node->setCount(lastNode->getCount());
					node->setType(lastNode->getType());
					node->setChange(true);

					char * nodeData = node->getData();
					char * lastNodeData = lastNode->getData();
					memcpy(nodeData, lastNodeData, TREE_NODE_DATA_SIZE);

					rootNode = getRootNode();
					if (rootNode->getSelf() == lastNode->getSelf()) {
						root = node->getSelf();
					} else {
						Treenode<key, value> * lastNodeParent = this->getParent(lastNode->getKey(0), lastNode->getSelf());

						if (lastNode->getType() == 1) {
							Treenode<key, value> * lastNodeLeft = this->getLeftNodeAll(lastNode, lastNodeParent);
							lastNodeLeft->setNext(node->getSelf());
							lastNodeLeft->writeBack();
						}
						lastNodeParent->updataValue(lastNode->getSelf(), node->getSelf());
						lastNodeParent->writeBack();
						Treenode<key, value> * t = lruCache->put(node->getSelf(), node);
						if (t) {
							delete t;
						}
					}
//					delete lastNode;
					totalBlock--;
				}
			}
		}
	}
	cout << "Bplustree::mergeRight end" << endl;
}

/**
 * 获取索引文件的最后一块
 */
template<typename key, typename value>
Treenode<key, value> * Bplustree<key, value>::getLastTreeNode(unsigned long int totalBlock) {
	//rootNode为第0块
	Treenode<key, value> * lastNode = nullptr;
	try {
		lastNode = lruCache->get(totalBlock - 1);
	} catch (exception & e) {
		//需要从磁盘中读取
		FILE * indexFile;
		if ((indexFile = fopen(indexFileName, "rb")) == NULL) {
			throw FileNotFoundException(string(indexFileName));
		}
		fseek(indexFile, (totalBlock - 1)*BLOCK_SIZE + OFFSET_LENGTH, SEEK_SET);
		char * block = (char*)malloc(BLOCK_SIZE);
		fread(block, BLOCK_SIZE, 1, indexFile);
		fclose(indexFile);
		//const char * block, int keyLen, int valueLen, const char * indexFileName
		lastNode = new Treenode<key, value>(block, keyLen, valueLen, indexFileName);
		Treenode<key, value> * t = lruCache->put(lastNode->getSelf(), lastNode);
		if (t) {
			delete t;
		}
	}

	return lastNode;
}
//获取指定的块
template<typename key, typename value>
Treenode<key, value> * Bplustree<key, value>::getTreeNode(unsigned long int self) {
	Treenode<key, value> * node = nullptr;
	try {
		node = lruCache->get(self);
	} catch (exception & e) {
		FILE * indexFile;;
		if ((indexFile = fopen(indexFileName, "rb")) == NULL) {
			throw FileNotFoundException(string(indexFileName));
		}
		fseek(indexFile, self*BLOCK_SIZE + OFFSET_LENGTH, SEEK_SET);
		char * block = (char*)malloc(BLOCK_SIZE);
		fread(block, BLOCK_SIZE, 1, indexFile);
		fclose(indexFile);

		node = new Treenode<key, value>(block, keyLen, valueLen, indexFileName);
		Treenode<key, value> * t = lruCache->put(node->getSelf(), node);
		if (t) {
			delete t;
		}
	}
	return node;
}

template<typename key, typename value>
void Bplustree<key, value>::borrowLeft(Treenode<key, value> * leftNode, Treenode<key, value> * node, Treenode<key, value> * parent){
//	cout << "Bplustree::borrowLeft() start" << endl;
	//node整体右窜一位,并将leftNode中的数据的最后一项加到node中
	node->moveRight(leftNode);
	//修改parent中的索引
	//1. 先获取移动后的leafNode的第一项的索引
	key k = node->getKey(0);
	//2. 将父结点value值为node->getSelf()的项的索引置为k
//	cout << "node->self = " << node->getSelf() << endl;
//	cout << "parent before update :" << endl;
//	parent->printTreeNode();
	parent->updataKey(k, node->getSelf());
	parent->writeBack();
}
template<typename key, typename value>
void Bplustree<key, value>::borrowRight(Treenode<key, value> * rightNode, Treenode<key, value> * node, Treenode<key, value> * parent) {
	//将rightNode中的第一项加到leafNode的尾部
	node->moveLeft(rightNode);
	//修改parent中的索引
	//1. 获取右边结点的第一项的索引
	key k = rightNode->getKey(0);
//	cout << "Bplustree::borrowRight() parent data before update :" << endl;
//	parent->printTreeNode();
	//2. 将父结点value值为rightNode->self(0的项的索引置为k
	parent->updataKey(k, rightNode->getSelf());
	parent->writeBack();
//	cout << "Bplustree::borrowRight() parent data after update :" << endl;
//	parent->printTreeNode();
}
/**
 * 获取可以不具有同一个父结点的做兄弟结点(只针对叶结点)
 */
template<typename key, typename value>
Treenode<key, value> * Bplustree<key, value>::getLeftNodeAll(Treenode<key, value> * node, Treenode<key, value> * parent) {
	Treenode<key, value> * leftNode;
	leftNode = getLeftNode(node, parent);
	if (leftNode == nullptr) {
		//getLeftNode(TreeNode<key, value> * node, TreeNode<key, value> * parent)
		Treenode<key, value> * parent_parent = getParent(parent->getKey(0), parent->getSelf());
		Treenode<key, value> * parentLeftNode = getLeftNode(parent, parent_parent);
		//查找具有第一个相同的父结点
		while (parentLeftNode == nullptr) {
			parent = parent_parent;
			parent_parent = getParent(parent->getKey(0), parent->getSelf());
			parentLeftNode = getLeftNode(parent, parent_parent);
		}
		leftNode = parentLeftNode;
		FILE * indexFile;
		if ((indexFile = fopen(indexFileName, "rb")) == NULL) {
			throw FileNotFoundException(indexFileName);
		}
		while (leftNode->getType() != 1) {
			value nextAddr = leftNode->getValue(leftNode->getCount());		//下标从0开始的
			try {
				leftNode = lruCache->get(nextAddr);
//				cout << "Bplustree::getLeftNodeAll() try" << endl;
//				cout << "leftNode->self = " << leftNode->getSelf() << " nextAddr = " << nextAddr << endl;

			} catch (exception &e) {
				char * block = (char*)malloc(BLOCK_SIZE);
				fseek(indexFile, nextAddr*BLOCK_SIZE + OFFSET_LENGTH, SEEK_SET);
				fread(block, BLOCK_SIZE, 1, indexFile);
				leftNode = new Treenode<key, value>(block, keyLen, valueLen, indexFileName);
				free(block);
				Treenode<key, value> * t = lruCache->put(nextAddr, leftNode);
				if (t) {
					delete t;
				}
			}
		}
		fclose(indexFile);
	}
	return leftNode;
}
/**
 * 获取具有同一个父结点的左兄弟结点
 */
template<typename key, typename value>
Treenode<key, value> * Bplustree<key, value>::getLeftNode(Treenode<key, value> * node, Treenode<key, value> * parent) {
	if (parent->getValue(0) == node->getSelf()) {						//node没有左兄弟结点
		return nullptr;
	}
	unsigned long int leftNodeAddr;
	for (int index = 0; index <= parent->getCount(); index++) {
		value vv = parent->getValue(index);
		if (vv == node->getSelf()) {
			leftNodeAddr = parent->getValue(index-1);
			break;
		}
	}

	FILE * indexFile;
	if ((indexFile = fopen(indexFileName, "rb")) == NULL) {
		throw FileNotFoundException(indexFileName);
	}
	Treenode<key, value> * leftNode = nullptr;
	try {
		leftNode = lruCache->get(leftNodeAddr);
	} catch (exception & e) {
		char * block = (char*)malloc(BLOCK_SIZE);
		fseek(indexFile, leftNodeAddr*BLOCK_SIZE + OFFSET_LENGTH, SEEK_SET);
		fread(block, BLOCK_SIZE, 1, indexFile);
		leftNode = new Treenode<key, value>(block, keyLen, valueLen, indexFileName);
		free(block);

		Treenode<key, value> * t = lruCache->put(leftNodeAddr, leftNode);
		if (t) {
			delete t;
		}
	}
	fclose(indexFile);
	return leftNode;
}
template<typename key, typename value>
Treenode<key, value> * Bplustree<key, value>::getRightNode(Treenode<key, value> * node, Treenode<key, value> * parent) {
	Treenode<key, value> * rightNode = nullptr;
	unsigned long int rightNodeAddr;
//	if (node->getSelf() == rootNode->getSelf()) {		//只有一个叶结点，也是根结点
//		return nullptr;
//	}
	//获取rightNodeAddr
	for (int index = 0; index <= parent->getCount(); index++) {
		value vv = parent->getValue(index);
		if (vv == node->getSelf()) {
			if (index == parent->getCount()) {		//没有右兄弟结点
				return nullptr;
			}
			rightNodeAddr = parent->getValue(index + 1);
			break;
		}
	}
	//从rightNodeAddr中获取结点TreeNode
	FILE * indexFile;
	if ((indexFile = fopen(indexFileName, "rb")) == NULL) {
		throw FileNotFoundException(indexFileName);
	}
	try {
		rightNode = lruCache->get(rightNodeAddr);
	} catch (exception & e) {
		char * block = (char*) malloc(BLOCK_SIZE);
		fseek(indexFile, rightNodeAddr * BLOCK_SIZE + OFFSET_LENGTH, SEEK_SET);
		fread(block, BLOCK_SIZE, 1, indexFile);
		//const char * block, int keyLen, int valueLen, const char * indexFileName
		rightNode = new Treenode<key, value>(block, keyLen, valueLen, indexFileName);
		free(block);

		Treenode<key, value> * t = lruCache->put(rightNodeAddr, rightNode);
		if (t) {
			delete t;
		}
	}
	fclose(indexFile);
	return rightNode;
}
template<typename key, typename value>
void Bplustree<key, value>::split(Treenode<key, value> * leafNode) {
//	cout << "Bplustree::split()" << endl;
	unsigned long int self = totalBlock;
	totalBlock++;
	Treenode<key, value> * rightNode = new Treenode<key, value>(keyLen, valueLen, self, 1, indexFileName);
	pair<key, value> p = leafNode->splitData(rightNode);	//返回分裂后要插入父结点的key以及叶结点的地址
//	cout << "Bplustree::split() pair : " << p.first << " " << p.second << endl;
	//将分裂后的结点放入LRU中
	Treenode<key, value> * t = lruCache->put(leafNode->getSelf(), leafNode);
	if (t) {
		delete t;
	}
	t = lruCache->put(rightNode->getSelf(), rightNode);
	if (t) {
		delete t;
	}
//	cout << "Bplustree::split() 1" << endl;

	//找分裂后左边结点的父结点,通过该结点的第一个关键词以及自身的块号（id）
//	cout << "Bplustree::split() leafNode->key(0) = " << leafNode->getKey(0);
//	cout << "  leafNode->self = " << leafNode->getSelf() << endl;
	Treenode<key, value> * parentNode = getParent(leafNode->getKey(0), leafNode->getSelf());
//	cout << "Bplustree::split() 2" << endl;
	if (parentNode->getSelf() == leafNode->getSelf()) {		//就一个根结点，即根结点也为叶结点
		rootNode = new Treenode<key, value>(keyLen, valueLen, totalBlock, 0, indexFileName);
		//rootNode->addData(p.first, p.second);
		rootNode->addFirstInnerData(leafNode->getSelf(), p.first, p.second);

//		cout << "Bplustree::split() rootNode self = " << rootNode->getSelf() << endl;

		t = lruCache->put(rootNode->getSelf(), rootNode);
		if (t) {
			delete t;
		}
		//更新索引文件头部信息
		totalBlock++;									//总块数
		root = rootNode->getSelf();						//根结点的位置
		rootNode->writeBack();							//把跟结点写回磁盘
		return;
	}
	//根结点为内节点
	parentNode->addInnerData(p.first, p.second);

	while (parentNode->getCount() >= treeNodeMaxSize) {
		cout << "Bplustree::split() 非叶结点分裂" << endl;
//		parentNode->printTreeNode();
		rootNode = getRootNode();
		if (parentNode->getSelf() == rootNode->getSelf()) {	//分裂的结点为根结点
			cout << "Bplustree::split() 根结点分裂" << endl;
//			parentNode->printTreeNode();
			//int keyLen, int valueLen, unsigned long int self,int type, const char * indexFileName
			Treenode<key, value> * rightInnerNode =
					new Treenode<key, value>(keyLen, valueLen, totalBlock, 0, indexFileName);
			totalBlock++;
			pair<key, value> pp = parentNode->splitInnerData(rightInnerNode);
			rootNode = new Treenode<key, value>(keyLen, valueLen, totalBlock, 0, indexFileName);
			totalBlock++;
			rootNode->addFirstInnerData(parentNode->getSelf(), pp.first, pp.second);
//			cout << "Bplustree::split() pair.first = " << pp.first << "  pair.second = " << pp.second << endl;
//			cout << "Bplustree::split() rootNode->value(0) = " << rootNode->getValue(0);
//			cout << "  rootNode->value(1) = " << rootNode->getValue(1) << endl;
			root = rootNode->getSelf();
//			cout << "parentNode->self = " << parentNode->getSelf();
//			cout << " rightNode->self = " << rightInnerNode->getSelf();
//			cout << " rootNode ->self = " << rootNode->getSelf() << endl;
			t = lruCache->put(parentNode->getSelf(), parentNode);
			if (t) {
				delete t;
			}
			t = lruCache->put(rightInnerNode->getSelf(), rightInnerNode);
			if (t) {
				delete t;
			}
			t = lruCache->put(rootNode->getSelf(), rootNode);
			if (t) {
				delete t;
			}
//			rootNode = new TreeNode<key, value>(keyLen, valueLen, totalBlock, 0, indexFileName);
//
//			rootNode->addFirstInnerData(leafNode->getSelf(), p.first, p.second);
//
//			//更新索引文件头部信息
//			totalBlock++;									//总块数
//			root = rootNode->getSelf();						//根结点的位置
			break;
		} else {
			cout << "1. parentNode->self = " << parentNode->getSelf() << endl;
			cout << "Bplustree::split() 内节点分裂 非根结点" << endl;
			self = totalBlock;
			totalBlock++;
			Treenode<key, value> * rightInnerNode =
					new Treenode<key, value>(keyLen, valueLen, self, 0, indexFileName);
			pair<key, value> inner = parentNode->splitInnerData(rightInnerNode);
//			cout << "Bplustree::split() inner.first = " << inner.first << "inner.second = " <<  inner.second << endl;
//			cout << "1).parentNode data:" << endl;
//			parentNode->printTreeNode();
//			cout << "2).rightInnerNode data:" << endl;
//			rightInnerNode->printTreeNode();

			//---------放入LRU缓冲区中---------
			t = lruCache->put(parentNode->getSelf(), parentNode);
			if (t) {
				delete t;
			}
			t = lruCache->put(rightInnerNode->getSelf(), rightInnerNode);
			if (t) {
				delete t;
			}

			parentNode = getParent(parentNode->getKey(0), parentNode->getSelf());
//			cout << "before add data : " << endl;
//			parentNode->printTreeNode();
			parentNode->addInnerData(inner.first, inner.second);
//			cout << "2. parenNode->self = " << parentNode->getSelf() << endl;
//			parentNode->printTreeNode();
		}
	}
//	cout << "Bplustree::split() end" << endl;
}

template<typename key, typename value>
Treenode<key, value> * Bplustree<key, value>::getParent(key k, value childId) {
//	cout << "Bplustree::getParent() k = " << k << " childId = " << childId << endl;
	rootNode = getRootNode();
	if (rootNode->getSelf() == childId) {	//只有根结点
//		cout << "Bplustree::getParent() only root node" << endl;
		return rootNode;
	}
	FILE * indexFile;
	if ((indexFile = fopen(indexFileName, "rb")) == NULL) {
		throw FileNotFoundException(indexFileName);
	}
	Treenode<key, value> * node = rootNode;

	Treenode<key, value> * childNode = nullptr;
	unsigned long int nextAddr = node->getNextChild(k);
	while (nextAddr != childId) {
//		cout << "Bplustree::getParent() 3 nextAddr = " << nextAddr << " childId = " << childId << endl;
//		node->printTreeNode();
		try {
			childNode = lruCache->get(nextAddr);
		} catch (exception & e) {
			char * child = (char*) malloc(BLOCK_SIZE);

			fseek(indexFile, nextAddr*BLOCK_SIZE+OFFSET_LENGTH, SEEK_SET);
			fread(child, BLOCK_SIZE, 1, indexFile);
			childNode = new Treenode<key, value>(child, keyLen, valueLen,indexFileName);
			free(child);
			//放入LRU缓冲区
			Treenode<key, value> * t = lruCache->put(childNode->getSelf(), childNode);
			if (t) {
				delete t;
			}
		}
		node = childNode;
		nextAddr = node->getNextChild(k);
	}
	fclose(indexFile);
	return node;
}

template<typename key, typename value>
Treenode<key, value> * Bplustree<key, value>::getLeafNode(key k) {
	FILE * indexFile;
	if ((indexFile = fopen(indexFileName, "rb")) == NULL) {
		throw FileNotFoundException(indexFileName);
	}
	rootNode = getRootNode();
	Treenode<key, value> * node = rootNode;
	Treenode<key, value> * childNode = nullptr;
	while (node->getType() != 1) {
		unsigned long int nextAddr = node->getNextChild(k);
		try {	//先尝试从缓冲区获取
			childNode = lruCache->get(nextAddr);

		} catch (exception & e) {	//缓冲区没找到
			char * child = (char*) malloc(BLOCK_SIZE);
			fseek(indexFile, nextAddr*BLOCK_SIZE+OFFSET_LENGTH, SEEK_SET);
			fread(child, BLOCK_SIZE, 1, indexFile);
			childNode = new Treenode<key, value>(child, keyLen, valueLen,indexFileName);
			free(child);
			//放入LRU缓冲区
			Treenode<key, value> * t = lruCache->put(nextAddr, childNode);
			if (t) {
				delete t;
			}
		}
		node = childNode;
	}
	fclose(indexFile);
	return node;
}
/*
 * 查找关键词k在文件中的块号
 */
template<typename key, typename value>
set<value> Bplustree<key, value>::get(key k) {
	//rootNode 为叶节点 (即索引中只有一个节点，直接遍历该叶节点即可）
	FILE * indexFile;
	if ((indexFile = fopen(indexFileName, "rb")) == NULL) {
		throw FileNotFoundException(indexFileName);
	}
	rootNode = getRootNode();
	Treenode<key, value> * node = rootNode;
	Treenode<key, value> * childNode = nullptr;

	//查找关键词在那个子节点
	while (node->getType() != 1) {
		unsigned long int nextAddr = node->getNextChild(k);
//		cout << "Bplustree::get() nextAddr = " << nextAddr << endl;

		try {	//先尝试从缓冲区获取
			childNode = lruCache->get(nextAddr);
		} catch (exception & e) {
			char * child = (char*)malloc(BLOCK_SIZE);
			fseek(indexFile, nextAddr*BLOCK_SIZE + OFFSET_LENGTH, SEEK_SET);
			fread(child, BLOCK_SIZE, 1, indexFile);
			childNode = new Treenode<key, value>(child, keyLen, valueLen, indexFileName);
			free(child);
			//放入LRU缓冲区
			Treenode<key, value> * t = lruCache->put(nextAddr, childNode);
			if (t) {
				delete t;
			}
		}
		node = childNode;
	}
	fclose(indexFile);
	//查找关键词k是否在子节点中
	int index = node->binarySearch(k);

//	vector<value> values;
	set<value> values;

	//index == -1表示没找到，否则index表示索引值k的下标
//	if (index==-1) {
//		throw KeyNotFoundException(k);
//	} else {
//		return node->getValue(index);	//返回k对应的value，即为关键词对应关系表中的第几块
//	}
	if (index != -1) {
		for (int i = index - 1; i >=0; i--) {
			if (node->getKey(i) == k) {
//				values.push_back(node->getValue(i));
				values.insert(node->getValue(i));
			} else {
				break;
			}
		}
//		values.push_back(node->getValue(index));
		values.insert(node->getValue(index));
		for (int i = index + 1; i < node->getCount(); i++) {
			if (node->getKey(i) == k) {
//				values.push_back(node->getValue(i));
				values.insert(node->getValue(i));
			} else {
				break;
			}
		}
	}
	return values;
}
template<typename key, typename value>
void Bplustree<key, value>::printTree() {
	rootNode = getRootNode();
	Treenode<key, value> * node = rootNode;
	if (rootNode == nullptr) {
		cout << "Bplustree()::printTree() rootNode == nullptr" << endl;
	}
	cout << "Bplustree::printTree() rootNode->getSelf() = " << rootNode->getSelf() << endl;
	cout << "Bplustree::printTree() rootNode data:" << endl;
	rootNode->printTreeNode();
//	cout << "rootNode data : rootNode->value(1) = " << rootNode->getValue(1) << endl;
	cout << "Bplustree::printTree() rootNode->getSelf() = " << rootNode->getSelf() << endl;
//	cout << "Bplustree::printTree() rootNode->change = " << rootNode->getChange() << endl;
//	if (rootNode->getChange()) {
//		rootNode->writeBack();
//		cout << "write back OK" << endl;
//	}
	FILE * indexFile;
	if ((indexFile = fopen(indexFileName, "rb")) == NULL) {
		throw FileNotFoundException(indexFileName);
	}
	while (node->getType() != 1) {
		unsigned long int nextAddr = node->getValue(0);
//		cout << "Bplustree::printTree() nextAddr = " << nextAddr << endl;
		try {
			node = lruCache->get(nextAddr);
		} catch (exception & e) {
			char * block = (char*)malloc(BLOCK_SIZE);
			fseek(indexFile, nextAddr*BLOCK_SIZE + OFFSET_LENGTH, SEEK_SET);
			fread(block, BLOCK_SIZE, 1, indexFile);
			//TreeNode(const char * block, int keyLen, int valueLen, const char * indexFileName);
			node = new Treenode<key, value>(block, keyLen, valueLen, indexFileName);
			Treenode<key, value> * t = lruCache->getLruCache()->put(nextAddr, node);
			if (t) {
				delete t;
			}
			free(block);
		}
	}
	Treenode<key, value> * parent = getParent(node->getKey(0), node->getSelf());
	cout << "leafNode data:" << endl;
	cout << "leafNode->self = " << node->getSelf() << " parent = " << parent->getSelf() << endl;
	node->printTreeNode();
	while (node->getNext() != -1) {
		unsigned long int nextAddr = node->getNext();
		cout << "Bplustree::printTree() nextAddr = " << nextAddr << endl;
		try {
			node = lruCache->get(nextAddr);
		} catch (exception & e) {
			char * block = (char*) malloc(BLOCK_SIZE);
			fseek(indexFile, nextAddr * BLOCK_SIZE + OFFSET_LENGTH, SEEK_SET);
			fread(block, BLOCK_SIZE, 1, indexFile);
			node = new Treenode<key, value>(block, keyLen, valueLen, indexFileName);
			Treenode<key, value> * t = lruCache->put(nextAddr, node);
			if (t) {
				cout << "Bplustree::printTree() delete" << endl;
				delete t;
				cout << "Bplustree::printTree() delete 2" << endl;
			}
			free(block);
		}
//		cout << "Bplustree::printTree() node data:" << endl;
//		node->printTreeNode();
		cout << "Bplustree::printTree() before getParent()" << endl;
		parent = getParent(node->getKey(0), node->getSelf());
		cout << "leafNode->self = " << node->getSelf() << " parent = " << parent->getSelf() << endl;
		node->printTreeNode();
	}
	fclose(indexFile);
}

template<typename key, typename value>
void Bplustree<key, value>::test(unsigned long int self) {
	//self = 494
	FILE * indexFile;
	if ((indexFile = fopen(indexFileName, "rb")) == NULL) {
		throw FileNotFoundException(indexFileName);
	}
	char * block = (char*)malloc(BLOCK_SIZE);
	fseek(indexFile, self*BLOCK_SIZE + OFFSET_LENGTH, SEEK_SET);
	fread(block, BLOCK_SIZE, 1, indexFile);
	Treenode<key, value> * node;
	node = new Treenode<key, value>(block, keyLen, valueLen, indexFileName);
	cout << "Bplustree::test() " << endl;
	cout << "node data :" << endl;
	node->printTreeNode();

	Treenode<key, value> * parent;
	Treenode<key, value> * leftNode;
	parent = this->getParent(node->getKey(0), node->getSelf());
	leftNode = this->getLeftNodeAll(node, parent);

	cout << "leftNode self = " << leftNode->getSelf() << endl;
	cout << "leftNode data:" << endl;
	leftNode->printTreeNode();

	delete node;
//	delete leftNode;
	fclose(indexFile);
}


//------------------------------------
//TreeNode
//------------------------------------
template<typename key, typename value>
Treenode<key,value>::Treenode(int keyLen, int valueLen, unsigned long int self,int type, const char * indexFileName) {
	this->keyLen = keyLen;
	this->valueLen = valueLen;
	this->self = self;
//	this->parent = parent;
	this->type = type;
	this->count = 0;
	this->indexFileName = indexFileName;

	data = (char*)malloc(TREE_NODE_DATA_SIZE);
	change = false;
	treeNodeMaxSize = (TREE_NODE_DATA_SIZE - valueLen)/(keyLen + valueLen);
	this->setNext(-1);
}
template<typename key, typename value>
Treenode<key,value>::Treenode(const char * block, int keyLen, int valueLen, const char * indexFileName) {
	this->keyLen = keyLen;
	this->valueLen = valueLen;
	this->indexFileName = indexFileName;
	data = (char*)malloc(TREE_NODE_DATA_SIZE);
	change = false;
	treeNodeMaxSize = (TREE_NODE_DATA_SIZE - valueLen)/(keyLen + valueLen);
//	this->setNext(-1);
	parsed(block);
}

template<typename key, typename value>
Treenode<key,value>::~Treenode() {
	if (change) {
		writeBack();
	}
	free(data);
}

//插入后还应该保持有序
template<typename key, typename value>
void Treenode<key, value>::addData(key k, value v) {
	char * d = data;
	if (count == 0) {	//直接添加
		memcpy(d, (char*) &v, valueLen);		//value都是unsigned long int类型
		d += valueLen;
//		string str = typeid(string).name();
//		if (typeid(key).name() == str) {		//string 类型
//			char * kk = (char*)malloc(keyLen);
//			const char *a = k.c_str();
//			memcpy(kk, a, k.size());
//			kk[k.size()] = '\0';
//			memcpy(d, kk, keyLen);
//			free(kk);
////			free(a);
//		} else {
			memcpy(d, (char*) &k, keyLen);		//其他类型 int、float、double
//		}

	} else {
		unsigned int len = valueLen + keyLen;
		unsigned int n = 0;
		while (n < count && getKey(n) < k) {
			n++;
		}
		//插入位置i中，其他往后串 memmove
		char * d0, *d1;
		d1 = data + len * count;
		d0 = d1 - len;
		unsigned int i = count;
		while (i > n) {
			memmove(d1, d0, len);
			d0 -= len;
			d1 -= len;
			i--;
		}
		//插入
		d = data + len * n;
		memcpy(d, (char*) &v, valueLen);
		d += valueLen;

//		string str = typeid(string).name();
//		if (typeid(key).name() == str) {		//string 类型
//			char * kk = (char*)malloc(keyLen);
//			const char *a = k.c_str();
//			memcpy(kk, a, k.size());
//			kk[k.size()] = '\0';
//			memcpy(d, kk, keyLen);
//			free(kk);
////			free(a);
//		} else {
			memcpy(d, (char*) &k, keyLen);
//		}
	}
	count++;
	change = true;
}
/**
 * 将键值为k的项删除（叶结点）
 */
template<typename key, typename value>
void Treenode<key, value>::delData(key k, value v) {
//	char * d = data;
	unsigned int index = 0;
	while(index < count && getKey(index) != k) {
		index++;
	}
	//没找到
	if (index >= count) {
		throw KeyNotFoundException(to_string(k));
	}

	while (index < count && getKey(index) == k && getValue(index) != v) {
		index++;
	}

	int len = keyLen + valueLen;
	if (index == (count - 1)) {
		count--;
	} else {
		char * d0 = data + len*index;
		char * d1 = d0 + len;
		for (unsigned int i = index; i < count - 1; i++) {
			memmove(d0, d1, len);
			d0 += len;
			d1 += len;
		}
		count--;
	}
	change = true;
}
/**
 * 将value为v的项删除（内结点）
 */
template<typename key, typename value>
key Treenode<key, value>::delInnerData(value v) {
//	char * d = data;
	int len = keyLen + valueLen;
	unsigned int index = 0;
	while (index <= count && getValue(index) != v) {
		index++;
	}
	if (index > count) {
		throw range_error("range out of index");
	}
	char * d0, *d1;
	//定位
	if (index == 0) {
		d0 = data;
		d1 = data + len;
	} else {
		d0 = data + len*index - keyLen;
		d1 = d0 + len;
	}
	//获取k
	key k;
	string str = typeid(string).name();
	char * kk;
	if (index == 0) {
		kk = d0 + valueLen;
	} else {
		kk = d0;
	}
//	if (typeid(key).name() == str) {
//		char * p = (char*)malloc(keyLen);
//		memcpy(p, kk, keyLen);
//		string s(p);
//		k = s;
//		free(p);
//	} else {
		memcpy((char*)&k, kk, keyLen);
//	}
	//移动
	for (unsigned int i = index; i < count; i++) {
		memmove(d0, d1, len);
		d0 += len;
		d1 += len;
	}
	count--;
	change = true;

	return k;
}
template<typename key, typename value>
void Treenode<key, value>::addInnerData(key k, value v) {
	unsigned int len = keyLen + valueLen;
	unsigned int n = 0;
	while (n < count && getKey(n) < k) {
		n++;
	}
	//插入位置i中，其他往后串 memmove
	char * d0, *d1;
	d1 = data + len * count + valueLen;
	d0 = d1 - len;
	unsigned int i = count;
	while (i > n) {
		memmove(d1, d0, len);
		d0 -= len;
		d1 -= len;
		i--;
	}

	char * d = data + len * n + valueLen;

	//先放key
//	string str = typeid(string).name();
//	if (typeid(key).name() == str) {		//string 类型
//		char * kk = (char*)malloc(keyLen);
//		const char *a = k.c_str();
//		memcpy(kk, a, k.size());
//		kk[k.size()] = '\0';
//		memcpy(d, kk, keyLen);
//		free(kk);
////		free(a);
//	} else {
		memcpy(d, (char*) &k, keyLen);
//	}
	d += keyLen;
	//后放value
	memcpy(d, (char*)&v, valueLen);

	count++;
	change = true;
}
template<typename key, typename value>
void Treenode<key, value>::addFirstInnerData(value left, key k, value right) {
	char * d = data;
	memcpy(d, (char*) &left, valueLen);
	d += valueLen;
//	string str = typeid(string).name();
//	if (typeid(key).name() == str) {			//string 类型
//		char * kk = (char*) malloc(keyLen);
//		const char *a = k.c_str();
//		memcpy(kk, a, k.size());
//		kk[k.size()] = '\0';
//		memcpy(d, kk, keyLen);
//		free(kk);
////		free(a);
//	} else {
		memcpy(d, (char*) &k, keyLen);		//其他类型 int、float、double
//	}
	d += keyLen;
	memcpy(d, (char*)&right, valueLen);
	count = 1;
	change = true;
}
/**
 * 将右边的结点合并到自个身上,*****注意：如果是叶结点的话应该把相应的链接叶要复制
 * ******仅叶结点有效******
 */
template<typename key, typename value>
void Treenode<key,value>::addRightNodeData(Treenode<key, value> * rightNode) {
	int len = keyLen + valueLen;

	char * leftData = data + len*count;
	char * rightData = rightNode->getData();
	int rightCount = rightNode->getCount();

	int rightDataLen = rightCount*len;
	memcpy(leftData, rightData, rightDataLen);

	//设置数目
	count += rightCount;

	//设置叶结点的链接
	if (type == 1) {
		unsigned long int rightNodeNext = rightNode->getNext();
		this->setNext(rightNodeNext);
	}
	change = true;
}
/**
 * 将左边的结点合并到自个身上,右边的数据需要移动
 * ******仅叶结点有效******
 */
template<typename key, typename value>
void Treenode<key,value>::addLeftNodeData(Treenode<key, value> * leftNode) {
	int len = keyLen + valueLen;
	int leftCount = leftNode->getCount();
	//自身数据右移leftCount个单位
	char * d0 = data + len*(count - 1);
	char * d1 = d0 + len*leftCount;
	//自身右移
	for (unsigned int i = 0; i < count; i++) {
		memmove(d1, d0, len);
		d0 -= len;
		d1 -= len;
	}

	//添加leftNode的数据
	char * rightData = data;
	char * leftData = leftNode->getData();
	memcpy(rightData, leftData, leftCount*len);

	//设置左左边的next
//	leftLeftNode->setNext(self);
//	leftLeftNode->setChange(true);

	count += leftCount;
	change = true;
}
/**
 *
 */
template<typename key, typename value>
void Treenode<key,value>::addRightNodeData_Inner(Treenode<key, value> * rightNode, key k) {
	int len = keyLen + valueLen;
	char * left = data + len*count + valueLen;

	//先把k加到自身后面
//	string str = typeid(string).name();
//	if (typeid(key).name() == str) {
//		char * p = (char*)malloc(keyLen);
//		const char * s = k.c_str();
//		memcpy(p, s, k.size());
//		p[k.size()] = '\0';
//		memcpy(left, p, keyLen);
//		free(p);
//		free(s);
//	} else {
		memcpy(left, (char*)&k, keyLen);
//	}
	left += keyLen;
	int rightCount = rightNode->getCount();
	char * rightData = rightNode->getData();
	memcpy(left, rightData, rightCount*len + valueLen);
	count += (rightCount + 1);
	change = true;
}
/**
 *
 */
template<typename key, typename value>
void Treenode<key,value>::addLeftNodeData_Inner(Treenode<key, value> * leftNode, key k) {
	int len = keyLen + valueLen;
	int leftCount = leftNode->getCount();
	//数据先右移 右移leftCount+1个
	char * d0 = data + len*(count - 1);
	char * d1 = d0 + len*(leftCount + 1);
	for (unsigned int i = 0; i < count; i++) {
		memmove(d1, d0, len);
		d0 -= len;
		d1 -= len;
	}
	//把leftNode数据从头放
	d0 = data;
	char * leftData = leftNode->getData();
	memcpy(d0, leftData, len*leftCount + valueLen);		//注意这个valueLen
	d0 += len*leftCount;
	//之后再放k
//	string str = typeid(string).name();
//	if (typeid(key).name() == str) {
//		char * p = (char*)malloc(keyLen);
//		const char * s = k.c_str();
//		memcpy(p, s, k.size());
//		p[k.size()] = '\0';
//		memcpy(d0, p, keyLen);
//		free(p);
//		free(s);
//	} else {
		memcpy(d0, (char*)&k, keyLen);
//	}
	count += (leftCount + 1);
	change = true;
}
template<typename key, typename value>
void Treenode<key,value>::parsed(const char * block) {
	const char * b = block;
	//self
	memcpy((char*)&self, b, sizeof(self));
	b += sizeof(self);

	//type
	memcpy((char*)&type, b, sizeof(type));
	b += sizeof(type);

	//count
	memcpy((char*)&count, b, sizeof(count));
	b += sizeof(count);

	//parent
//	memcpy((char*)&parent, b, sizeof(parent));
//	b += sizeof(parent);

	//next
//	memcpy((char*)&next, b, sizeof(next));
//	b += sizeof(next);

	//data
	memcpy(data, b, TREE_NODE_DATA_SIZE);
}

template<typename key, typename value>
void Treenode<key,value>::writeBack() {
//	cout << "write back" << endl;
//	cout << "type = " << this->type << endl;
//	cout << "self = " << this->self << endl;
//	cout << "count = " << this->count << endl;

	char * block = (char*)malloc(BLOCK_SIZE);
	char * b = block;
	//self
	memcpy(b, (char*)&self, sizeof(self));
	b += sizeof(self);

	//type
	memcpy(b, (char*)&type, sizeof(type));
	b += sizeof(type);

	//count
	memcpy(b, (char*)&count, sizeof(count));
	b += sizeof(count);

	memcpy(b, data, TREE_NODE_DATA_SIZE);

	//write to disk
	FILE * indexFile;
	if ((indexFile = fopen(indexFileName, "rb+")) == NULL) {
		throw FileNotFoundException(indexFileName);
	}
	fseek(indexFile, self*BLOCK_SIZE + OFFSET_LENGTH, SEEK_SET);
	fwrite(block, BLOCK_SIZE, 1, indexFile);

//	cout << "TreeNode::writeBack() self = " << self << "  ftell() = " << ftell(indexFile) << endl;
//	this->printTreeNode();
	change = false;
	free(block);
	fclose(indexFile);
}
template<typename key, typename value>
char * Treenode<key, value>::getData() {
	return data;
}
template<typename key, typename value>
unsigned long int Treenode<key,value>::getSelf() {
	return self;
}
template<typename key, typename value>
void Treenode<key, value>::setSelf(unsigned long int self) {
	this->self = self;
}
//template<typename key, typename value>
//unsigned long int TreeNode<key,value>::getParent() {
//	return parent;
//}

template<typename key, typename value>
int Treenode<key,value>::getType() {
	return type;
}
template<typename key, typename value>
void Treenode<key, value>::setType(int type) {
	this->type = type;
}
template<typename key, typename value>
int Treenode<key,value>::getCount() {
	return count;
}
template<typename key, typename value>
void Treenode<key, value>::setCount(int count) {
	this->count = count;
}
template<typename key, typename value>
void Treenode<key, value>::setChange(bool change) {
	this->change = change;
}
template<typename key, typename value>
bool Treenode<key, value>::getChange() {
	return change;
}
template<typename key, typename value>
key Treenode<key,value>::getKey(int index) {
	key k;
	const char * d = data;
	unsigned int len = keyLen + valueLen;
	d = d + len*index;
	d += valueLen;
//	string str = typeid(string).name();
//	if (typeid(key).name() == str) {
//		char * p = (char*)malloc(keyLen);
//		memcpy(p, d, keyLen);
//		string str(p);
//		delete p;
//		k = str;
//	} else {
		memcpy((char*)&k, d, keyLen);
//	}
	return k;
}

/*
 * 返回下标对应的关键词的指针（地址）
 */
template<typename key, typename value>
value Treenode<key,value>::getValue(int index) {
	value v;
	const char * d = data;
	unsigned int len = keyLen + valueLen;
	d = d + len*index;
	memcpy((char*)&v, d, valueLen);
	return v;
}

template<typename key, typename value>
int Treenode<key, value>::binarySearch(key k) {
	int low = 0;
	int high = count-1;
	while (low <= high) {
		int mid = (low+high)/2;
		if (k == getKey(mid)) {
			return mid;
		} else if (k < getKey(mid)) {
			high = mid - 1;
		} else {
			low = mid + 1;
		}
	}
	return -1;
}
/*
 * 返回值为value,即下一个子节点的指针
 */
template<typename key, typename value>
unsigned long int Treenode<key, value>::getNextChild(key k) {
	for (unsigned int i = 0; i < count; i++) {	//count不可能为0，因为如果count小于某一个值后两个节点将会被合并
		key ki = getKey(i);
		if (ki >= k) {
			if (ki == k) {
				return getValue(i+1);
			} else {
				return getValue(i);
			}
		}
	}
	return getValue(count);
}
/*
 * 叶结点分裂
 */
template<typename key, typename value>
pair<key, value> Treenode<key, value>::splitData(Treenode<key, value> * right) {
	char * rightData = right->getData();
	unsigned long int next = this->getNext();

	int leftCount = count/2;
	int len = keyLen + valueLen;
	char * d = data + leftCount*len;
	memcpy(rightData, d, (count-leftCount)*len);	//将原有数据复制一半到新的结点中

	right->setCount(count-leftCount);				//跟新右边数据的个数  先
	count = leftCount;								//更新左边数据的个数  后

	right->setNext(next);							//右边部分的next应该是原有数据的next
	this->setNext(right->getSelf());				//左边部分的next应该指向新的结点

	pair<key, value> p;								//分裂后产生一组数据插入父结点中
	p.first = right->getKey(0);						//键值为该结点第一个键的键值
	p.second = right->getSelf();						//数据域应该是该结点的id

	this->setChange(true);
	right->setChange(true);

	return p;
}
/**
 *非叶结点分裂
 */
template<typename key, typename value>
pair<key, value> Treenode<key, value>::splitInnerData(Treenode<key, value> * right) {
	char * rightData = right->getData();

	pair<key, value> p;
	p.first = this->getKey(count/2);
	p.second = right->getSelf();

	int leftCount = count/2;
	int len = keyLen + valueLen;

	char * leftData = data + valueLen + len*leftCount + keyLen;
	memcpy(rightData, leftData, valueLen);
	leftData += valueLen;
	rightData += valueLen;
	memcpy(rightData, leftData, len*(count-leftCount-1));

	right->setCount(count-leftCount-1);
	this->count = leftCount;

	this->setChange(true);
	right->setChange(true);

	return p;
}
/**
 * 从左边结点（leftNode)移动一位数据到右边
 */
template<typename key, typename value>
void Treenode<key, value>::moveRight(Treenode<key, value> * leftNode) {
	int len = keyLen + valueLen;
	char * d1 = data + len*(count + 1);		//如果是内节点的话需要移动count+1项，不是的话都行,不却它这一项
	char * d0 = d1 - len;
	//d0 >>> d1
	for (unsigned int i = 0; i <= count; i++) {
		memmove(d1, d0, len);
		d0 -= len;
		d1 -= len;
	}
	char * leftData = leftNode->getData();
	int leftCount = leftNode->getCount();
	//左边结点最右边的一项数据
	if (leftNode->getType() == 1) {		//leftNode为叶结点
		char * left = leftData + len*(leftCount - 1);
		d0 = data;
		memcpy(d0, left, len);
	} else {							//leftNode为内结点
		char * left = leftData + len*leftCount;
		d0 = data;
		memcpy(d0, left, valueLen);
		left -= keyLen;
		d0 += valueLen;
		memcpy(d0, left, keyLen);
	}


	//改变数据的个数
	leftNode->setCount(leftCount - 1);
	count++;
	//改动标志
	change = true;
	leftNode->setChange(true);
}
/**
 * 从右边结点（rightNode）移动一位数据到左边
 */
template<typename key, typename value>
void Treenode<key, value>::moveLeft(Treenode<key, value> * rightNode) {
	int len = keyLen + valueLen;
	char * rightData = rightNode->getData();
	char * d;
	if (type == 1){
		d = data + len*count;
		memcpy(d, rightData, len);
	} else {
		d = data + len*count + valueLen;
		rightData += valueLen;
		memcpy(d, rightData, keyLen);
		d += keyLen;
		rightData -= valueLen;
		memcpy(d, rightData, valueLen);
	}
	//右边结点整体左移一位
	char * d0 = rightNode->getData();
	char * d1 = d0 + len;
	int rightCount = rightNode->getCount();
	for (int i = 0; i < rightCount; i++) { //如果是非叶结点的话需要移动rightCount次，不是的话也行
		memmove(d0, d1, len);
		d0 += len;
		d1 += len;
	}
	//改变两结点数据个数
	count += 1;
	rightNode->setCount(rightCount - 1);
	//改动标志
	change = true;
	rightNode->setChange(true);
}
/**
 * 将值为v的项的索引置为k
 */
template<typename key, typename value>
void Treenode<key, value>::updataKey(key k, value v) {
	unsigned int n = 0;
	while(n <= count && this->getValue(n) != v) {
		n++;
	}
	int len = keyLen + valueLen;
	char * d = data + len*(n - 1) + valueLen;
	//替换k
//	string str = typeid(string).name();
//	if (typeid(key).name() == str) {
//		char * kk = (char*)malloc(keyLen);
//		const char * a = k.c_str();
//		memcpy(kk, a, k.size());
//		kk[k.size()] = '\0';
//		memcpy(d, kk, keyLen);
//		free(kk);
//		free(a);
//	} else {
		memcpy(d, (char*)&k, keyLen);
//	}
}
/**
 * 将 v0 改成 v1
 */
template<typename key, typename value>
void Treenode<key, value>::updataValue(value v0, value v1) {
	unsigned int index = 0;
	while (index <= count && getValue(index) != v0) {
		index++;
	}
	int len = keyLen + valueLen;
	char * d = data + len*index;
	memcpy(d, (char*)&v1, valueLen);
	change = true;
}
//叶结点的下一个结点
template<typename key, typename value>
long int Treenode<key, value>::getNext() {
	int len = keyLen + valueLen;
	const char * d = data + treeNodeMaxSize*len;
	long int next;
	memcpy((char*)&next, d, sizeof(next));
	return next;
}
template<typename key, typename value>
void Treenode<key, value>::setNext(long int next) {
	int len = keyLen + valueLen;
	char * d = data + treeNodeMaxSize*len;
	memcpy(d, (char*)&next, sizeof(next));
}

template<typename key, typename value>
void Treenode<key, value>::copyData(Treenode<key, value> * node) {
	char * d = data;
	char * nodeData = node->getData();
//	this->self = node->getSelf();			//这个不能复制
	this->type = node->getType();
	this->count = node->getCount();
	memcpy(d, nodeData, (keyLen + valueLen)*count);
	this->setNext(node->getNext());
	this->change = true;
}

template<typename key, typename value>
void Treenode<key, value>::printTreeNode() {
	key k;
	value v;
	char * d = data;
//	bool flag = false;
//	char * kk = (char*)malloc(keyLen);

//	string str = typeid(string).name();
//	if (typeid(key).name() == str) {
//		flag = true;
//	}

	for (unsigned int i = 0; i < count; i++) {
		memcpy((char*)&v, d, valueLen);
		d += valueLen;
//		if (flag) {
//			memcpy(kk, d, keyLen);
//			k = string(kk);
//		} else {
			memcpy((char*)&k, d, keyLen);
//		}
		d += keyLen;
		cout << k << " " << v << endl;
	}
}







#endif /* TOOLS_HEAD_Bplustree_H_ */
