#pragma once

#include "EngineMinimal.h"

//@note: ų��ũ Ÿ���� ��ü�� �Ǵ� �������̽� ������ ����ü�̴�. 
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
//@note: ų��ũ Ÿ���� int�� ��.
typedef FKillMarkCondition::IdentityType KillMarkType;


//@note: �� ����ü�� ��ӹ޾� �����ϸ� ���ο� ų��Ʈ Ÿ���� �����ϴ� �Ͱ� ����.
// �����ϴ� ������� TypeObject�� ����, FKillMarkCondition::ObjectArray�� ������� �߰��ȴ�.
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
		����� ���ÿ� �����θ� �����ϵ��� �ϱ� ���� �뵵.
	*/
	static const T TypeObject;
	static const uintptr_t TypeObjectAddr;
private:
	//@note: UniqueID�� KillMark Ÿ�� ���� ������� �����ȴ�.
	static KillMarkType UniqueID;
};
template <typename T> const T FKillMarkConditionImp<T>::TypeObject;
template <typename T> KillMarkType FKillMarkConditionImp<T>::UniqueID;
template <typename T> const uintptr_t FKillMarkConditionImp<T>::TypeObjectAddr = reinterpret_cast<uintptr_t>(&TypeObject);

//@note: ų��ũ ����� ���� ��ũ��.
#define PBKillMarkType(T, Category) \
struct T : FKillMarkConditionImp<T>\
{ \
	virtual FString GetTypeAsString() final { return TEXT(# T); }\
	virtual EKillMarkCategory GetCategory() final { return Category; }\
protected: \
	virtual bool CheckInternal(class APBPlayerState *KillerPlayerState, class APBPlayerState *VictimPlayerState, class AActor *DamageCauser, const FTakeDamageInfo &InTakeDamageInfo) final;\
} *Obj_ ## T = T::GetTypeObject();
