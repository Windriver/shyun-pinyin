// Copyright 2011, ZhongYi Inc
// Author: Windriver < http://windriver.github.io >

﻿// Copyright 2011, ZhongYi Inc.
// Author: Windriver < http://windriver.github.io/ >

#ifndef  __INCLUDE_DICTBUILDER_H__
#define __INCLUDE_DICTBUILDER_H__

#include <stdio.h>
#include <stdlib.h>

#include <queue>

#include "dictdef.h"
#include "pinyinmap.h"
#include "utf16char.h"
#include "utf16reader.h"

//临时树, 先把词库里的Lemma信息插入到这里, 然后整理到多层Trie树里
struct TempNode
{
	uint16 node_type;
	uint16 sy_id;
	uint16 lemma_num;
	LemmaIdType lemma_id_1st;
	TempNode* right;
	TempNode* down;
};

class DictBuilder
{
private:
	//从拼音到声韵的转换类, 以后要将此类改为static风格的
	PinyinMap pinyin_map_;

	//临时树的根结点
	TempNode* root_temp_;

	//用来统计树中每层的Lemma的数量, (一个结点有lemma_num个Lemma)
	size_t lemma_num_lay_[20];

	//Trie结点中存放Lemma Id的偏移, 用偏移加上此数组中对应层的值, 得到真正Lemma Id
	//此举是为了减少内存的占用
	size_t total_lemma_num_before_lay_[20];

	//多层Trie的9个数组
	LmaNodeLayer00* lma_node_lay0_;
	LmaNodeLayer01* lma_node_lay1_;
	LmaNodeLayer02* lma_node_lay2_;
	LmaNodeLayerSm* lma_node_lay3_;
	LmaNodeLayerYm* lma_node_lay4_;
	LmaNodeLayerSm* lma_node_lay5_;
	LmaNodeLayerYm* lma_node_lay6_;
	LmaNodeLayerSm* lma_node_lay7_;
	LmaNodeLayerYm* lma_node_lay8_;

	//当建立多层Trie树时, 它记录当前第 i 层的结点总数量, 用来确定各层当前该处理的结点
	size_t cur_node_num_lay_[20];

	//当建立多层Trie树时, 它记录第 i 层的结点的之前所有结点的子链的累积长度(偏移), 用来确定本结点的 son_off_1st
	size_t cur_son_num_lay_[20];

	//从词典中成功读取的词元的数量
	size_t lemma_num_;

	//这个用来临时存储 unigram 信息, 即词元频率的Log
	float freq_[70000];

	//用来记录词元频度的总和,  以求出每个词元的频率
	double total_freq_sum_;

	//这个 4 个数组表示每个系统 Lemma Id 对应的 Lemma(词元)
	//因为系统词典词元长度最大为四, 所以暂时只有这4个数组
	char16* single_word_;
	char16* double_word_;
	char16* three_word_;
	char16* four_word_;

	//统计每类Lemma的数量
	size_t lemma_num_word_[kMaxLemmaSize + 1];

public:
	DictBuilder();
	~DictBuilder();

	bool build_dict();

private:
	//以二进制形式保存字典, 字典中包括多层数组Trie树, 以及...
	bool save_dict();

	//分配资源, 特别是那4个超大数组
	bool alloc_resource();

	size_t read_raw_dict(const char* filename);

	//向root_temp_代表的那棵树上尝试插入一个节点, 失败则返回false
	bool insert_to_temp_tree( uint16* sy_id_arr, size_t sy_num, LemmaIdType lemma_id );

	//统计temp tree
	void stat_temp_tree();

	//层序遍历temp tree, 并生成最终多层Trie树
	bool build_layer_trie();

	//被build_layer_trie()调用, 将结点加入相应层中
	bool insert_to_layer_trie( TempNode* temp_node, uint16 lay_id,
												size_t* max_lemma_num_this_layer, size_t* max_son_num_this_layer );

	//统计多层数组Trie树
	void stat_layer_trie();

	//统计 unigram
	void stat_uni_gram();

	//存储 unigram
	bool save_uni_gram();

	//统计Lemma本体的情况
	void stat_lemma_str();

	//存储Lemma本体
	bool save_lemma_str();

};

#endif //__INCLUDE_DICTBUILDER_H__


