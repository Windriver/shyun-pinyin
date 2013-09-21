// Copyright 2011, ZhongYi-i  Inc
// Author: Windriver < http://windriver.github.io >

#include "unigram.h"

UniGram::UniGram()
{

}

UniGram::~UniGram()
{

}

bool UniGram::load_uni_gram()
{
	FILE* fp = fopen("unigram.dat", "rb");
	if(NULL == fp) {
		return false;
	}

	if( fread( &lemma_num_, sizeof(size_t), 1, fp ) != 1 ) {
		printf("读取词元数目出错 !!\n");
		return false;
	}

	freq_ = new float[lemma_num_ + 1];
	freq_[0] = 0.0;
	if( fread( freq_ + 1, sizeof(float), lemma_num_, fp ) != lemma_num_ ) {
		printf("读取unigram失败 !!\n");
		return false;
	}

	return true;
}

inline float UniGram::get_uni_psb( LemmaIdType lemma_id )
{
	if(lemma_id > 0 || lemma_id <= lemma_num_) {
		return freq_[lemma_id];
	}

	return 0.0;
}

void UniGram::stat_uni_gram()
{
	printf("词典中共有 %d 个词元(lemma) \n", lemma_num_);

	for(size_t i = 0; i <= lemma_num_ - 65000; i++) {
		printf( "Freq[ %d ] = %f , log = %f \n", i, freq_[i], get_uni_psb(i) );
	}
}

