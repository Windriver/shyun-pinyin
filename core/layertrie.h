// Copyright 2011, ZhongYi Inc
// Author: Windriver < http://windriver.github.io >

#ifndef  __INCLUDE_LAYERTRIE_H__
#define __INCLUDE_LAYERTRIE_H__

#include "dictdef.h"
#include "searchutility.h"

//调试用, 稍后删掉
#include "unigram.h"
#include "lemmafetch.h"

class LayerTrie
{
private:
	//这个两个数组以及下面的9个指针用来表示LayerTrie树
	size_t node_num_lay_[kMaxShyunLayer];
	size_t total_lemma_num_before_lay_[kMaxShyunLayer];

	LmaNodeLayer00* lma_node_lay0_;
	LmaNodeLayer01* lma_node_lay1_;
	LmaNodeLayer02* lma_node_lay2_;
	LmaNodeLayerSm* lma_node_lay3_;
	LmaNodeLayerYm* lma_node_lay4_;
	LmaNodeLayerSm* lma_node_lay5_;
	LmaNodeLayerYm* lma_node_lay6_;
	LmaNodeLayerSm* lma_node_lay7_;
	LmaNodeLayerYm* lma_node_lay8_;

	//下面的成员变量用来表征当前搜索的状态
	//调用reset_layer_tree时, 重置这些状态

	//当前的声韵Id的数量, 以及每次输入的声韵Id序列
	size_t cur_sy_num_;
	uint16 sy_array_[ kMaxSyIdNum + 1 ];

	//总共的汉字数
	size_t hanzi_len_;

	//已经选定的汉字总数
	size_t fixed_hanzi_len_;

	//下面三个都应该是二维数组
	//第一个数组记录了从各个汉字位置开始搜索的各层的所有偏移
	//第二个数组记录了从各个汉字位置开始搜索的各层的偏移在数组中的第一位的序号
	//第三个数组记录了从各个汉字位置开始搜索的各层的偏移在的数量
	uint16 offset_array_[kMaxLemmaSize][kMaxOffsetNum];
	size_t begin_offset_[kMaxLemmaSize][kMaxShyunLayer];
	size_t num_of_offset_[kMaxLemmaSize][kMaxShyunLayer];

	//此数组记录了各个汉字开始搜索的多层Trie偏移了几次, 即到了第几层, 最大为 8
	size_t lay_num_of_hz_[kMaxLemmaSize];

	//以下的成员变量跟整句相关
	SentenceStateNode sentence_state_[kMaxLemmaSize + 1];

	LmaPsbItem lma_psb_item_[kMaxLemmaSize + 1];

	//测试用
	LemmaFetch lemma_fetch_;
	UniGram uni_gram_;

public:
	LayerTrie();
	~LayerTrie();

	bool load_layer_trie();

	//统计trie, 以确保它的正确, debug时用, 发布版本应该删掉它
	void stat_layer_tree();

	bool save_layer_tree();

	void reset_layer_tree();

	//处理声母的输入
	bool process_shmu( ShyunIdType sm_id, LemmaIdSet* lma_id_set, size_t* set_num );

	//处理韵母的输入
	bool process_yunmu( ShyunIdType ym_id, LemmaIdSet* lma_id_set, size_t* set_num );

	//处理删除操作
	bool process_delete();

	//处理选择操作 (所谓选择, 就是用户点击了候选词)
	bool process_select( uint16 select_num );

private:
	//被process_shmu() 和 process_yunmu() 调用
	//当获取偏移之后, 调用它来将每个偏移对应的词元Id组传递给shyunime
	bool get_lemma_id_array(size_t hz_pos, size_t lay_id, LemmaIdSet* lma_id_set, size_t* set_num);

	//更新整句状态,
	char16* generate_full_sentence(size_t travel_num);


};

#endif //__INCLUDE_LAYERTRIE_H__
