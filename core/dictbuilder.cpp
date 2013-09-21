// Copyright 2011, ZhongYi Inc
// Author: Windriver < http://windriver.github.io >

#include "dictbuilder.h"

DictBuilder::DictBuilder()
{
	root_temp_ = new TempNode();
	root_temp_->lemma_num = 0;
	root_temp_->right = NULL;
	root_temp_->down = NULL;
	for(size_t i = 0; i < 20; i++){
		lemma_num_lay_[i] = 0;
		total_lemma_num_before_lay_[i] = 0;
	}

	for(size_t i = 0; i < 20; i++){
		cur_node_num_lay_[i] = 0;
		cur_son_num_lay_[i] = 0;
	}

	freq_[0] = 0.0;
	total_freq_sum_ = 0.0;

	for(size_t i = 0; i <= kMaxLemmaSize; i++) {
		lemma_num_word_[i] = 0;
	}
}

DictBuilder::~DictBuilder()
{

}

bool DictBuilder::build_dict()
{
	if( ! alloc_resource() ) {
		printf("\n !!! 分配资源失败 !!! \n");
		return false;
	}
	lemma_num_ = read_raw_dict( "rawdict_utf16_65105_freq.txt" );
	if( lemma_num_ <= 0 ) {
		printf("\n !!! 读取词典初步处理失败 !!! \n");
		return false;
	}
	printf("\nThe temp tree 已经创建, 现在开始统计... \n");
	stat_temp_tree();

	printf("\n开始层序遍历, 并生成多层Trie树... \n");
	if( !build_layer_trie() ) {
		printf("\n !!! 创建多层trie树失败 !!! \n");
		return false;
	}
	printf("\n多层数组Trie树已经创建, 下面开始统计 ...\n");
	stat_layer_trie();

	printf("\n二进制词典创建完毕, 开始保存 ...\n");
	if( !save_dict() ){
		printf("Error !!! 保存词典失败 !!!\n");
		return false;
	}
	printf("\n**************词典创建并保存成功**************\n");
	printf("\n********下面开始保存unigram信息********\n");
	if( !save_uni_gram() ) {
		printf("Error !!! unigram信息保存失败 !!!\n");
		return false;
	}
	printf("\nunigram信息保存成功, 下面开始统计...\n");
	stat_uni_gram();

	printf("\n下面开始统计Lemma本体信息...\n");
	stat_lemma_str();

	printf("\n下面开始存储Lemma本体..\n");
	if( !save_lemma_str() ) {
		printf("\n !!! Error !!! Lemma本体保存失败 !!!\n");
		return false;
	}
	printf("\n**************Lemma本体保存成功**************\n");
	return true;

}

bool DictBuilder::save_dict()
{
	//暂时这么处理
	FILE* fp = fopen("dict_shyun.dat", "wb");
	if( NULL == fp ) {
		return false;
	}

	for(size_t i = 0; i < kMaxShyunLayer; i++) {
		if( fwrite( &cur_node_num_lay_[i], sizeof(size_t), 1, fp ) != 1 ){
			return false;
		}
	}

	for(size_t i = 0; i < kMaxShyunLayer; i++) {
		if( fwrite( &total_lemma_num_before_lay_[i], sizeof(size_t), 1, fp ) != 1 ){
			return false;
		}
	}

	if( fwrite( lma_node_lay0_, sizeof(LmaNodeLayer00), cur_node_num_lay_[0], fp ) != cur_node_num_lay_[0] ){
		return false;
	}
 	if( fwrite( lma_node_lay1_, sizeof(LmaNodeLayer01), cur_node_num_lay_[1], fp ) != cur_node_num_lay_[1] ){
 		return false;
 	}
 	if( fwrite( lma_node_lay2_, sizeof(LmaNodeLayer02), cur_node_num_lay_[2], fp ) != cur_node_num_lay_[2] ){
 		return false;
 	}
	if( fwrite( lma_node_lay3_, sizeof(LmaNodeLayerSm), cur_node_num_lay_[3], fp ) != cur_node_num_lay_[3] ){
		return false;
	}
	if( fwrite( lma_node_lay4_, sizeof(LmaNodeLayerYm), cur_node_num_lay_[4], fp ) != cur_node_num_lay_[4] ){
		return false;
	}
	if( fwrite( lma_node_lay5_, sizeof(LmaNodeLayerSm), cur_node_num_lay_[5], fp ) != cur_node_num_lay_[5] ){
		return false;
	}
	if( fwrite( lma_node_lay6_, sizeof(LmaNodeLayerYm), cur_node_num_lay_[6], fp ) != cur_node_num_lay_[6] ){
		return false;
	}
	if( fwrite( lma_node_lay7_, sizeof(LmaNodeLayerSm), cur_node_num_lay_[7], fp ) != cur_node_num_lay_[7] ){
		return false;
	}
	if( fwrite( lma_node_lay8_, sizeof(LmaNodeLayerYm), cur_node_num_lay_[8], fp ) != cur_node_num_lay_[8] ){
		return false;
	}

	fclose(fp);
	return true;
}

bool DictBuilder::alloc_resource()
{
	single_word_ = new char16[17028 + 1];
	double_word_ = new char16[51098 * 2 + 1];
	three_word_ = new char16[7372 * 3 + 1];
	four_word_ = new char16[6626 * 4 + 1];

	if( NULL == single_word_ || NULL == double_word_ ||
						NULL == three_word_ || NULL == four_word_ ) {
			delete []single_word_;
			delete []double_word_;
			delete []three_word_;
			delete []four_word_;
			single_word_ = NULL;
			double_word_ = NULL;
			three_word_ = NULL;
			four_word_ = NULL;

			return false;
	}

	//为分层分配存储空间先
	lma_node_lay0_ = new LmaNodeLayer00[ kNodeNumLayer00 ];
	lma_node_lay1_ = new LmaNodeLayer01[ kNodeNumLayer01 ];
	lma_node_lay2_ = new LmaNodeLayer02[ kNodeNumLayer02 ];
	lma_node_lay3_ = new LmaNodeLayerSm[ kNodeNumLayer03 ];
	lma_node_lay4_ = new LmaNodeLayerYm[ kNodeNumLayer04 ];
	lma_node_lay5_ = new LmaNodeLayerSm[ kNodeNumLayer05 ];
	lma_node_lay6_ = new LmaNodeLayerYm[ kNodeNumLayer06 ];
	lma_node_lay7_ = new LmaNodeLayerSm[ kNodeNumLayer07 ];
	lma_node_lay8_ = new LmaNodeLayerYm[ kNodeNumLayer08 ];

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

	return true;
}

size_t DictBuilder::read_raw_dict( const char* filename )
{
	if( NULL == filename ) {
		printf( "!!! 读取词典失败, 文件名为空 !!! ");
	}
	const size_t kReadBufSize = 512;
	Utf16Reader utf16_reader;
	if( !utf16_reader.open( filename,  kReadBufSize * 10 ) ) {
		printf( "utf-open error\n" );
		return 0;
	}

	printf( "现在开始逐行读取词典, 并创建临时树 ...\n");
	char16 read_buf[kReadBufSize];
	size_t lemma_num;

	for(size_t i = 0; i < 360000; i++) {
		if( !utf16_reader.readline( read_buf, kReadBufSize ) ) {
			lemma_num = i;
			break;
		}

		size_t token_size;
		char16 *token;
		char16 *to_tokenize = read_buf;

		char16 lemma_buf[kMaxLemmaSize + 1];
		size_t lemma_len;
		LemmaIdType lemma_id = i+1;

		// 获得Lemma本体
		token = utf16_strtok(to_tokenize, &token_size, &to_tokenize);
		if(NULL == token){
			utf16_reader.close();
			printf("Lemma[ %d ] read error!", i);
			return 0;
		}

		//将Lemma本体拷贝到buffer中
		if ( utf16_strcpy(lemma_buf, token) == NULL ) {
			printf("\n!!! 拷贝Lemma时出错 !!!\n");
			return 0;
		}

//  		setlocale(LC_ALL, "chs");
// 		wprintf(L"%ls\n", token);

		lemma_len = token_size;

		if (lemma_len > kMaxLemmaSize) {
			i--;
			continue;
		}

		if (lemma_len > 4) {
			i--;
			continue;
		}

		//读取每个Lemma的频率信息
		token = utf16_strtok(to_tokenize, &token_size, &to_tokenize);
		if(NULL == token){
			utf16_reader.close();
			printf("Freq[ %d ] read error!", i);
			return 0;
		}
		freq_[lemma_id] = utf16_atof(token);
		total_freq_sum_ += freq_[lemma_id];

		if( i < 100 || i > 65000 ){
//			printf("Freq[%d] = %f\n", lemma_id, freq_[lemma_id]);
		}

		//读取每个单字Lemma是否为GBK, 0或者1
		token = utf16_strtok(to_tokenize, &token_size, &to_tokenize);

		//下面开始读Lemma的拼音了
		char py_buf[10];
		uint16 sy_id_arr[20];
		for(size_t hz_pos = 0; hz_pos < lemma_len; hz_pos++){
			token = utf16_strtok(to_tokenize, &token_size, &to_tokenize);
			if(NULL == token){
				printf("Pinyin[%d] of Lemma[%d] error!", hz_pos +1, i + 1);
				return 0;
			}
			utf16_strcpy_tochar(py_buf, token);
			if( false == pinyin_map_.pinyin_to_sy( py_buf, &sy_id_arr[hz_pos * 2], &sy_id_arr[hz_pos * 2 + 1] ) ){
				lemma_num = i;
				goto end;
			}
		} //for (hz_pos)

//		printf("LemmaId 为 %d\n", lemma_id);
		//尝试将此Lemma插入到临时树上
		if( false == insert_to_temp_tree(sy_id_arr, lemma_len * 2, lemma_id ) ) {
			printf("Insert to temp tree error !\n");
			return 0;
		}
		if( i % 10000 == 0 ) {
			printf("已读到 : Lemma %d\n", i);
		}

		//到了这一步, 说明此Lemma合法, 可以拷进lemma数组中了
		switch(lemma_len) {
		case 1:
			single_word_[ lemma_num_word_[1] ] = lemma_buf[0];
			lemma_num_word_[1]++;
			break;
		case 2:
			double_word_[ lemma_num_word_[2] * 2 ] = lemma_buf[0];
			double_word_[ lemma_num_word_[2] * 2 + 1 ] = lemma_buf[1];
			lemma_num_word_[2]++;
			break;
		case 3:
			three_word_[ lemma_num_word_[3] * 3 ] = lemma_buf[0];
			three_word_[ lemma_num_word_[3] * 3 + 1 ] = lemma_buf[1];
			three_word_[ lemma_num_word_[3] * 3 + 2 ] = lemma_buf[2];
			lemma_num_word_[3]++;
			break;
		case 4:
			four_word_[ lemma_num_word_[4] * 4 ] = lemma_buf[0];
			four_word_[ lemma_num_word_[4] * 4 + 1 ] = lemma_buf[1];
			four_word_[ lemma_num_word_[4] * 4 + 2 ] = lemma_buf[2];
			four_word_[ lemma_num_word_[4] * 4 + 3] = lemma_buf[3];
			lemma_num_word_[4]++;
			break;
			default:
				printf("\n !!! 错误的Lemma长度 in switch(lemma_len) !!! \n");
				return 0;
		} //switch
	} //for

	//用了goto, 当map不到时走这里, 当插入结点错误也走这里
	end:
	printf( "Lemma number is: %d\n", lemma_num );
	return lemma_num;
}

bool DictBuilder::insert_to_temp_tree( uint16* sy_id_arr, size_t sy_num,LemmaIdType lemma_id )
{
	TempNode* temp_node = root_temp_;
	for(size_t lay_id = 0; lay_id < sy_num; lay_id++) {
		uint16 sy_id_lay = sy_id_arr[lay_id]; //取得此层的声韵id

		if( NULL == temp_node->down ){
	//		printf("我是第一! . ");
			//表示此上一个syid对应的结点还未有子节点
			TempNode* new_node = new TempNode;
			new_node->node_type = lay_id + 1;
			new_node->sy_id = sy_id_arr[lay_id];
			if( lay_id == sy_num - 1 ){
				new_node->lemma_id_1st = lemma_id;
			}
			new_node->lemma_num = 0;
			new_node->down = NULL;
			new_node->right = NULL;
			temp_node->down = new_node;
			//此时temp_node下降
			temp_node = temp_node->down;
		}
		else if( temp_node->down->sy_id > sy_id_lay ) {
	//		printf("插到最前面 . ");
			//此层的首结点的syid较大, 故应该插入到首结点之前
			TempNode* new_node = new TempNode;
			new_node->node_type = lay_id + 1;
			new_node->sy_id = sy_id_arr[lay_id];
			if( lay_id == sy_num - 1 ){
				new_node->lemma_id_1st = lemma_id;
			}
			new_node->lemma_num = 0;
			new_node->down = NULL;
			new_node->right = NULL;
			TempNode* first_node = temp_node->down;
			temp_node->down = new_node;
			new_node->right =first_node;
			temp_node = new_node;
		}
		else if( temp_node->down->sy_id == sy_id_lay ){
	//		printf("首结点Lemma++ . ");
			temp_node = temp_node->down;
			assert( temp_node->node_type == lay_id + 1 );

		}
		else{
			temp_node = temp_node->down;
			while( temp_node->right != NULL && temp_node->right->sy_id < sy_id_lay ){
				temp_node = temp_node->right;
			}
			if( temp_node->right == NULL || temp_node->right->sy_id > sy_id_lay ){
	//			printf("在之前插入 . ");
				TempNode* new_node = new TempNode;
				new_node->node_type = lay_id + 1;
				new_node->sy_id = sy_id_arr[lay_id];
				if( lay_id == sy_num - 1 ){
					new_node->lemma_id_1st = lemma_id;
				}
				new_node->lemma_num = 0;
				new_node->down = NULL;
				new_node->right = temp_node->right;
				temp_node->right = new_node;
				temp_node = new_node;
			}
			else {
	//			printf("该结点自增 . ");
				//找到了已经存在的结点
				temp_node = temp_node->right;
				assert( temp_node->node_type == lay_id + 1 );

				}
			}
		//当不是途经结点时, lemma_num自增 (这两个判定条件是对的 !)
		if( lay_id ==sy_num - 1 || lay_id == sy_num - 2 ) {
			temp_node->lemma_num++;
			lemma_num_lay_[lay_id + 1]++;
		}

	}  //for
//	printf("\n");
	return true;
}

void DictBuilder::stat_temp_tree()
{
	printf("********各层Lemma数的统计********\n");
	size_t total_lemma_num = 0;
	for(size_t i = 1; i < 10; i++){
		printf("第 %d 层 :  %d\n", i, lemma_num_lay_[i]);
		total_lemma_num_before_lay_[i] += total_lemma_num_before_lay_[i-1] + ( ( i % 2 == 0 ) ? 0 : lemma_num_lay_[i - 1] );
		if( i % 2 == 0 ) {
			printf("第 %d 层之前共有 %d 个结点 \n", i, total_lemma_num_before_lay_[i]);
		}
		total_lemma_num += lemma_num_lay_[i];
	}
	printf("Lemma结点总数: %d\n", total_lemma_num / 2);
}

bool DictBuilder::build_layer_trie()
{
	std::queue<TempNode*> que;
	que.push(root_temp_);

	size_t total_lemma_num = 0;
	size_t lay_id = 0;

	while(true) {
		//先获取当前队列中的元素数, 也就是Trie树第lay_id层的结点总数
		size_t que_size = que.size();
		if( 0 == que_size ){
			break;
		}

		printf("第 %d 层有 %d 个结点 . \n", lay_id, que_size);
		size_t max_lemma_num_this_layer = 0;
		size_t max_son_num_this_layer = 0;

		//对这que_size个队列元素进行处理, 以达到层序遍历的目的
		for(size_t i = 0; i < que_size; i++) {
			//先访问front
			TempNode* temp_node = que.front();
			if( !insert_to_layer_trie(temp_node, lay_id, &max_lemma_num_this_layer, &max_son_num_this_layer)) {
				printf("插入多层Trie数是发生错误 !\n");
				return false;
			}
			total_lemma_num += temp_node->lemma_num;

			//front元素出队列
			que.pop();

			//出队列后, 将它down以后的子链的结点全部加入队列中
			TempNode* down_node = temp_node->down;
			while( NULL != down_node ){
				que.push(down_node);
				down_node = down_node->right;
			} //while (忘记加大括号了, 导致一直push, 系统都跑死了
		} //for

//  		printf("\n本层结点中, 最多词元数量为: %d , 最多子节点数量为 : %d \n", max_lemma_num_this_layer, max_son_num_this_layer);
//  		printf( "\n" );

		lay_id++; //层数自增
	} //while(true)

	printf("Total lemma number = %d\n", total_lemma_num / 2);
	return true;
}

bool DictBuilder::insert_to_layer_trie( TempNode* temp_node, uint16 lay_id,
																	size_t* max_lemma_num_this_layer, size_t* max_son_num_this_layer )
{
	assert( lay_id == temp_node->node_type ) ;
	//先统计该结点子链长度
	size_t son_num = 0;
	size_t lemma_num = temp_node->lemma_num;
	uint16 first_lemma_off = temp_node->lemma_id_1st - total_lemma_num_before_lay_[lay_id];
	ShyunIdType sy_id = temp_node->sy_id;

	temp_node = temp_node->down;
	while( NULL != temp_node ) {
		son_num++;
		temp_node = temp_node->right;
	} //while

	switch( lay_id ) {
	case 0:
		lma_node_lay0_[ cur_node_num_lay_[lay_id] ].num_of_son = son_num;
		lma_node_lay0_[ cur_node_num_lay_[lay_id] ].num_of_lma = lemma_num;
		lma_node_lay0_[ cur_node_num_lay_[lay_id] ].son_off_1st = cur_son_num_lay_[lay_id];
		break;
	case 1:
		lma_node_lay1_[ cur_node_num_lay_[lay_id] ].num_of_son = son_num;
		lma_node_lay1_[ cur_node_num_lay_[lay_id] ].num_of_lma = lemma_num;
		lma_node_lay1_[ cur_node_num_lay_[lay_id] ].son_off_1st = cur_son_num_lay_[lay_id];
		lma_node_lay1_[ cur_node_num_lay_[lay_id] ].sm_id = sy_id;
		break;
	case 2:
		lma_node_lay2_[ cur_node_num_lay_[lay_id] ].num_of_son = son_num;
		lma_node_lay2_[ cur_node_num_lay_[lay_id] ].num_of_lma = lemma_num;
		lma_node_lay2_[ cur_node_num_lay_[lay_id] ].son_off_1st = cur_son_num_lay_[lay_id];
		lma_node_lay2_[ cur_node_num_lay_[lay_id] ].lma_off_1st = first_lemma_off;
		lma_node_lay2_[ cur_node_num_lay_[lay_id] ].ym_id = sy_id;
		break;
	case 3:
		lma_node_lay3_[ cur_node_num_lay_[lay_id] ].num_of_son = son_num;
		lma_node_lay3_[ cur_node_num_lay_[lay_id] ].num_of_lma = lemma_num;
		lma_node_lay3_[ cur_node_num_lay_[lay_id] ].son_off_1st = cur_son_num_lay_[lay_id];
		lma_node_lay3_[ cur_node_num_lay_[lay_id] ].sm_id = sy_id;
		break;
	case 4:
		lma_node_lay4_[ cur_node_num_lay_[lay_id] ].num_of_son = son_num;
		lma_node_lay4_[ cur_node_num_lay_[lay_id] ].num_of_lma = lemma_num;
		lma_node_lay4_[ cur_node_num_lay_[lay_id] ].son_off_1st = cur_son_num_lay_[lay_id];
		lma_node_lay4_[ cur_node_num_lay_[lay_id] ].lma_off_1st = first_lemma_off;
		lma_node_lay4_[ cur_node_num_lay_[lay_id] ].ym_id = sy_id;
		break;
	case 5:
		lma_node_lay5_[ cur_node_num_lay_[lay_id] ].num_of_son = son_num;
		lma_node_lay5_[ cur_node_num_lay_[lay_id] ].num_of_lma = lemma_num;
		lma_node_lay5_[ cur_node_num_lay_[lay_id] ].son_off_1st = cur_son_num_lay_[lay_id];
		lma_node_lay5_[ cur_node_num_lay_[lay_id] ].sm_id = sy_id;
		break;
	case 6:
		lma_node_lay6_[ cur_node_num_lay_[lay_id] ].num_of_son = son_num;
		lma_node_lay6_[ cur_node_num_lay_[lay_id] ].num_of_lma = lemma_num;
		lma_node_lay6_[ cur_node_num_lay_[lay_id] ].son_off_1st = cur_son_num_lay_[lay_id];
		lma_node_lay6_[ cur_node_num_lay_[lay_id] ].lma_off_1st = first_lemma_off;
		lma_node_lay6_[ cur_node_num_lay_[lay_id] ].ym_id = sy_id;
		break;
	case 7:
		lma_node_lay7_[ cur_node_num_lay_[lay_id] ].num_of_son = son_num;
		lma_node_lay7_[ cur_node_num_lay_[lay_id] ].num_of_lma = lemma_num;
		lma_node_lay7_[ cur_node_num_lay_[lay_id] ].son_off_1st = cur_son_num_lay_[lay_id];
		lma_node_lay7_[ cur_node_num_lay_[lay_id] ].sm_id = sy_id;
		break;
	case 8:
		lma_node_lay8_[ cur_node_num_lay_[lay_id] ].num_of_son = son_num;
		lma_node_lay8_[ cur_node_num_lay_[lay_id] ].num_of_lma = lemma_num;
		lma_node_lay8_[ cur_node_num_lay_[lay_id] ].son_off_1st = cur_son_num_lay_[lay_id];
		lma_node_lay8_[ cur_node_num_lay_[lay_id] ].lma_off_1st = first_lemma_off;
		lma_node_lay8_[ cur_node_num_lay_[lay_id] ].ym_id = sy_id;
		break;
	default:
		printf("lay_id 不在 0~8 之间 , lay_id = %d \n", lay_id);
		return false;
	}
	cur_node_num_lay_[lay_id]++; //每加入一个结点还要让相应层的总个数, 自增一下子
	cur_son_num_lay_[lay_id] += son_num; //还要计算以下累积偏移

//	printf( "%d-%d  ", lemma_num, son_num );
	//每层的结点最多有几个词元, 最多有几个子结点
	if( lemma_num > *max_lemma_num_this_layer ) {
		*max_lemma_num_this_layer = lemma_num;
	}
	if( son_num > *max_son_num_this_layer ) {
		*max_son_num_this_layer = son_num;
	}
	return true;
}

void DictBuilder::stat_layer_trie()
{
	printf("*****多层数组Trie树的各层结点数统计*****\n");
	size_t total_node_num = 0;
	for(size_t i = 0; i < 10; i++) {
		printf("第 %d 层有 %d 个结点 . \n", i, cur_node_num_lay_[i]);
		total_node_num += cur_node_num_lay_[i];
	}
	printf("结点总数为: %d . \n", total_node_num);

	printf("*****各层各结点的子链累积长度统计*****\n");
	size_t total_son_off = 0;
	for(size_t i = 0; i < 10; i++) {
		printf("第 %d 层总偏移为: %d  . \n", i, cur_son_num_lay_[i]);
		total_son_off += cur_son_num_lay_[i];
	}
	printf("总偏移数为: %d . \n", total_son_off);
	printf("每个结点占用空间为: %d, 估计共占用空间 %d . \n", sizeof(LmaNodeLayerSm), total_node_num * sizeof(LmaNodeLayerYm));
	printf("\n");
	for(size_t i = cur_node_num_lay_[8] - 1; i > cur_node_num_lay_[8] - 10; i--) {
		printf("第八层第 %d 个结点的首Lemma Id为 %d \n", i, lma_node_lay8_[i].lma_off_1st + total_lemma_num_before_lay_[8] );
	}
}

void DictBuilder::stat_uni_gram()
{
	printf("词典中共有 %d 个词元(lemma) \n", lemma_num_);
	printf("词元频度总和为: %lf \n", log( total_freq_sum_ ) );
	for(size_t i = 0; i <= lemma_num_ - 65000; i++) {
				printf("Freq[ %d ] = %f \n", i, freq_[i]);
	}
}

bool DictBuilder::save_uni_gram()
{
	FILE* fp = fopen("unigram.dat", "wb");
	if( NULL ==fp ) {
		printf("打开 unigram.dat 失败 \n");
		return false;
	}

	//用频度除以所有词元的频度总和, 得到每个词元的频率
	for( size_t lemma_id = 1; lemma_id <= lemma_num_; lemma_id++ ) {
		freq_[lemma_id] = log( freq_[lemma_id] ) - log( total_freq_sum_ );
	}

	if( fwrite( &lemma_num_, sizeof(size_t), 1, fp ) != 1 ) {
		printf("写入词元数目失败 !!! \n");
		return false;
	}

	if( fwrite( freq_ + 1, sizeof(float), lemma_num_, fp ) != lemma_num_ ) {
		printf("写入unigram信息失败!\n");
		return false;
	}

	fclose(fp);
	return true;
}

void DictBuilder::stat_lemma_str()
{
	for(size_t i = 0; i <= kMaxLemmaSize; i++) {
		printf("%d-字词共有 %d 个\n", i, lemma_num_word_[i]);
	}
}


bool DictBuilder::save_lemma_str()
{
	FILE* fp = fopen("lemma_str.dat", "wb");
	if(NULL == fp) {
		printf("\n!!! 打开lemma_str.dat文件错误 !!!\n");
		return false;
	}

	//只存那4个数组
	if( fwrite( &lemma_num_word_[1], sizeof(size_t), 4, fp ) != 4 ) {
		printf("\n!!! 存储各类Lemma的数量时出错 !!!\n");
		return false;
	}

	if( fwrite( single_word_, sizeof(char16), lemma_num_word_[1], fp ) != lemma_num_word_[1] ) {
		printf("\n!!! 存储single word时出错 !!!\n");
		return false;
	}
	if( fwrite( double_word_, sizeof(char16), lemma_num_word_[2] * 2, fp ) != lemma_num_word_[2] * 2 ) {
		printf("\n!!! 存储double word时出错 !!!\n");
		return false;
	}
	if( fwrite( three_word_, sizeof(char16), lemma_num_word_[3] * 3, fp ) != lemma_num_word_[3] * 3 ) {
		printf("\n!!! 存储three word时出错 !!!\n");
		return false;
	}
	if( fwrite( four_word_, sizeof(char16), lemma_num_word_[4] * 4, fp ) != lemma_num_word_[4] * 4 ) {
		printf("\n!!! 存储four word时出错 !!!\n");
		return false;
	}

	fclose(fp);

	return true;
}



