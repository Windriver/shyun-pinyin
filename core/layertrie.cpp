// Copyright 2011, ZhongYi Inc
// Author: Windriver < http://windriver.github.io >

#include "layertrie.h"

LayerTrie::LayerTrie()
{
	lma_node_lay0_ = NULL;
	lma_node_lay1_ = NULL;
	lma_node_lay2_ = NULL;
	lma_node_lay3_ = NULL;
	lma_node_lay4_ = NULL;
	lma_node_lay5_ = NULL;
	lma_node_lay6_ = NULL;
	lma_node_lay7_ = NULL;
	lma_node_lay8_ = NULL;

	//先将各个以各个汉字开始搜索的Trie首层offset在数组的起始位和结束位初始化(永远为0)
	for(size_t i = 0; i < kMaxLemmaSize; i++) {
		offset_array_[i][0] = 0;
		begin_offset_[i][0] = 0;
		num_of_offset_[i][0] = 1;
	}

	//整句状态的首节点
	sentence_state_[0].psb_sum = 0;
	sentence_state_[0].from = 0;

	reset_layer_tree();
}

LayerTrie::~LayerTrie()
{
	//暂时先这么处理, 也许根本不需要释放的
	delete []lma_node_lay0_; delete []lma_node_lay1_; delete []lma_node_lay2_;
	delete []lma_node_lay3_; delete []lma_node_lay4_; delete []lma_node_lay5_;
	delete []lma_node_lay6_; delete []lma_node_lay7_; delete []lma_node_lay8_;
	lma_node_lay0_ = NULL; lma_node_lay1_ = NULL; lma_node_lay2_ = NULL;
	lma_node_lay3_ = NULL; lma_node_lay4_ = NULL; lma_node_lay5_ = NULL;
	lma_node_lay6_ = NULL; lma_node_lay7_ = NULL; lma_node_lay8_ = NULL;
}

bool LayerTrie::load_layer_trie()
{
	printf( "***********开始读取Layer Tree ***********\n");
	FILE* fp = fopen("dict_shyun.dat", "rb");
	if(NULL == fp) {
		printf(" !!! 打开加载多层Trie树失败 !!! \n");
		return false;
	}

	for(size_t i = 0; i < kMaxShyunLayer; i++) {
		if( fread( &node_num_lay_[i], sizeof(size_t), 1, fp ) != 1 ) {
			printf("!!! 读第 %d 层结点数量时出错 !!!\n", i);
			return false;
		}
	}

	for(size_t i = 0; i < kMaxShyunLayer; i++) {
		if( fread( &total_lemma_num_before_lay_[i], sizeof(size_t), 1, fp ) != 1 ) {
			printf("!!! 读第 %d 层累积结点数量时出错 !!!\n", i);
			return false;
		}
	}

	//为分层分配存储空间先
	lma_node_lay0_ =  static_cast<LmaNodeLayer00*>
		(malloc(node_num_lay_[0] * sizeof(LmaNodeLayer00)));
	lma_node_lay1_ = new LmaNodeLayer01[ node_num_lay_[1] +1 ];
	lma_node_lay2_ = new LmaNodeLayer02[ node_num_lay_[2] +1 ];
	lma_node_lay3_ = new LmaNodeLayerSm[ node_num_lay_[3] +1 ];
	lma_node_lay4_ = new LmaNodeLayerYm[ node_num_lay_[4] +1 ];
	lma_node_lay5_ = new LmaNodeLayerSm[ node_num_lay_[5] +1 ];
	lma_node_lay6_ = new LmaNodeLayerYm[ node_num_lay_[6] +1 ];
	lma_node_lay7_ = new LmaNodeLayerSm[ node_num_lay_[7] +1 ];
	lma_node_lay8_ = new LmaNodeLayerYm[ node_num_lay_[8] +1 ];

	if( NULL == lma_node_lay0_ || NULL == lma_node_lay1_ || NULL == lma_node_lay2_ ||
		NULL == lma_node_lay3_ || NULL == lma_node_lay4_ || NULL == lma_node_lay5_ ||
		NULL == lma_node_lay6_ || NULL == lma_node_lay7_ || NULL == lma_node_lay8_  ) {
			printf("\n !!! 分配空间失败 !!! \n");
			delete []lma_node_lay0_; delete []lma_node_lay1_; delete []lma_node_lay2_;
			delete []lma_node_lay3_; delete []lma_node_lay4_; delete []lma_node_lay5_;
			delete []lma_node_lay6_; delete []lma_node_lay7_; delete []lma_node_lay8_;
			lma_node_lay0_ = NULL; lma_node_lay1_ = NULL; lma_node_lay2_ = NULL;
			lma_node_lay3_ = NULL; lma_node_lay4_ = NULL; lma_node_lay5_ = NULL;
			lma_node_lay6_ = NULL; lma_node_lay7_ = NULL; lma_node_lay8_ = NULL;

			return false;
	}

	if( fread( lma_node_lay0_, sizeof(LmaNodeLayer00), node_num_lay_[0], fp ) != node_num_lay_[0] ) {
		printf("!!! 读第 0 层结点时出错了 !!!\n");
		return false;
	}
 	if( fread( lma_node_lay1_, sizeof(LmaNodeLayer01), node_num_lay_[1], fp ) != node_num_lay_[1] ) {
 		printf("!!! 读第 1 层结点时出错了 !!!\n");
 		return false;
 	}
 	if( fread( lma_node_lay2_, sizeof(LmaNodeLayer02), node_num_lay_[2], fp ) != node_num_lay_[2] ) {
 		printf("!!! 读第 2 层结点时出错了 !!!\n");
 		return false;
 	}
	if( fread( lma_node_lay3_, sizeof(LmaNodeLayerSm), node_num_lay_[3], fp ) != node_num_lay_[3] ) {
		printf("!!! 读第 3 层结点时出错了 !!!\n");
		return false;
	}
	if( fread( lma_node_lay4_, sizeof(LmaNodeLayerYm), node_num_lay_[4], fp ) != node_num_lay_[4] ) {
		printf("!!! 读第 4 层结点时出错了 !!!\n");
		return false;
	}
	if( fread( lma_node_lay5_, sizeof(LmaNodeLayerSm), node_num_lay_[5], fp ) != node_num_lay_[5] ) {
		printf("!!! 读第 5 层结点时出错了 !!!\n");
		return false;
	}
	if( fread( lma_node_lay6_, sizeof(LmaNodeLayerYm), node_num_lay_[6], fp ) != node_num_lay_[6] ) {
		printf("!!! 读第 6 层结点时出错了 !!!\n");
		return false;
	}
	if( fread( lma_node_lay7_, sizeof(LmaNodeLayerSm), node_num_lay_[7], fp ) != node_num_lay_[7] ) {
		printf("!!! 读第 7 层结点时出错了 !!!\n");
		return false;
	}
	if( fread( lma_node_lay8_, sizeof(LmaNodeLayerYm), node_num_lay_[8], fp ) != node_num_lay_[8] ) {
		printf("!!! 读第 8 层结点时出错了 !!!\n");
		return false;
	}

	if(!uni_gram_.load_uni_gram() || !lemma_fetch_.load_lemma_str()) {
		return false;
	}

	printf( "***********Layer Tree 读取成功!***********\n");

	fclose(fp);
	return true;
}

void LayerTrie::stat_layer_tree()
{
	for(size_t i = 0; i < kMaxShyunLayer; i++ ) {
		printf("第 %d 层共有 %d 个结点\n", i, node_num_lay_[i]);
		printf("第 %d 层之前累积共有 %d 个词元(Lemma)\n", i, total_lemma_num_before_lay_[i]);
	}

	printf("\n");
}

bool LayerTrie::save_layer_tree()
{
	return true;
}

void LayerTrie::reset_layer_tree()
{
	cur_sy_num_ = 0;
	fixed_hanzi_len_ = 0;

	//暂时这么处理, 以后会用memset等高效的手法
	for( size_t i = 0; i < kMaxLemmaSize; i++ ) {
		lay_num_of_hz_[i] = 0;
	}
}

bool LayerTrie::process_shmu( ShyunIdType sm_id, LemmaIdSet* lma_id_set, size_t* set_num )
{
	if( sm_id < 0 || sm_id >= kAllShmuNum ) {
		printf(" !!! 声母 Id 越界, sm_id = %d !!!\n", sm_id);
		return false;
	}

	if(cur_sy_num_ < 0 || cur_sy_num_ >= kMaxSyIdNum) {
		printf(" !!! sy_num 已达到最大值, 不能再输入了 !!! \n");
		return false;
	}

	//先自增以下声韵Id数目
	cur_sy_num_++;
	sy_array_[cur_sy_num_] = sm_id;
	assert( cur_sy_num_ % 2 == 1 );
	hanzi_len_ = (cur_sy_num_ + 1) / 2;
	printf("汉字数量: %d \n", hanzi_len_);

//	size_t search_hz_begin = ( fixed_hanzi_len_ > ( ( cur_sy_num_ /2 ) - kMaxSysLmaSize ) ) ? ( ( cur_sy_num_ /2 ) - kMaxSysLmaSize ) : fixed_hanzi_len_;
	size_t search_hz_begin;
	if( ((cur_sy_num_ - 1) /2 - fixed_hanzi_len_) < kMaxSysLmaSize - 1 ) { //系统词典中的词元长度最大为 4
		search_hz_begin = fixed_hanzi_len_;
	}
	else {
		search_hz_begin = ( (cur_sy_num_ - 1)/2 ) - kMaxSysLmaSize + 1;
	}
	size_t search_hz_end = (cur_sy_num_ - 1) /2;

//	printf("汉字位置: begin = %d, end = %d \n", search_hz_begin, search_hz_end);

	for( size_t hz_pos = search_hz_begin; hz_pos <= search_hz_end; hz_pos++ ) {
		float psb_of_max = kMinLmaFreq;
		LemmaIdType lemma_id_of_max = 0; //初值为为 0 , 后面0可以作为是否有效的判断标志

		size_t lay_num = lay_num_of_hz_[hz_pos]; //先取得此汉字开始搜索的Trie偏移到了几层
		assert( lay_num >= 0 && lay_num <= 8 && lay_num % 2 == 0 );

		size_t first_off_pos = begin_offset_[hz_pos][lay_num];
		size_t last_off_pos = first_off_pos + num_of_offset_[hz_pos][lay_num];
		size_t first_off_pos_next = last_off_pos;
		size_t num_of_off = 0;

		//因为各层的结点不一样, 所以不容易写出通用的代码, 先这样"丑陋"地实现一下吧
		//(其实就 4 行代码是与具体各层相关的)
//		printf("case %d\n", lay_num);
		switch(lay_num) {
		case 0:
			for(size_t off_pos = first_off_pos; off_pos < last_off_pos; off_pos++) {
				uint16 offset = offset_array_[hz_pos][off_pos];
				//先获得上一层的结点
				LmaNodeLayer00 lma_node = lma_node_lay0_[offset];
				LmaNodeLayer01 son_node;
				// 再找到下一层的结点
				for(uint16 next_offset = lma_node.son_off_1st;
					next_offset < lma_node.son_off_1st + lma_node.num_of_son; next_offset++) {
						if(sm_id == lma_node_lay1_[next_offset].sm_id) {
							son_node = lma_node_lay1_[next_offset];
							break;
						}
				}

			    //将下一层的偏移加入数组中
				for(uint16 offset_next = son_node.son_off_1st;
					offset_next < son_node.son_off_1st + son_node.num_of_son; offset_next++) {
						offset_array_[hz_pos][first_off_pos_next + num_of_off] = offset_next;
						num_of_off++;

						if(lma_node_lay2_[offset_next].num_of_lma == 0) {
							continue;
						}
						LemmaIdType lemma_id_this_offset = total_lemma_num_before_lay_[2] + lma_node_lay2_[offset_next].lma_off_1st + lma_node_lay2_[offset_next].num_of_lma - 1;
						if( uni_gram_.get_uni_psb(lemma_id_this_offset) > psb_of_max ) {
							lemma_id_of_max = lemma_id_this_offset;
							psb_of_max = uni_gram_.get_uni_psb(lemma_id_this_offset);
						}
				}
			}
			break;

		case 2:
			for(size_t off_pos = first_off_pos; off_pos < last_off_pos; off_pos++) {
				uint16 offset = offset_array_[hz_pos][off_pos];
				//先获得上一层的结点
				LmaNodeLayer02 lma_node = lma_node_lay2_[offset];
				LmaNodeLayerSm son_node;
				// 再找到下一层的结点
				bool sm_is_found = false;
				for(uint16 next_offset = lma_node.son_off_1st;
					next_offset < lma_node.son_off_1st + lma_node.num_of_son; next_offset++) {
						if(sm_id == lma_node_lay3_[next_offset].sm_id) {
							son_node = lma_node_lay3_[next_offset];
							sm_is_found = true;
							break;
						}
				}

				if( !sm_is_found ){
					continue; //没找到对应的声母, 继续for循环
				}

				//将下一层的偏移加入数组中
				for(uint16 offset_next = son_node.son_off_1st;
					offset_next < son_node.son_off_1st + son_node.num_of_son; offset_next++) {
						offset_array_[hz_pos][first_off_pos_next + num_of_off] = offset_next;
						num_of_off++;

						if(lma_node_lay4_[offset_next].num_of_lma == 0) {
							continue;
						}
						LemmaIdType lemma_id_this_offset = total_lemma_num_before_lay_[4] + lma_node_lay4_[offset_next].lma_off_1st + lma_node_lay4_[offset_next].num_of_lma - 1;
						if( uni_gram_.get_uni_psb(lemma_id_this_offset) > psb_of_max ) {
							lemma_id_of_max = lemma_id_this_offset;
							psb_of_max = uni_gram_.get_uni_psb(lemma_id_this_offset);
						}
				}  //for 2
			}	//for 1
			break;
		case 4:
			for(size_t off_pos = first_off_pos; off_pos < last_off_pos; off_pos++) {
				uint16 offset = offset_array_[hz_pos][off_pos];
				//先获得上一层的结点
				LmaNodeLayerYm lma_node = lma_node_lay4_[offset];
				LmaNodeLayerSm son_node;
				// 再找到下一层的结点
				bool sm_is_found = false;
				for(uint16 next_offset = lma_node.son_off_1st;
					next_offset < lma_node.son_off_1st + lma_node.num_of_son; next_offset++) {
						if(sm_id == lma_node_lay5_[next_offset].sm_id) {
							son_node = lma_node_lay5_[next_offset];
							sm_is_found = true;
							break;
						}
				}

				if( !sm_is_found ){
					continue; //没找到对应的声母, 继续for循环
				}

				//将下一层的偏移加入数组中
				for(uint16 offset_next = son_node.son_off_1st;
					offset_next < son_node.son_off_1st + son_node.num_of_son; offset_next++) {
						offset_array_[hz_pos][first_off_pos_next + num_of_off] = offset_next;
						num_of_off++;

						if(lma_node_lay6_[offset_next].num_of_lma == 0) {
							continue;
						}
						LemmaIdType lemma_id_this_offset = total_lemma_num_before_lay_[6] + lma_node_lay6_[offset_next].lma_off_1st + lma_node_lay6_[offset_next].num_of_lma - 1;
						if( uni_gram_.get_uni_psb(lemma_id_this_offset) > psb_of_max ) {
							lemma_id_of_max = lemma_id_this_offset;
							psb_of_max = uni_gram_.get_uni_psb(lemma_id_this_offset);
						}
				}  //for 2
			}	//for 1
			break;
		case 6:
			for(size_t off_pos = first_off_pos; off_pos < last_off_pos; off_pos++) {
				uint16 offset = offset_array_[hz_pos][off_pos];
				//先获得上一层的结点
				LmaNodeLayerYm lma_node = lma_node_lay6_[offset];
				LmaNodeLayerSm son_node;
				// 再找到下一层的结点
				bool sm_is_found = false;
				for(uint16 next_offset = lma_node.son_off_1st;
					next_offset < lma_node.son_off_1st + lma_node.num_of_son; next_offset++) {
						if(sm_id == lma_node_lay7_[next_offset].sm_id) {
							son_node = lma_node_lay7_[next_offset];
							sm_is_found = true;
							break;
						}
				}

				if( !sm_is_found ){
					continue; //没找到对应的声母, 继续for循环
				}

				//将下一层的偏移加入数组中
				for(uint16 offset_next = son_node.son_off_1st;
					offset_next < son_node.son_off_1st + son_node.num_of_son; offset_next++) {
						offset_array_[hz_pos][first_off_pos_next + num_of_off] = offset_next;
						num_of_off++;

						if(lma_node_lay8_[offset_next].num_of_lma == 0) {
							continue;
						}
						LemmaIdType lemma_id_this_offset = total_lemma_num_before_lay_[8] + lma_node_lay8_[offset_next].lma_off_1st + lma_node_lay8_[offset_next].num_of_lma - 1;
						if( uni_gram_.get_uni_psb(lemma_id_this_offset) > psb_of_max ) {
							lemma_id_of_max = lemma_id_this_offset;
							psb_of_max = uni_gram_.get_uni_psb(lemma_id_this_offset);
						}
				}  //for 2
			}	//for 1
			break;
		default:
			printf(" !!! 错误的 lay_num = %d\n", lay_num);
			reset_layer_tree();
			return false;
		}

		//输入的声韵序号在此汉字开始的解码中无子节点. 这种情况可以统一看待
// 		if( 0 == num_of_off ) {
// 			printf("这次输入的声母Id = %d , 没有子节点 !! \n", sm_id);
// 		}

		//确定下一层偏移们在偏移数组中的起始位置和数量
		lay_num++;
		begin_offset_[hz_pos][lay_num] = first_off_pos_next;
		num_of_offset_[hz_pos][lay_num] = num_of_off;
		lay_num_of_hz_[hz_pos] = lay_num;

		if(lemma_id_of_max == 0) {
			lma_psb_item_[hz_pos - search_hz_begin].lemma_id = 0;
			lma_psb_item_[hz_pos - search_hz_begin].lemma_psb = -1000; //
		}
		else {
			lma_psb_item_[hz_pos - search_hz_begin].lemma_id = lemma_id_of_max;
			lma_psb_item_[hz_pos - search_hz_begin].lemma_psb = psb_of_max;
		}
//		printf("添加了 %d 个偏移, 在第 %d 个汉字 第 %d 层.  ", num_of_off, hz_pos, lay_num);
// 		printf("序号从 %d 到 %d , 偏移值如下:\n",  begin_offset_[hz_pos][lay_num], begin_offset_[hz_pos][lay_num] + num_of_offset_[hz_pos][lay_num] - 1);
// 		for(size_t i = begin_offset_[hz_pos][lay_num]; i < begin_offset_[hz_pos][lay_num] + num_of_offset_[hz_pos][lay_num]; i++ ) {
// 			printf("%d  ", offset_array_[hz_pos][i]);
// 		}
//		printf("\n");
	} //for 1

	//生成整句先
	generate_full_sentence(search_hz_end - search_hz_begin + 1);

	//将非整句候选词传给引擎
	if( ((cur_sy_num_ - 1) /2 - fixed_hanzi_len_) < kMaxSysLmaSize ) {
		if( !get_lemma_id_array( search_hz_begin, lay_num_of_hz_[search_hz_begin], lma_id_set, set_num ) ) {
			printf("获取 Lemma id array 失败, 声母 = %d\n", sm_id);
			return false;
		}
	}

	return true;
}

bool LayerTrie::process_yunmu( ShyunIdType ym_id, LemmaIdSet* lma_id_set, size_t* set_num )
{
	if( ym_id < 0 || ym_id >= kAllYunmuNum ) {
		printf(" !!! 韵母 Id 越界, ym_id = %d !!!\n", ym_id);
		return false;
	}

	if(cur_sy_num_ < 0 || cur_sy_num_ >= kMaxSyIdNum) {
		printf(" !!! sy_num 已达到最大值, 不能再输入了 !!! \n");
		return false;
	}

	//先自增以下声韵Id数目
	cur_sy_num_++;
	sy_array_[cur_sy_num_] = ym_id;
	assert( cur_sy_num_ % 2 == 0 );
	hanzi_len_ = (cur_sy_num_ + 1) / 2;
	printf("汉字数量: %d \n", hanzi_len_);

//	size_t search_hz_begin = ( fixed_hanzi_len_ > ( ( cur_sy_num_ /2 ) - kMaxSysLmaSize ) ) ? fixed_hanzi_len_ : ( ( cur_sy_num_ /2 ) - kMaxSysLmaSize );
	size_t search_hz_begin;
	if( ((cur_sy_num_ - 1) /2 - fixed_hanzi_len_) < kMaxSysLmaSize - 1 ) { //系统词典中的词元长度最大为 4
		search_hz_begin = fixed_hanzi_len_;
	}
	else {
		search_hz_begin = ( ( cur_sy_num_ - 1) /2 ) - kMaxSysLmaSize + 1 ;
	}
	size_t search_hz_end = (cur_sy_num_ - 1) /2;

//	printf("汉字位置:  begin = %d,  end = %d \n", search_hz_begin, search_hz_end);

	for( size_t hz_pos = search_hz_begin; hz_pos <= search_hz_end; hz_pos++ ) {
		float psb_of_max = kMinLmaFreq;
		LemmaIdType lemma_id_of_max = 0; //初值为为 0 , 后面0可以作为是否有效的判断标志

		size_t lay_num = lay_num_of_hz_[hz_pos]; //先取得此汉字开始搜索的Trie偏移到了几层
		assert( lay_num >= 0 && lay_num <= 8 && lay_num % 2 == 1 );

		size_t first_off_pos = begin_offset_[hz_pos][lay_num];
		size_t last_off_pos = first_off_pos + num_of_offset_[hz_pos][lay_num];
		size_t first_off_pos_next = last_off_pos;
		size_t num_of_off = 0;

		bool is_half_spell = false;
		if(0 == ym_id ) {
			is_half_spell = true;
		}

		//因为各层的结点不一样, 所以不容易写出通用的代码, 先这样"丑陋"地实现一下吧
		//(其实就 1 行代码是与具体各层相关的)
//		printf("case %d\n", lay_num);
		switch(lay_num) {
		case 1:
			for(size_t off_pos = first_off_pos; off_pos < last_off_pos; off_pos++) {
				//遍历上一层的所有偏移, 挑出其中ym_id与输入的ym_id相同的偏移, 加入到下一层偏移该存放的地方
				uint16 offset = offset_array_[hz_pos][off_pos];
				if(ym_id == lma_node_lay2_[offset].ym_id || is_half_spell) {
//					printf("ym id = %d\n", ym_id);
					offset_array_[hz_pos][first_off_pos_next + num_of_off] = offset;
					num_of_off++;

					if(lma_node_lay2_[offset].num_of_lma == 0) {
						continue;
					}
					LemmaIdType lemma_id_this_offset = total_lemma_num_before_lay_[2] + lma_node_lay2_[offset].lma_off_1st + lma_node_lay2_[offset].num_of_lma - 1;
					if( uni_gram_.get_uni_psb(lemma_id_this_offset) > psb_of_max ) {
						lemma_id_of_max = lemma_id_this_offset;
						psb_of_max = uni_gram_.get_uni_psb(lemma_id_this_offset);
					}
				}
			}

			break;
		case 3:
			for(size_t off_pos = first_off_pos; off_pos < last_off_pos; off_pos++) {
				//遍历上一层的所有偏移, 挑出其中ym_id与输入的ym_id相同的偏移, 加入到下一层偏移该存放的地方
				uint16 offset = offset_array_[hz_pos][off_pos];
				if(ym_id == lma_node_lay4_[offset].ym_id || is_half_spell) {
//					printf("ym id = %d\n", ym_id);
					offset_array_[hz_pos][first_off_pos_next + num_of_off] = offset;
					num_of_off++;

					if(lma_node_lay4_[offset].num_of_lma == 0) {
						continue;
					}
					LemmaIdType lemma_id_this_offset = total_lemma_num_before_lay_[4] + lma_node_lay4_[offset].lma_off_1st + lma_node_lay4_[offset].num_of_lma - 1;
					if( uni_gram_.get_uni_psb(lemma_id_this_offset) > psb_of_max ) {
						lemma_id_of_max = lemma_id_this_offset;
						psb_of_max = uni_gram_.get_uni_psb(lemma_id_this_offset);
					}
				}
			}
			break;
		case 5:
			for(size_t off_pos = first_off_pos; off_pos < last_off_pos; off_pos++) {
				//遍历上一层的所有偏移, 挑出其中ym_id与输入的ym_id相同的偏移, 加入到下一层偏移该存放的地方
				uint16 offset = offset_array_[hz_pos][off_pos];
				if(ym_id == lma_node_lay6_[offset].ym_id || is_half_spell) {
//					printf("ym id = %d\n", ym_id);
					offset_array_[hz_pos][first_off_pos_next + num_of_off] = offset;
					num_of_off++;

					if(lma_node_lay6_[offset].num_of_lma == 0) {
						continue;
					}
					LemmaIdType lemma_id_this_offset = total_lemma_num_before_lay_[6] + lma_node_lay6_[offset].lma_off_1st + lma_node_lay6_[offset].num_of_lma - 1;
					if( uni_gram_.get_uni_psb(lemma_id_this_offset) > psb_of_max ) {
						lemma_id_of_max = lemma_id_this_offset;
						psb_of_max = uni_gram_.get_uni_psb(lemma_id_this_offset);
					}
				}
			}
			break;
		case 7:
			for(size_t off_pos = first_off_pos; off_pos < last_off_pos; off_pos++) {
				//遍历上一层的所有偏移, 挑出其中ym_id与输入的ym_id相同的偏移, 加入到下一层偏移该存放的地方
				uint16 offset = offset_array_[hz_pos][off_pos];
				if(ym_id == lma_node_lay8_[offset].ym_id || is_half_spell) {
//					printf("ym id = %d\n", ym_id);
					offset_array_[hz_pos][first_off_pos_next + num_of_off] = offset;
					num_of_off++;

					if(lma_node_lay8_[offset].num_of_lma == 0) {
						continue;
					}
					LemmaIdType lemma_id_this_offset = total_lemma_num_before_lay_[8] + lma_node_lay8_[offset].lma_off_1st + lma_node_lay8_[offset].num_of_lma - 1;
					if( uni_gram_.get_uni_psb(lemma_id_this_offset) > psb_of_max ) {
						lemma_id_of_max = lemma_id_this_offset;
						psb_of_max = uni_gram_.get_uni_psb(lemma_id_this_offset);
					}
				}
			}
			break;
		default:
			printf(" !!! 错误的 lay_num = %d\n", lay_num);
			reset_layer_tree();
			return false;
		}

		//输入的声韵序号在此汉字开始的解码中无子节点. 这种情况可以统一看待
// 		if( 0 == num_of_off ) {
// 			printf("这次输入的韵母Id = %d , 没有子节点 !! \n", ym_id);
// 		}

		//确定下一层偏移们在偏移数组中的起始位置和结束位置
		lay_num++;
		begin_offset_[hz_pos][lay_num] = first_off_pos_next;
		num_of_offset_[hz_pos][lay_num] = num_of_off;
		lay_num_of_hz_[hz_pos] = lay_num;

		if(lemma_id_of_max == 0) {
			lma_psb_item_[hz_pos - search_hz_begin].lemma_id = 0;
			lma_psb_item_[hz_pos - search_hz_begin].lemma_psb = -1000; //
		}
		else {
			lma_psb_item_[hz_pos - search_hz_begin].lemma_id = lemma_id_of_max;
			lma_psb_item_[hz_pos - search_hz_begin].lemma_psb = psb_of_max;
		}
//		printf("添加了 %d 个偏移, 在第 %d 个汉字 第 %d 层.  ", num_of_off, hz_pos, lay_num);
// 		printf("序号从 %d 到 %d , 偏移值如下:\n",  begin_offset_[hz_pos][lay_num], begin_offset_[hz_pos][lay_num] + num_of_offset_[hz_pos][lay_num] - 1);
// 		for(size_t i = begin_offset_[hz_pos][lay_num]; i < begin_offset_[hz_pos][lay_num] + num_of_offset_[hz_pos][lay_num]; i++ ) {
// 			printf("%d  ", offset_array_[hz_pos][i]);
// 		}
//		printf("\n");
	} // for 1

	//生成整句先
	generate_full_sentence(search_hz_end - search_hz_begin + 1);

	//将非整句候选词传给引擎
	if( ((cur_sy_num_ - 1) /2 - fixed_hanzi_len_) < kMaxSysLmaSize ) {
		if( !get_lemma_id_array( search_hz_begin, lay_num_of_hz_[search_hz_begin], lma_id_set, set_num ) ) {
			printf("获取 Lemma id array 失败, 韵母 = %d\n", ym_id);
			return false;
		}
	}

	return true;
}

bool LayerTrie::process_delete()
{
	return true;
}

bool LayerTrie::process_select( uint16 select_num )
{
	return true;
}

bool LayerTrie::get_lemma_id_array( size_t hz_pos, size_t lay_id, LemmaIdSet* lma_id_set, size_t* set_num )
{
	size_t first_off_pos = begin_offset_[hz_pos][lay_id];
	size_t final_off_pos = first_off_pos + num_of_offset_[hz_pos][lay_id];

	//每次都向lma_id_set的首位置装东西
	size_t new_set_num = 0;

	switch(lay_id) {
	case 1:
	case 2:
		for(size_t off_pos = first_off_pos; off_pos < final_off_pos; off_pos++) {
			uint16 off = offset_array_[hz_pos][off_pos];
			if( lma_node_lay2_[off].num_of_lma == 0 ) {
				continue;  // 表示此节点无Lemma, 如输出的话, 会显示 "对得住"
			}
			LemmaIdType first_lma_id = lma_node_lay2_[off].lma_off_1st + total_lemma_num_before_lay_[lay_id];
			LemmaIdType final_lma_id = first_lma_id + lma_node_lay2_[off].num_of_lma - 1;
			lma_id_set[new_set_num].cur_lemma_id = final_lma_id;
			lma_id_set[new_set_num].least_psb_id = first_lma_id;
			new_set_num++;
		} //for
		break;
	case 3:
	case 4:
		for(size_t off_pos = first_off_pos; off_pos < final_off_pos; off_pos++) {
			uint16 off = offset_array_[hz_pos][off_pos];
			if( lma_node_lay4_[off].num_of_lma == 0 ) {
				continue;  // 表示此节点无Lemma, 如输出的话, 会显示 "对得住"
			}
			LemmaIdType first_lma_id = lma_node_lay4_[off].lma_off_1st + total_lemma_num_before_lay_[lay_id];
			LemmaIdType final_lma_id = first_lma_id + lma_node_lay4_[off].num_of_lma - 1;
			lma_id_set[new_set_num].cur_lemma_id = final_lma_id;
			lma_id_set[new_set_num].least_psb_id = first_lma_id;
			new_set_num++;
		} //for
		break;
	case 5:
	case 6:
		for(size_t off_pos = first_off_pos; off_pos < final_off_pos; off_pos++) {
			uint16 off = offset_array_[hz_pos][off_pos];
			if( lma_node_lay6_[off].num_of_lma == 0 ) {
				continue;  // 表示此节点无Lemma, 如输出的话, 会显示 "对得住"
			}
			LemmaIdType first_lma_id = lma_node_lay6_[off].lma_off_1st + total_lemma_num_before_lay_[lay_id];
			LemmaIdType final_lma_id = first_lma_id + lma_node_lay6_[off].num_of_lma - 1;
			lma_id_set[new_set_num].cur_lemma_id = final_lma_id;
			lma_id_set[new_set_num].least_psb_id = first_lma_id;
			new_set_num++;
		} //for
		break;
	case 7:
	case 8:
		for(size_t off_pos = first_off_pos; off_pos < final_off_pos; off_pos++) {
			uint16 off = offset_array_[hz_pos][off_pos];
			if( lma_node_lay8_[off].num_of_lma == 0 ) {
				continue;  // 表示此节点无Lemma, 如输出的话, 会显示 "对得住"
			}
			LemmaIdType first_lma_id = lma_node_lay8_[off].lma_off_1st + total_lemma_num_before_lay_[lay_id];
			LemmaIdType final_lma_id = first_lma_id + lma_node_lay8_[off].num_of_lma - 1;
			lma_id_set[new_set_num].cur_lemma_id = final_lma_id;
			lma_id_set[new_set_num].least_psb_id = first_lma_id;
			new_set_num++;
		} //for
		break;
	default:
		return false;
	}

	if(new_set_num > kMaxLmaIdSetNum) {
		printf("\n !!! 缓存用光了 !!!\n");
		return false;
	}
	*set_num = new_set_num;

	return true;
}

char16* LayerTrie::generate_full_sentence( size_t travel_num )
{
	float max_sent_psb = kMinLmaFreq;
	size_t pos_of_max = travel_num;
	for(size_t i = 0; i < travel_num; i++) {
//		printf("频率最大的是: %d, psb = %f\n", lma_psb_item_[i].lemma_id, lma_psb_item_[i].lemma_psb);
		if( lma_psb_item_[i].lemma_psb + sentence_state_[hanzi_len_ - travel_num + i].psb_sum > max_sent_psb ) {
			max_sent_psb = lma_psb_item_[i].lemma_psb + sentence_state_[hanzi_len_ - travel_num + i].psb_sum;
			pos_of_max = i;
		}
	}
	assert(pos_of_max < travel_num);

	sentence_state_[hanzi_len_].from = hanzi_len_ - travel_num + pos_of_max;
	sentence_state_[hanzi_len_].psb_sum = max_sent_psb;
	sentence_state_[hanzi_len_].lemma_id = lma_psb_item_[pos_of_max].lemma_id;

	char16 lma_buf[kMaxLemmaSize + 1];
	setlocale(LC_ALL, "chs");

	size_t cur_state = hanzi_len_;
	while(cur_state != 0) {
		wprintf( L"%ls ", lemma_fetch_.get_lemma_by_id( sentence_state_[cur_state].lemma_id, lma_buf ) );
		cur_state = sentence_state_[cur_state].from;
	}
	printf("\n");

//	printf("max_sent_psb : %f, pos = %d\n", max_sent_psb, pos_of_max);
	for(size_t i = 0; i <= hanzi_len_; i++) {
//		printf("From  : %d, Psb sum = %f\n", sentence_state_[i].from, sentence_state_[i].psb_sum);
	}
		return NULL;
}



