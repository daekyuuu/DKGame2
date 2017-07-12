// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBKillMarkTypes.h"
#include "PBKillMarkWidget.h"

PBGetterWidgetBlueprintProperty_Implementation(UWidgetAnimation, UPBKillMarkWidget, KillMarkIn);

PBGetterWidgetBlueprintVariable_Implementation(UTextBlock, UPBKillMarkWidget, KillMessage);

UPBKillMarkWidget::UPBKillMarkWidget(const FObjectInitializer& ObjectInitializer)
{
	FirstAnimationDelayTime = 1.0f;
}

void UPBKillMarkWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindWidgetEvent(OnShowKillMark, UPBKillMarkWidget::NotifyKillMark);
	BindWidgetEvent(OnWarmupStarted, UPBKillMarkWidget::NotifyWarmupStarted);

	Hide();
}

void UPBKillMarkWidget::NativeDestruct()
{
	UnbindWidgetEvent(OnShowKillMark);
	UnbindWidgetEvent(OnWarmupStarted);

	Super::NativeDestruct();
}

void UPBKillMarkWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UPBKillMarkWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);

	if (Animation && GetBPP_KillMarkIn() == Animation)
	{
		PlayAnimationNextKillMark();
	}
}

FText UPBKillMarkWidget::GetKillMarkText() const
{
	FText text = FText::FromString(FString("------"));
	if (KillMarkQueue.Num() > 0)
	{
		FString str = FKillMarkCondition::GetTypeAsString(KillMarkQueue.Last());
		
		if (false == str.IsEmpty())
			text = FText::FromString(str);
	}

	return text;
}

void UPBKillMarkWidget::PlayAnimationNextKillMark()
{
	Hide();
	DequeueKillMark();
	if (IsTheKillMarkQueueEmpty())
	{
		bIsPlayingKillMark = false;
	}
	else
	{
		Show();
		if (GetBPV_KillMessage())
		{
			GetBPV_KillMessage()->SetText(GetKillMarkText());
		}
		PlayAnimation(GetBPP_KillMarkIn());
		PlayKillMarkSound();
	}
}

void UPBKillMarkWidget::PlayKillMarkSound() const
{
	if (KillMarkQueue.Num() > 0)
	{
		//Note: types of EKillMarkTypes and EPBVoiceType are expected same member name.
		KillMarkType CurKillMarkType = KillMarkQueue.Last();
		FString str = FKillMarkCondition::GetTypeAsString(CurKillMarkType);

		EPBVoiceType VoiceType = EnumFromString(EPBVoiceType, str);

		if (EPBVoiceType::None != VoiceType)
		{
			auto PBCharacter = GetOwningCharacter();
			if (IsSafe(PBCharacter))
			{
				PBCharacter->PlayVoice(VoiceType);
			}
		}
	}
}

void UPBKillMarkWidget::NotifyKillMark(const TArray<int32>& killMarkIds, EWeaponType WeapType)
{
	// Implement in BP
	// Enqueue() -> Show() -> Play()

	if (nullptr == GetWorld())
		return;

	EnqueueKillMark(killMarkIds, WeapType);

	if (false == bIsPlayingKillMark)
	{
		bIsPlayingKillMark = true;

		FTimerHandle DisposableTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(DisposableTimerHandle
		, [&]()
		{
			Show();
			if (GetBPV_KillMessage())
			{
				GetBPV_KillMessage()->SetText(GetKillMarkText());
			}
			PlayAnimation(GetBPP_KillMarkIn());
			PlayKillMarkSound();
		}, FirstAnimationDelayTime, false);
	}
}

void UPBKillMarkWidget::NotifyWarmupStarted()
{
	EmptyKillMarkQueue();
	StopAnimation(GetBPP_KillMarkIn());
	PlayAnimationNextKillMark();
}

void UPBKillMarkWidget::EnqueueKillMark(const TArray<int32>& KillMarkIds, EWeaponType WeapType)
{
	/**
	* Kill mark printing RULE
	* A) 다음 순서로 최대 3개까지만 출력한다.( Rare -> Special -> Revenge -> Normal)
	* B) 만약 Special이 출력되면 Normal은 출력하지 않는다.
	* C) 동일 분류에서 2개 이상을 만족하면 그 중 상위의 1개만 출력 한다.
	*/

	TArray<KillMarkType> Rares;
	TArray<KillMarkType> Specials;
	TArray<KillMarkType> Revenges;
	TArray<KillMarkType> Normal;
	TArray<KillMarkType> SortedKillMarkIds;
	


	// remove the duplications and sort as priority of kill mark(It follow the enum order)
	for (auto id : KillMarkIds)
	{
		SortedKillMarkIds.AddUnique(id);
	}
	SortedKillMarkIds.Sort(TLess<KillMarkType>());

	// Collect ids to each category's container
	for (auto id : SortedKillMarkIds)
	{
		auto Category = FKillMarkCondition::GetCategory(id);
		switch (Category)
		{

		case EKillMarkCategory::Rare:
			Rares.Add(id);
			break;
		case EKillMarkCategory::Special:
			Specials.Add(id);
			break;
		case EKillMarkCategory::Revenge:
			Revenges.Add(id);
			break;
		case EKillMarkCategory::Normal:
			Normal.Add(id);
			break;
		}
	}

	// Count enqueue to restrict amount
	int32 EnqueueCnt = 0;


	// only use index 0 (to follow the rule C)
	if (Rares.Num() > 0)
	{
		KillMarkQueue.Insert(Rares[0], 0);
		EnqueueCnt++;
	}

	if (Specials.Num() > 0)
	{
		KillMarkQueue.Insert(Specials[0], 0);
		EnqueueCnt++;
	}

	if (Revenges.Num() > 0)
	{
		KillMarkQueue.Insert(Revenges[0], 0);
		EnqueueCnt++;
	}

	// To follow rule B
	if (Normal.Num() > 0 && Specials.Num() == 0)
	{
		// to follow rule A
		if (EnqueueCnt < 3)
		{
			KillMarkQueue.Insert(Normal[0], 0);
		}
	}

}

void UPBKillMarkWidget::DequeueKillMark()
{
	if (KillMarkQueue.Num() > 0)
	{
		KillMarkQueue.Pop();
	}
}


void UPBKillMarkWidget::EmptyKillMarkQueue()
{
	KillMarkQueue.Empty();
}

bool UPBKillMarkWidget::IsTheKillMarkQueueEmpty() const
{
	return KillMarkQueue.Num() == 0;
}

