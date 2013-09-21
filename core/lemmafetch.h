// Copyright 2011, ZhongYi-i  Inc
// Author: Windriver < http://windriver.github.io >

﻿#ifndef __INCLUDE_LEMMAFETCH_H__
#define __INCLUDE_LEMMAFETCH_H__

#include <stdio.h>
#include <locale.h>

#include "dictdef.h"

class LemmaFetch
{
private:
	//这个 4 个数组表示每个系统 Lemma Id 对应的 Lemma(词元)
	//因为系统词典词元长度最大为四, 所以暂时只有这4个数组
	//暂时分成多个数组放, 以后可能会合到同一个数组中
	char16* single_word_;
	char16* double_word_;
	char16* three_word_;
	char16* four_word_;

	//这个数组记录着每类Lemma的的首Id, 他们对应着上面数组的首个元素
	size_t first_id_[kMaxLemmaSize + 1];


public:
	LemmaFetch();
	~LemmaFetch();

	//加载lemma本体, 并为first_id_数组赋值
	bool load_lemma_str();

	//通过Lemma Id获取Lemma
	//调用此函数之前要保证load_lemma_str()已经成功执行
	char16* get_lemma_by_id(LemmaIdType lemma_id, char16* lemma_buf);

	//统计lemma_str, debug用, 发布版应该删掉
	void stat_lemma_str();

};


#endif //__INCLUDE_LEMMAFETCH_H__