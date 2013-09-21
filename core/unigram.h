// Copyright 2011, ZhongYi-i  Inc
// Author: Windriver < http://windriver.github.io >

﻿#ifndef __INCLUDE_UNIGRAM_H__
#define  __INCLUDE_UNIGRAM_H__

#include <stdio.h>
#include "dictdef.h"

class UniGram
{
private:
	size_t lemma_num_;
	float* freq_;

public:
	UniGram();
	~UniGram();

	bool load_uni_gram();

	float get_uni_psb(LemmaIdType lemma_id);

	//调试用, 发布版应该删除它
	void stat_uni_gram();
};






#endif //__INCLUDE_UNIGRAM_H__