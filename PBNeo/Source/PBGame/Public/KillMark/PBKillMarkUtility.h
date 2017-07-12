#pragma once

#include "EngineMinimal.h"

//@note: 킬마크 타입의 주체가 되는 인터페이스 제공용 구조체이다. 
struct FKillMarkCondition
{
	friend class FPBKillMarkConditionChecker;

	typedef int32 IdentityType;
	
protected:
	FKillMarkCondition();
public:
	virtual ~FKillMarkCondition();
public:
	virtual IdentityType GetUniqueID();
	virtual FString GetTypeAsString() = 0;
	virtual EKillMarkCategory GetCategory() = 0;
protected:
	virtual bool CheckInternal(class APBPlayerState *KillerPlayerState, class APBPlayerState *VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &TakeDamageInfo) = 0;
public:
	template <typename T>
	static bool Check(class APBPlayerState *KillerPlayerState, class APBPlayerState *VictimPlayerState, AActor *DamageCauser, const FTakeDamageInfo &TakeDamageInfo)
	{
		FKillMarkCondition **FoundItem = ObjectMap.Find(T::UniqueID);
		if (false == ensure(FoundItem && *FoundItem))
			return false;

		return (*FoundItem)->CheckInternal(KillerPlayerState, VictimPlayerState, DamageCauser, TakeDamageInfo);
	}
	static FString GetTypeAsString(IdentityType Type);
	static EKillMarkCategory GetCategory(IdentityType Type);
public:
	static const IdentityType InvalidID;
protected:
	static TArray<FKillMarkCondition *>  ObjectArray;
	static TMap<IdentityType, FKillMarkCondition *>  ObjectMap;
};
//@note: 킬마크 타입은 int일 뿐.
typedef FKillMarkCondition::IdentityType KillMarkType;


//@note: 이 구조체를 상속받아 선언하면 새로운 킬마트 타입을 선언하는 것과 같다.
// 선언하는 순서대로 TypeObject가 생성, FKillMarkCondition::ObjectArray에 순서대로 추가된다.
template <typename T>
struct FKillMarkConditionImp : FKillMarkCondition
{
	friend struct FKillMarkCondition;
	friend class FPBKillMarkConditionChecker;

protected:
	FKillMarkConditionImp()
	{
		UniqueID = ObjectArray.Num();
		ObjectArray.AddUnique(this);
		ObjectMap.Add(UniqueID, this);
	}
	virtual ~FKillMarkConditionImp() {}
public:
	virtual KillMarkType GetUniqueID() final { return UniqueID; }
	static KillMarkType GetStaticUID() { return UniqueID; }
	static T *GetTypeObject() { return const_cast<T *>(&TypeObject); }
protected:
	virtual bool CheckInternal(class APBPlayerState *KillerPlayerState, class APBPlayerState *VictimPlayerState, class AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo) override = 0;
private:
	/*
		선언과 동시에 스스로를 생성하도록 하기 위한 용도.
	*/
	static const T TypeObject;
	static const uintptr_t TypeObjectAddr;
private:
	//@note: UniqueID는 KillMark 타입 생성 순서대로 지정된다.
	static KillMarkType UniqueID;
};
template <typename T> const T FKillMarkConditionImp<T>::TypeObject;
template <typename T> KillMarkType FKillMarkConditionImp<T>::UniqueID;
template <typename T> const uintptr_t FKillMarkConditionImp<T>::TypeObjectAddr = reinterpret_cast<uintptr_t>(&TypeObject);

//@note: 킬마크 선언용 도움 매크로.
#define PBKillMarkType(T, Category) \
struct T : FKillMarkConditionImp<T>\
{ \
	virtual FString GetTypeAsString() final { return TEXT(# T); }\
	virtual EKillMarkCategory GetCategory() final { return Category; }\
protected: \
	virtual bool CheckInternal(class APBPlayerState *KillerPlayerState, class APBPlayerState *VictimPlayerState, class AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo) final;\
} *Obj_ ## T = T::GetTypeObject();
