// Copyright 2011, ZhongYi Inc
// Author: Windriver < http://windriver.github.io >

#ifndef __INCLUDE_SEARCHUTILITY_H__
#define __INCLUDE_SEARCHUTILITY_H__

#include "dictdef.h"

//每个汉字的位置最多可能对应的偏移数量, 在LayerTrie中使用
const static size_t kMaxOffsetNum = 500;

const static size_t kMaxLmaIdSetNum = 100;

//此struct转为方便排序而设
struct LemmaIdSet
{
	LemmaIdType cur_lemma_id;
	LemmaIdType least_psb_id;
};

const static size_t kMaxSingleHanziCandNum = 1500;
const static size_t kMaxDoubleHanziCandNum = 2000;
const static size_t kMaxThreeHanziCandNum = 300;
const static size_t kMaxFourHanziCandNum = 100;

//整句状态结点
struct SentenceStateNode
{
	float psb_sum;
	size_t from;
	char16 sentence[kMaxLemmaSize + 1];
	LemmaIdType lemma_id;
	size_t lemma_len;
};

//词元Id 和概率
struct LmaPsbItem
{
	LemmaIdType lemma_id;
	float lemma_psb;
};




#endif
