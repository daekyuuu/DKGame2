// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBAIStart.h"


APBAIStart::APBAIStart()
{

}

bool APBAIStart::IsAllowed(AController* Player)
{
	APBAIController* AIController = Cast<APBAIController>(Player);
	return AIController != nullptr;
}
