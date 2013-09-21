// Copyright 2011, ZhongYi  Inc
// Author: Windriver < http://windriver.github.io >

#include <iostream>
#include <string>
using namespace std;

#include <stdio.h>

#include "dictdef.h"
#include "dictbuilder.h"
#include "layertrie.h"
#include "pinyinmap.h"
#include "unigram.h"
#include "lemmafetch.h"
#include "imengine.h"

bool test_builder();

bool test_trie_search();

bool test_uni_gram();

bool test_lemma_str();

bool test_im_engine();

int main()
{
	//   词典建立的调试
// test_builder();


	//   多层数组Trie树的调试
//	test_trie_search();

	//   unigram的调试
//	test_uni_gram();

	//LemmaFetch的调试
//	test_lemma_str();

	//引擎的调试
	test_im_engine();

	system("pause");
	return 0 ;
}

bool test_builder()
{
	DictBuilder dict_builder;
		if( !dict_builder.build_dict() ){
 				printf( "Build dict error !\n");
				return false;
		}
		return true;
}

bool test_trie_search()
{
		LayerTrie lay_trie;
		if( !lay_trie.load_layer_trie() ) {
		 	printf( "!!! 加载词典失败 in main function !!!" );
		 	return false;
		}
			lay_trie.stat_layer_tree();

	printf("\n******开始测试搜索******\n");

	PinyinMap pinyin_map;

	bool flag = false;
	char sy_str[10];
	ShyunIdType sy_id;
	size_t sy_num = 0;

	while(true) {
		scanf("%s", sy_str);

		if(sy_num % 2 == 0) {
			if( !pinyin_map.sm_to_id(sy_str, &sy_id) ) {
				printf("\n!!! Map 错误 !!!\n");
				lay_trie.reset_layer_tree();
				sy_num = 0;
				continue;
			}
			if( !lay_trie.process_shmu(sy_id, NULL, 0) ) {
				printf("\n !!! 声母处理错误 !!!\n");
				lay_trie.reset_layer_tree();
				sy_num = 0;
				continue;
			}

			sy_num++;
		}
		else {
			if(strcmp(sy_str, "all") == 0){
				sy_id = 0;
			}
			else if( !pinyin_map.ym_to_id(sy_str, &sy_id) ) {
				printf("\n!!! Map 错误 !!!\n");
				lay_trie.reset_layer_tree();
				sy_num = 0;
				continue;
			}
			if( !lay_trie.process_yunmu(sy_id, NULL, 0) ) {
				printf("\n !!! 韵母处理错误 !!!\n");
				lay_trie.reset_layer_tree();
				sy_num = 0;
				continue;
			}

			sy_num++;
		}
		printf("\n");

		if( sy_num >= kMaxSyIdNum ){
			printf("\n此轮输完, 重置数据, 进行下一轮.... \n");
			lay_trie.reset_layer_tree();
			sy_num = 0;
			continue;
		}
	} //while

	return true;
}

bool test_uni_gram()
{
	UniGram uni_gram;
	if( !uni_gram.load_uni_gram() ) {
		printf("\n !!! unigram 加载失败 !!! \n");
		return false;
	}
	printf("\n******下面开始统计 unigram******\n");
	uni_gram.stat_uni_gram();
}

bool test_lemma_str()
{
	LemmaFetch lemma_fetch;
	if( !lemma_fetch.load_lemma_str() ) {
		printf("\n !!! 加载lemma_str.dat出错 !!! \n");
		return false;
	}

	printf("\n******下面开始统计 Lemma 本体******\n");
	lemma_fetch.stat_lemma_str();
}

bool test_im_engine()
{
	ImEngine im_engine;
	if( !im_engine.init() ) {
		printf( "!!! 引擎初始化失败 in main function !!!" );
		return false;
	}

	printf("\n******开始测试引擎******\n");

	PinyinMap pinyin_map;

	bool flag = false;
	char sy_str[10];
	ShyunIdType sy_id;
	size_t sy_num = 0;

	while(true) {
		scanf("%s", sy_str);

		if(sy_num % 2 == 0) {
			if( !pinyin_map.sm_to_id(sy_str, &sy_id) ) {
				printf("\n!!! Map 错误 !!!\n");
				im_engine.reset_im_engine();
				sy_num = 0;
				continue;
			}
			if( !im_engine.process__input(sy_id) ) {
				printf("\n !!! 声母处理错误 !!!\n");
				im_engine.reset_im_engine();
				sy_num = 0;
				continue;
			}

			sy_num++;
		}
		else {
			if(strcmp(sy_str, ";") == 0){
				sy_id = 0;
			}
			else if( !pinyin_map.ym_to_id(sy_str, &sy_id) ) {
				printf("\n!!! Map 错误 !!!\n");
				im_engine.reset_im_engine();
				sy_num = 0;
				continue;
			}
			if( !im_engine.process__input(sy_id) ) {
				printf("\n !!! 韵母处理错误 !!!\n");
				im_engine.reset_im_engine();
				sy_num = 0;
				continue;
			}

			sy_num++;
		}
		printf("\n");

		if( sy_num >= kMaxSyIdNum ){
			printf("\n此轮输完, 重置数据, 进行下一轮.... \n");
			im_engine.reset_im_engine();
			sy_num = 0;
			continue;
		}
	} //while

	return true;
}
