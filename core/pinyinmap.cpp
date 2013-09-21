// Copyright 2011, ZhongYi-i  Inc
// Author: Windriver < http://windriver.github.io >

﻿#include "pinyinmap.h"

PinyinMap::PinyinMap()
{
	sm_str_id_.clear();
	ym_str_id_.clear();
	build_sm_map();
	build_ym_map();
}

PinyinMap::~PinyinMap()
{

}

bool PinyinMap::pinyin_to_sy( const char* py_str, ShmuIdType* sm_id, YunmuIdType* ym_id )
{
	//先把拼音串分成声母串和韵母串
	//再使用map将声母串和韵母串分别映射成相应的声母Id和韵母Id
 	if( NULL == py_str || NULL == sm_id || NULL == ym_id ) {
 		return false;
 	}

	char sm_str_arr[10];
	char ym_str_arr[10];
	int py_len, sm_len, ym_len;
	
	py_len = strlen( py_str );
	if( 'h' == py_str[1] ) {
		sm_len = 2;
		ym_len = py_len - 2;
		strncpy( sm_str_arr, py_str, 2 );
		sm_str_arr[sm_len] = '\0';
		strncpy( ym_str_arr, py_str + sm_len, ym_len );
		ym_str_arr[ym_len] = '\0';
	}
	else if( 'a' == py_str[0] || 'o' == py_str[0] || 'e' == py_str[0] ) {
		sm_len = 1;
		ym_len = py_len;
		strncpy( sm_str_arr, py_str, 1 );
		sm_str_arr[sm_len] = '\0';
		strncpy( ym_str_arr, py_str, ym_len );
		ym_str_arr[ym_len] = '\0';
	}
	else {
		sm_len = 1;
		ym_len = py_len - 1;
		strncpy( sm_str_arr, py_str, 1 );
		sm_str_arr[sm_len] = '\0';
		strncpy( ym_str_arr, py_str + 1, ym_len );
		ym_str_arr[ym_len] = '\0';
	}

	std::string sm_str( sm_str_arr );
	std::string ym_str( ym_str_arr );
//	std::cout<<sm_str<<"-"<<ym_str<<" ";

	//检测声母或者韵母是否在map中, 作用有:
	//1, 在创建多层Trie树时, 如不在,则标出来,然后手工从词库中删去相应的词元(Lemma)
	std::map<std::string, ShmuIdType>::iterator it_sm = sm_str_id_.find(sm_str);
	std::map<std::string, YunmuIdType>::iterator it_ym = ym_str_id_.find(ym_str);
	if( sm_str_id_.end() == it_sm || ym_str_id_.end() == it_ym ){
		std::cout<<sm_str<<"-"<<ym_str<<" ";
		printf("Map error: cannot find the key-value !\n");
		return false;
	}
	*sm_id = sm_str_id_[sm_str];
	*ym_id = ym_str_id_[ym_str];
//	printf("%d-%d ", sm_str_id_[sm_str], ym_str_id_[ym_str]);


	return true;
}

bool PinyinMap::sm_to_id( const char* sm_str, ShyunIdType* sm_id )
{
	std::string sm_string( sm_str );
	std::map<std::string, ShmuIdType>::iterator it_sm = sm_str_id_.find(sm_string);
	if(sm_str_id_.end() == it_sm) {
		return false;
	}

	*sm_id = sm_str_id_[sm_string];
	return true;
}

bool PinyinMap::ym_to_id( const char* ym_str, ShyunIdType* ym_id )
{
	std::string ym_string( ym_str );
	std::map<std::string, ShmuIdType>::iterator it_ym = ym_str_id_.find(ym_string);
	if(ym_str_id_.end() == it_ym) {
		return false;
	}

	*ym_id = ym_str_id_[ym_string];
	return true;
}

bool PinyinMap::build_sm_map()
{
	sm_str_id_.insert( std::pair<std::string, ShmuIdType>("b", 0) );   sm_str_id_.insert(std:: pair<std::string, ShmuIdType>("p", 1) ); //1
	sm_str_id_.insert( std::pair<std::string, ShmuIdType>("m", 2) );   sm_str_id_.insert( std::pair<std::string, ShmuIdType>("f", 3) ); //2
	sm_str_id_.insert( std::pair<std::string, ShmuIdType>("d", 4) );   sm_str_id_.insert( std::pair<std::string, ShmuIdType>("t", 5) ); //3
	sm_str_id_.insert( std::pair<std::string, ShmuIdType>("n", 6) );   sm_str_id_.insert( std::pair<std::string, ShmuIdType>("l", 7) );//4
	sm_str_id_.insert( std::pair<std::string, ShmuIdType>("g", 8) );   sm_str_id_.insert( std::pair<std::string, ShmuIdType>("k", 9) ); //5
	sm_str_id_.insert( std::pair<std::string, ShmuIdType>("h", 10) ); sm_str_id_.insert( std::pair<std::string, ShmuIdType>("j", 11) ); //6
	sm_str_id_.insert( std::pair<std::string, ShmuIdType>("q", 12) ); sm_str_id_.insert( std::pair<std::string, ShmuIdType>("x", 13) ); //7
	sm_str_id_.insert( std::pair<std::string, ShmuIdType>("zh", 14) ); sm_str_id_.insert( std::pair<std::string, ShmuIdType>("ch", 15) ); //8
	sm_str_id_.insert( std::pair<std::string, ShmuIdType>("sh", 16) ); sm_str_id_.insert( std::pair<std::string, ShmuIdType>("r", 17) ); //9
	sm_str_id_.insert( std::pair<std::string, ShmuIdType>("z", 18) ); sm_str_id_.insert( std::pair<std::string, ShmuIdType>("c", 19) ); //10
	sm_str_id_.insert( std::pair<std::string, ShmuIdType>("s", 20) ); sm_str_id_.insert( std::pair<std::string, ShmuIdType>("y", 21) ); //11
	sm_str_id_.insert( std::pair<std::string, ShmuIdType>("w", 22) );  sm_str_id_.insert( std::pair<std::string, ShmuIdType>("a", 23) ); //12
	sm_str_id_.insert( std::pair<std::string, ShmuIdType>("e", 24) );  sm_str_id_.insert( std::pair<std::string, ShmuIdType>("o", 25) ); //13

	return true;
}

bool PinyinMap::build_ym_map()
{
	ym_str_id_.insert( std::pair<std::string, YunmuIdType>("", 0) );
	ym_str_id_.insert( std::pair<std::string, YunmuIdType>("a", 1) );         ym_str_id_.insert( std::pair<std::string, YunmuIdType>("ai", 2) );  //1
	ym_str_id_.insert( std::pair<std::string, YunmuIdType>("an", 3) );       ym_str_id_.insert( std::pair<std::string, YunmuIdType>("ang", 4) );  //2
	ym_str_id_.insert( std::pair<std::string, YunmuIdType>("ao", 5) );       ym_str_id_.insert( std::pair<std::string, YunmuIdType>("e", 6) );  //3
	ym_str_id_.insert( std::pair<std::string, YunmuIdType>("ei", 7) );        ym_str_id_.insert( std::pair<std::string, YunmuIdType>("en", 8) );  //4
	ym_str_id_.insert( std::pair<std::string, YunmuIdType>("eng", 9) );    ym_str_id_.insert( std::pair<std::string, YunmuIdType>("er", 10) );  //5
	ym_str_id_.insert(std:: pair<std::string, YunmuIdType>("o", 11) );      ym_str_id_.insert( std::pair<std::string, YunmuIdType>("ong", 12) );  //6
	ym_str_id_.insert( std::pair<std::string, YunmuIdType>("ou", 13) );    ym_str_id_.insert( std::pair<std::string, YunmuIdType>("i", 14) );  //7
	ym_str_id_.insert( std::pair<std::string, YunmuIdType>("ia", 15) );      ym_str_id_.insert( std::pair<std::string, YunmuIdType>("ian", 16) );  //8
	ym_str_id_.insert( std::pair<std::string, YunmuIdType>("iang", 17) ); ym_str_id_.insert( std::pair<std::string, YunmuIdType>("iao", 18) );  //9
	ym_str_id_.insert( std::pair<std::string, YunmuIdType>("ie", 19) );      ym_str_id_.insert( std::pair<std::string, YunmuIdType>("in", 20) );  //10
	ym_str_id_.insert( std::pair<std::string, YunmuIdType>("ing", 21) );    ym_str_id_.insert( std::pair<std::string, YunmuIdType>("iong", 22) );  //11
	ym_str_id_.insert( std::pair<std::string, YunmuIdType>("iu", 23) );       ym_str_id_.insert( std::pair<std::string, YunmuIdType>("u", 24) );  //12
	ym_str_id_.insert( std::pair<std::string, YunmuIdType>("ua", 25) );      ym_str_id_.insert( std::pair<std::string, YunmuIdType>("uai", 26) );  //13
	ym_str_id_.insert( std::pair<std::string, YunmuIdType>("uan", 27) );    ym_str_id_.insert( std::pair<std::string, YunmuIdType>("uang", 28) );  //14
	ym_str_id_.insert( std::pair<std::string, YunmuIdType>("ue", 29) );      ym_str_id_.insert( std::pair<std::string, YunmuIdType>("ui", 30) );  //15
	ym_str_id_.insert( std::pair<std::string, YunmuIdType>("un", 31) );      ym_str_id_.insert( std::pair<std::string, YunmuIdType>("uo", 32) );  //16
	ym_str_id_.insert( std::pair<std::string, YunmuIdType>("v", 33) );   //17

	return true;
}



