// Copyright 2011, ZhongYi-i  Inc
// Author: Windriver < http://windriver.github.io >

#include "imengine.h"


ImEngine::ImEngine()
{
	cur_sy_num_ = 0;
	fixed_hanzi_len_ = 0;

	for(size_t i = 0; i <= kMaxLemmaSize; i++) {
		total_cand_num_[i] = 0;
		picked_cand_num_[i] = 0;
	}
}

ImEngine::~ImEngine()
{

}

bool ImEngine::init()
{
	layer_trie_ = new LayerTrie();
	uni_gram_ = new UniGram();
	lemma_fetch_ = new LemmaFetch();

	if( NULL == layer_trie_ || NULL == uni_gram_ || NULL == lemma_fetch_ ) {
		return false;
	}

	if( !layer_trie_->load_layer_trie() || !uni_gram_->load_uni_gram() || !lemma_fetch_->load_lemma_str() ) {
		delete layer_trie_;
		delete uni_gram_;
		delete lemma_fetch_;
		return false;
	}

	candidate_[1] = new LemmaIdType[kMaxSingleHanziCandNum];
	candidate_[2] = new LemmaIdType[kMaxDoubleHanziCandNum];
	candidate_[3] = new LemmaIdType[kMaxThreeHanziCandNum];
	candidate_[4] = new LemmaIdType[kMaxFourHanziCandNum];

	for(size_t i = 1; i <= 4; i++) {
		if( NULL == candidate_[i] ) {
			delete []candidate_[1];
			delete []candidate_[2];
			delete []candidate_[3];
			delete []candidate_[4];
			return false;
		}
	}

	return true;
}

void ImEngine::reset_im_engine()
{
	cur_sy_num_ = 0;
	fixed_hanzi_len_ = 0;

	for(size_t i = 0; i <= kMaxLemmaSize; i++) {
		total_cand_num_[i] = 0;
		picked_cand_num_[i] = 0;
	}

	layer_trie_->reset_layer_tree();
}

bool ImEngine::process__input( ShyunIdType sy_id )
{
	if(cur_sy_num_ < 0 || cur_sy_num_ > kMaxSyIdNum) {
		return false; //声韵母数量达到上限
	}

	cur_sy_num_++; //声韵母数量先自增

	//下面对输入的声韵Id在多层Trie树里面进行搜索, 并且返回整句串和新增加的LemmaIdArray
	bool ret_flag = false;
	set_num_ = 0;
	if(cur_sy_num_ % 2 == 1) { //当用户输入声母时
		ret_flag = layer_trie_->process_shmu(sy_id, lma_id_set_, &set_num_);
	}
	else {										//当用户输入韵母时
		ret_flag = layer_trie_->process_yunmu(sy_id, lma_id_set_, &set_num_);
	}

	if( !ret_flag ) {
		return false;
	}

	//调试用, 稍后删去
// 	setlocale(LC_ALL, "chs");
// 	char16 lemma_buf[kMaxLemmaSize + 1];
// 	for(size_t i = 0; i < set_num_; i++) {
// 		wprintf(L"%ls ", lemma_fetch_->get_lemma_by_id(lma_id_set_[i].cur_lemma_id, lemma_buf));
// 	}

	//先求得这次的声韵输入新增加的候选词的长度(若发现大于 4 ,则不准备候选词, 因为系统词典中无4字以上候选词)
	size_t lma_len = (cur_sy_num_ - 1) / 2 - fixed_hanzi_len_ + 1;
	if( lma_len <= 4 ) {
		prepare_candidates(10, lma_len);
	}

	return true;
}

size_t ImEngine::prepare_candidates( size_t cand_num_needed, size_t lma_len )
{
	//要先把此长度的候选词数组清空
	picked_cand_num_[lma_len] = 0;

	size_t cand_num_picked = 0;
	while(cand_num_needed > 0) {
		//先遍历LmaIdSet, 找出其中cur_lma_id最大者,
		float psb_of_max_ = 0;
		size_t pos_of_max = set_num_;
		for(size_t i = 0; i < set_num_; i++) {
			if( lma_id_set_[i].cur_lemma_id >= lma_id_set_[i].least_psb_id && uni_gram_->get_uni_psb(lma_id_set_[i].cur_lemma_id) > psb_of_max_ ) {
				psb_of_max_ = uni_gram_->get_uni_psb(lma_id_set_[i].cur_lemma_id);
				pos_of_max = i;
			} //if
		} //for

		if(pos_of_max == set_num_) {
			cand_num_needed--;
			continue; //表明此轮未找到
		}

		assert(pos_of_max >= 0 && pos_of_max < set_num_);

		//已经找到了此Id, 在相应的候选词数组中加入此Id
		candidate_[ lma_len ][ picked_cand_num_[ lma_len ] ] = lma_id_set_[pos_of_max].cur_lemma_id;
		picked_cand_num_[lma_len]++;
		lma_id_set_[pos_of_max].cur_lemma_id--;
		cand_num_picked++;
		cand_num_needed--;
	} //while

	//调试用, 稍后删去
	setlocale(LC_ALL, "chs");
	char16 lemma_buf[kMaxLemmaSize + 1];
	for(size_t len = 4; len >= 1; len--) {
		for(size_t i = 0; i < picked_cand_num_[len]; i++) {
			wprintf( L"%ls ", lemma_fetch_->get_lemma_by_id( candidate_[len][i], lemma_buf ) );
//			printf( "%f \n", uni_gram_->get_uni_psb( candidate_[len][i]) );
		}
	}
	for(size_t i = 0; i < picked_cand_num_[lma_len]; i++) {

	}

	return cand_num_picked;
}


