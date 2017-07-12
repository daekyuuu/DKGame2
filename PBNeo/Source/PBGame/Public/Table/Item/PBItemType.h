// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PBItemType.generated.h"

//#define ITEM_BIT_SIZE_TYPE 6
//#define ITEM_BIT_SIZE_SUB 8
//#define ITEM_BIT_SIZE_INDEX 12
//
//#define ITEM_ID( type, sub, idx ) \
//	(unsigned int)( (type<<24) | (sub<<16) | (idx) )
//
//#define GOODS_ID( type, sub, idx ) \
//	(unsigned int)( (1<<28) | (type<<24) | (sub<<16) | (idx) )
//#define GET_GOODS_TYPE( id ) ( (id >> 28) & 0x0F )
//
//#define GET_ITEM_TYPE( id ) ( (id >> 24) & 0x0F )
//#define GET_ITEM_SUBTYPE( id ) ((id >> 16) & 0xFF )
//#define GET_ITEM_INDEX( id ) (id & 0xFFFF)

// 자릿수로 type, sub, index 를 구한다.
#define GET_ITEM_NUM(id, from, to) \
		(int32)((id % int32(pow(10, to))) / int32(pow(10, from-1)))

#define ITEM_TYPE_DIGIT_MAX 8
#define ITEM_TYPE_DIGIT_MIN 8
#define ITEM_SUBTYPE_DIGIT_MAX 7
#define ITEM_SUBTYPE_DIGIT_MIN 6

#define GET_ITEM_TYPE(id) GET_ITEM_NUM(id, ITEM_TYPE_DIGIT_MIN, ITEM_TYPE_DIGIT_MAX)
#define GET_ITEM_SUBTYPE(id) GET_ITEM_NUM(id, ITEM_SUBTYPE_DIGIT_MIN, ITEM_SUBTYPE_DIGIT_MAX)

// 아래 Enum 번호는 아이템ID 규칙을 참고해서 동일하게 맞춰야 합니다.
UENUM()
enum class EPBItemType : uint8
{
	None = 0,
	Character = 1,
	Weapon = 3,
	Functional,
};

// 이건 아직 기획이 없다
UENUM()
enum class EPBItemSubType : uint8
{
	None = 0,
};
