// Copyright 2011, ZhongYi  Inc
// Author: Windriver < http://windriver.github.io >

#include "lemmafetch.h"

LemmaFetch::LemmaFetch()
{
	single_word_ = NULL;
	double_word_ = NULL;
	three_word_ = NULL;
	double_word_ = NULL;

	for(size_t i = 0; i <= kMaxLemmaSize; i++) {
		first_id_[i] = 0;
	}

}

LemmaFetch::~LemmaFetch()
{

}

bool LemmaFetch::load_lemma_str()
{
	FILE* fp = fopen("lemma_str.dat", "rb");
	if(NULL == fp) {
		printf("\n!!! 打开lemma_str.dat文件错误 !!!\n");
		return false;
	}

	size_t lemma_num_word[kMaxLemmaSize + 1];
	for(size_t i = 0; i <= kMaxLemmaSize; i++) {
		lemma_num_word[i] = 0;
	}

	//读取那四个数组的长度及其元素
	if( fread( &lemma_num_word[1], sizeof(size_t), 4, fp ) != 4 ) {
		printf("\n!!! 存储各类Lemma的数量时出错 !!!\n");
		return false;
	}

	single_word_ = new char16[ lemma_num_word[1] + 1 ];
	double_word_ = new char16[ lemma_num_word[2] * 2 + 1 ];
	three_word_ = new char16[ lemma_num_word[3] * 3 + 1 ];
	four_word_ = new char16[ lemma_num_word[4] * 4 + 1 ];

	if( NULL == single_word_ || NULL == double_word_ ||
			NULL == three_word_ || NULL == four_word_ ) {
				printf("\n !!! 分配数据空间失败 !!!\n");
				delete []single_word_; delete []double_word_;
				delete []three_word_; delete []four_word_;
				single_word_ =NULL; double_word_ =NULL;
				three_word_ =NULL; four_word_ =NULL;

				return false;
	}

	if( fread( single_word_, sizeof(char16), lemma_num_word[1], fp ) != lemma_num_word[1] ) {
		printf("\n!!! 读取single word时出错 !!!\n");
		return false;
	}
	if( fread( double_word_, sizeof(char16), lemma_num_word[2] * 2, fp ) != lemma_num_word[2] * 2 ) {
		printf("\n!!! 读取double word时出错 !!!\n");
		return false;
	}
	if( fread( three_word_, sizeof(char16), lemma_num_word[3] * 3, fp ) != lemma_num_word[3] * 3 ) {
		printf("\n!!! 读取three word时出错 !!!\n");
		return false;
	}
	if( fread( four_word_, sizeof(char16), lemma_num_word[4] * 4, fp ) != lemma_num_word[4] * 4 ) {
		printf("\n!!! 读取four word时出错 !!!\n");
		return false;
	}

	fclose(fp);

	//Lemma Id 从 1 开始的, 0号Lemma用作它用
	first_id_[1] = 1;
	first_id_[2] = first_id_[1] + lemma_num_word[1];
	first_id_[3] = first_id_[2] + lemma_num_word[2];
	first_id_[4] = first_id_[3] + lemma_num_word[3];
	first_id_[5] = first_id_[4] + lemma_num_word[4];

	return true;
}

char16* LemmaFetch::get_lemma_by_id( LemmaIdType lemma_id, char16* lemma_buf )
{
	if( lemma_id <= 0 || lemma_id >= first_id_[5] ) {
		printf("\n !!! Lemma Id 超出范围 !!!\n");
		return NULL;
	}

	size_t lemma_len = 0;
	for(size_t i = 0; i < kMaxLemmaSize; i++) {
		if( lemma_id >= first_id_[i] && lemma_id < first_id_[i+1] ) {
			lemma_len = i;
			break;
		}
	} //for

	if( 0 == lemma_len ) {
		printf("\n !!! 计算Lemma长度出错 !!!\n");
		return NULL;
	}

	size_t lemma_id_off = lemma_id - first_id_[lemma_len];

	switch(lemma_len) {
	case 1:
		lemma_buf[0] = single_word_[lemma_id_off];
		lemma_buf[1] = (char16)'\0';
		break;
	case 2:
		lemma_buf[0] = double_word_[lemma_id_off * 2];
		lemma_buf[1] = double_word_[lemma_id_off * 2 + 1];
		lemma_buf[2] = (char16)'\0';
		break;
	case 3:
		lemma_buf[0] = three_word_[lemma_id_off * 3];
		lemma_buf[1] = three_word_[lemma_id_off * 3 + 1];
		lemma_buf[2] = three_word_[lemma_id_off * 3 + 2];
		lemma_buf[3] = (char16)'\0';
		break;
	case 4:
		lemma_buf[0] = four_word_[lemma_id_off * 4];
		lemma_buf[1] = four_word_[lemma_id_off * 4 + 1];
		lemma_buf[2] = four_word_[lemma_id_off * 4 + 2];
		lemma_buf[3] = four_word_[lemma_id_off * 4 + 3];
		lemma_buf[4] = (char16)'\0';
		break;
	default:
		printf("\n!!! switch 错误. lemma 长度为: %d !!!\n", lemma_len);
		return NULL;
	}

	return lemma_buf;
}

void LemmaFetch::stat_lemma_str()
{
	for(size_t i = 0; i <= kMaxLemmaSize; i++) {
		printf("%d-字词的首Lemma Id = %d 个\n", i, first_id_[i]);
	}

	char16 lemma_buf[kMaxLemmaSize + 1];
	size_t begin = 65090;  //从34063开始出问题, 发现34063正好近似于17029, 发现蹊跷
											//检查一下, 果然是因为fwrite和fread时忘记乘以lemma长度导致的
	for(LemmaIdType lemma_id = begin; lemma_id < begin + 20; lemma_id++) {
		if( get_lemma_by_id(lemma_id, lemma_buf) != NULL ) {
			setlocale(LC_ALL, "chs");
			wprintf(L"Lemma %d = %ls\n", lemma_id, lemma_buf);
		}
	}
}

