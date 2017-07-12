#include "PBGame.h"
#include "PBKillMarkUtility.h"

#define KILLMARK_INVALID_ID -1

const FKillMarkCondition::IdentityType FKillMarkCondition::InvalidID = KILLMARK_INVALID_ID;
TArray<FKillMarkCondition *> FKillMarkCondition::ObjectArray;
TMap<FKillMarkCondition::IdentityType, FKillMarkCondition *> FKillMarkCondition::ObjectMap;

FKillMarkCondition::FKillMarkCondition() 
{
}

FKillMarkCondition::~FKillMarkCondition() { }

FKillMarkCondition::IdentityType FKillMarkCondition::GetUniqueID()
{
	return InvalidID;
}

FString FKillMarkCondition::GetTypeAsString(IdentityType Type)
{
	auto FoundTypeObject = ObjectMap.Find(Type);
	if (nullptr == FoundTypeObject || nullptr == *FoundTypeObject)
		return FString();

	return (*FoundTypeObject)->GetTypeAsString();
}

EKillMarkCategory FKillMarkCondition::GetCategory(IdentityType Type)
{
	auto FoundTypeObject = ObjectMap.Find(Type);
	if (nullptr == FoundTypeObject || nullptr == *FoundTypeObject)
		return EKillMarkCategory::None;

	return (*FoundTypeObject)->GetCategory();
}
