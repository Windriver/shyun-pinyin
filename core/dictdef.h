// Copyright 2011, ZhongYi-i  Inc
// Author: Windriver < http://windriver.github.io >

// Copyright 2011, ZhongYi-i  Inc
// Author: Windriver < http://windriver.github.io >

#ifndef  __INCLUDE_DICTDEF_H__
#define __INCLUDE_DICTDEF_H__

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;

typedef signed char        int8;
typedef short              int16;
typedef int                int32;
typedef long long          int64;
typedef unsigned long long uint64;

 typedef unsigned short char16;

 typedef uint16 ShmuIdType;
 typedef uint16 YunmuIdType;
 typedef uint16 ShyunIdType;

 typedef size_t LemmaIdType;

const size_t kMaxLemmaSize = 9;
const size_t kMaxSyIdNum = 2 * kMaxLemmaSize;

//系统词典中词元的最大长度
const size_t kMaxSysLmaSize = 4;

//声母总数
const size_t kAllShmuNum = 26;

//韵母总数
const size_t kAllYunmuNum = 34;

//每个声母可跟的最大韵母数
const size_t kMaxYunmuNum = 25;

//多层Trie树的最大层数
const size_t kMaxShyunLayer = 9;

//在整句生成时, 它代表负无穷大的频率
const float kMinLmaFreq = -1000.0;
//
const size_t kNodeNumLayer00 = 1 + 10;
const size_t kNodeNumLayer01 = 26 + 10;
const size_t kNodeNumLayer02 = 411 + 10;
const size_t kNodeNumLayer03 = 7392 + 10;
const size_t kNodeNumLayer04 = 26183 + 10;
const size_t kNodeNumLayer05 = 12191 + 10;
const size_t kNodeNumLayer06 = 13370 + 10;
const size_t kNodeNumLayer07 = 6601 + 10;
const size_t kNodeNumLayer08 = 6619 + 10;

//根结点, 它有26个子结点
struct LmaNodeLayer00
{
	uint16 son_off_1st;
	uint16 num_of_son;
	uint16 lma_off_1st;
	uint16 num_of_lma;
};

//第 1 层有 26 个结点 .
//本层结点中, 最多词元数量为: 1692 , 最多子节点数量为 : 26
struct LmaNodeLayer01
{
	 uint16 son_off_1st;
	 uint16 num_of_son;
	 uint16 lma_off_1st;
	 uint16 num_of_lma;
	 ShyunIdType sm_id;
};

//第 2 层有 411 个结点 .
//本层结点中, 最多词元数量为: 312 , 最多子节点数量为 : 26
struct LmaNodeLayer02
{
	uint16 son_off_1st;
	uint16 num_of_son;
	uint16 lma_off_1st;
	uint16 num_of_lma;
	ShyunIdType ym_id;
};

//3层及以上的通用奇数层结点
struct LmaNodeLayerSm
{
	uint16 son_off_1st;
	uint16 lma_off_1st;
	uint8 num_of_son;
	uint8 num_of_lma;
	ShyunIdType sm_id;
};

//3层及以上的通用偶数层结点
struct LmaNodeLayerYm
{
	uint16 son_off_1st;
	uint16 lma_off_1st;
	uint8 num_of_son;
	uint8 num_of_lma;
	ShyunIdType ym_id;
};

//第 3 层有 7392 个结点 .
//本层结点中, 最多词元数量为: 63 , 最多子节点数量为 : 21
struct LmaNodeLayer03
{
	uint16 son_off_1st;
	uint16 lma_off_1st;
	uint8 num_of_son;
	uint8 num_of_lma;
};

//第 4 层有 26183 个结点 .
//本层结点中, 最多词元数量为: 19 , 最多子节点数量为 : 21
struct LmaNodeLayer04
{

};

//第 5 层有 12191 个结点 .
//本层结点中, 最多词元数量为: 6 , 最多子节点数量为 : 6
struct LmaNodeLayer05
{

};

//第 6 层有 13370 个结点 .
//本层结点中, 最多词元数量为: 2 , 最多子节点数量为 : 5
struct LmaNodeLayer06
{

};

//第 7 层有 6601 个结点 .
//本层结点中, 最多词元数量为: 2 , 最多子节点数量为 : 2
struct LmaNodeLayer07
{

};

//第 8 层有 6619 个结点 .
//本层结点中, 最多词元数量为: 2 , 最多子节点数量为 : 0
struct LmaNodeLayer08
{

};

#endif //__INCLUDE_DICTDEF_H__
