// Copyright 2011, ZhongYi-i  Inc
// Author: Windriver < http://windriver.github.io >

﻿#ifndef  __INCLUDE_IMENGINE_H__
#define __INCLUDE_IMENGINE_H__

#include <stdio.h>

#include "dictdef.h"
#include "layertrie.h"
#include "unigram.h"
#include "lemmafetch.h"
#include "searchutility.h"

class ImEngine
{
private:
	LayerTrie* layer_trie_;
	UniGram* uni_gram_;
	LemmaFetch* lemma_fetch_;

	//引擎中当前声韵 Id 的数量
	size_t cur_sy_num_;

	//已经被选中的汉字数量
	size_t fixed_hanzi_len_;

	//用以存放各种长度的候选词的Lemma Id
	LemmaIdType single_word_cand_[100];
	LemmaIdType double_word_cand_[100];
	LemmaIdType three_word_cand_[100];
	LemmaIdType four_word_cand_[100];

	///用以存放LayerTrie搜索出的各种长度的候选词的总数
	size_t total_cand_num_[kMaxLemmaSize + 1];

	//用以存放各种长度的候选词的当前已排出序的个数
	size_t picked_cand_num_[kMaxLemmaSize + 1];

	//暂时存着一些LemmaIdSet. 供归并排序用
	LemmaIdSet lma_id_set_[kMaxLmaIdSetNum];
	size_t set_num_;

	//这个几个数组用来存放各种长度的候选词, 暂时候选词以Lemma Id的形式存放
	LemmaIdType* candidate_[kMaxLemmaSize + 1];

public:
	ImEngine();
	~ImEngine();

	//作用是初始化引擎, 分配资源, 加载词典
	bool init();

	//重置引擎的状态
	void reset_im_engine();

	//处理声韵母输入的函数
	bool process__input( ShyunIdType sy_id );

private:
	size_t prepare_candidates(size_t cand_num, size_t lma_len);

	//
};

#endif //__INCLUDE_IMENGINE_H__