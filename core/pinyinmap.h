// Copyright 2011, ZhongYi-i  Inc
// Author: Windriver < http://windriver.github.io >

#ifndef __INCLIDE_PINYINMAP_H__
#define __INCLIDE_PINYINMAP_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <string>
#include <map>
#include <iostream>
#include "dictdef.h"

class PinyinMap
{
private:
	std::map<std::string, ShmuIdType> sm_str_id_;
	std::map<std::string, YunmuIdType> ym_str_id_;

public:
	PinyinMap();
	~PinyinMap();

	bool pinyin_to_sy( const char* py_str, ShmuIdType* sm_id, YunmuIdType* ym_id );

	bool sm_to_id(const char* sm_str, ShyunIdType* sm_id);

	bool ym_to_id(const char* ym_str, ShyunIdType* ym_id);
private:
	bool build_sm_map();
	bool build_ym_map();
};
#endif