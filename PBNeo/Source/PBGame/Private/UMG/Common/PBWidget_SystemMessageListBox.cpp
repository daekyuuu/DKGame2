// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWidget_SystemMessageListBox.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextWidgetTypes.h"
#include "Components/RichTextBlockDecorator.h"
#include "Online/PBNetClientInterface.h"
#include "Online/PBPacketManager.h"

PBGetterWidgetBlueprintVariable_Implementation(UScrollBox, UPBWidget_SystemMessageListBox, MessageScrollBox);

UPBWidget_SystemMessageListBox::UPBWidget_SystemMessageListBox(const FObjectInitializer &Initializer)
	: Super(Initializer)
{
	SetEnableAutoDestroy(false);
}

void UPBWidget_SystemMessageListBox::NativeConstruct()
{
	Super::NativeConstruct();

	UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(this);
	if (ensure(ClientInterface))
	{
		ClientInterface->OnNoticeEvt.AddUObject(this, &UPBWidget_SystemMessageListBox::OnNoticeEvent);
	}

	/*
	if (nullptr == GetWorld())
		return;

	auto GI = Cast<UPBGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI)
	{
		for (auto MC : GI->SystemMessageContexts)
		{
			AddTextBlock(MC.Value, MC.Key);
		}
	}
	*/
}

void UPBWidget_SystemMessageListBox::OnNoticeEvent(const FString &InMessage)
{
	//@todo:   InMessage를 파싱하여, 다양하게 표현해주려면 이곳에서.

	AddTextBlock(InMessage);
}

void UPBWidget_SystemMessageListBox::OnEndTimer(UPBTimerWidget *WidgetTimed)
{
	auto SB = GetBPV_MessageScrollBox();
	if (false == ensure(SB))
		return;

	//if (GetWorld())
	//{
	//	auto GI = Cast<UPBGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	//	if (GI)
	//	{
	//		GI->SystemMessageContexts.Remove(WidgetTimed->GetTimeStamp());
	//	}
	//}

	WidgetTimed->SetVisibility(ESlateVisibility::Hidden);
	SB->RemoveChild(WidgetTimed);

	auto NextItem = Cast<UPBTimerWidget>(SB->GetChildAt(0));
	if (nullptr != NextItem)
	{
		NextItem->StartTimer();
	}
}

void UPBWidget_SystemMessageListBox::AddTextBlock(const FString &InText, int64 InTimeStamp)
{
	auto SB = GetBPV_MessageScrollBox();
	if (nullptr == SB)
		return;

	auto World = GetWorld();
	if (nullptr == World)
		return;

	if (false == ensure(PBRichTextBlockClass))
		return;

	auto TextBlockWidget = CreateWidget<UPBTimerWidget>(World, PBRichTextBlockClass);
	if (false == ensure(TextBlockWidget))
		return;

	if (InTimeStamp > 0)
	{
		TextBlockWidget->SetTimeStamp(InTimeStamp);
	}

	auto TextBlockComp = TextBlockWidget->GetWidgetFromBP<UPBRichTextBlock>("Text");
	if (false == ensureMsgf(TextBlockComp, TEXT(R"(Couldn't find "Text" from Blueprint.)")))
	{
		TextBlockWidget->SetVisibility(ESlateVisibility::Hidden);
		TextBlockWidget = nullptr;
		return;
	}
	
	TextBlockComp->SetText(InText);

	TextBlockWidget->LifeTimeInSeconds = 2.0f;
	TextBlockWidget->bIsFocusable = false;
	TextBlockWidget->OnEndTimer.AddUObject(this, &UPBWidget_SystemMessageListBox::OnEndTimer);
	if (SB->GetChildrenCount() == 0)
	{
		TextBlockWidget->bTrigManually = false;
	}

	SB->AddChild(TextBlockWidget);

	//if (InTimeStamp == 0)
	//{
	//	auto GI = Cast<UPBGameInstance>(UGameplayStatics::GetGameInstance(World));
	//	if (GI)
	//	{
	//		GI->SystemMessageContexts.Add(TextBlockWidget->GetTimeStamp(), InText);
	//	}
	//}
}





#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
// URichTextBlock

UPBRichTextBlock::UPBRichTextBlock(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (!IsRunningDedicatedServer())
	{
		static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(TEXT("/Engine/EngineFonts/Roboto"));
		Font = FSlateFontInfo(RobotoFontObj.Object, 12, FName("Regular"));
	}
	Color = FLinearColor::White;

	Decorators.Add(ObjectInitializer.CreateOptionalDefaultSubobject<URichTextBlockDecorator>(this, FName("DefaultDecorator")));
}

void UPBRichTextBlock::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyRichTextBlock.Reset();
}

TSharedRef<SWidget> UPBRichTextBlock::RebuildWidget()
{
	//+ OnHyperlinkClicked = FSlateHyperlinkRun::FOnClick::CreateStatic(&RichTextHelper::OnBrowserLinkClicked, AsShared());
	//+ FHyperlinkDecorator::Create(TEXT("browser"), OnHyperlinkClicked))
	//+MakeShareable(new FDefaultRichTextDecorator(Font, Color));

	DefaultStyle.SetFont(Font);
	DefaultStyle.SetColorAndOpacity(Color);

	TArray< TSharedRef< class ITextDecorator > > CreatedDecorators;

	for (URichTextBlockDecorator* Decorator : Decorators)
	{
		if (Decorator)
		{
			CreatedDecorators.Add(Decorator->CreateDecorator(Font, Color));
		}
	}

	MyRichTextBlock =
		SNew(SRichTextBlock)
		.TextStyle(&DefaultStyle)
		.Decorators(CreatedDecorators);

	return MyRichTextBlock.ToSharedRef();
}

void UPBRichTextBlock::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	TAttribute<FText> TextBinding = OPTIONAL_BINDING(FText, Text);

	MyRichTextBlock->SetText(TextBinding);

	Super::SynchronizeTextLayoutProperties(*MyRichTextBlock);
}

void UPBRichTextBlock::SetText(const FString &InText)
{
	Text = FText::FromString(InText);
}

void UPBRichTextBlock::SetJustification(ETextJustify::Type InJustification)
{
	if (MyRichTextBlock.IsValid())
	{
		MyRichTextBlock->SetJustification(InJustification);
	}
}

#if WITH_EDITOR

const FText UPBRichTextBlock::GetPaletteCategory()
{
	return LOCTEXT("Common", "Common");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
