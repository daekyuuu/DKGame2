// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBUserWidget.h"
#include "Net/S2MODefine.h"
#include "PBWidget_SystemMessageListBox.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBWidget_SystemMessageListBox : public UPBUserWidget
{
	GENERATED_BODY()

	PBGetterWidgetBlueprintVariable(UScrollBox, MessageScrollBox);

public:
	UPBWidget_SystemMessageListBox(const FObjectInitializer &Initializer);
	virtual void NativeConstruct() override;

protected:
	void OnNoticeEvent(const FString &InMessage);
	void OnEndTimer(UPBTimerWidget *WidgetTimed);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Templates")
	TSubclassOf<UPBUserWidget> PBRichTextBlockClass;

public:
	// InTimeStamp�� 0�̸� ���� �ð����� ����Ѵ�.
	void AddTextBlock(const FString &InText, int64 InTimeStamp = 0);
};

//@note: �ӽ÷� �����. ���� �������� ��ɿ� ���Ե� Ŭ�����̹Ƿ� UMG���� ���� ����� �������� �ʽ��ϴ�.
UCLASS()
class UPBRichTextBlock : public UTextLayoutWidget
{
	GENERATED_BODY()

public:
	UPBRichTextBlock(const FObjectInitializer& ObjectInitializer);

	// UWidget interface
	virtual void SynchronizeProperties() override;
	// End of UWidget interface

	// UVisual interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// End of UVisual interface

#if WITH_EDITOR
	// UWidget interface
	virtual const FText GetPaletteCategory() override;
	// End UWidget interface
#endif

public:
	void SetText(const FString &InText);

	void SetJustification(ETextJustify::Type InJustification);

protected:
	/** The text to display */
	UPROPERTY(EditAnywhere, Category = Content, meta = (MultiLine = "true"))
		FText Text;

	/** A bindable delegate to allow logic to drive the text of the widget */
	UPROPERTY()
		FGetText TextDelegate;

	/** The default font for the text. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
		FSlateFontInfo Font;

	/** The default color for the text. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
		FLinearColor Color;

	UPROPERTY(EditAnywhere, Instanced, Category = Decorators)
		TArray<class URichTextBlockDecorator*> Decorators;

protected:
	FTextBlockStyle DefaultStyle;

	/** Native Slate Widget */
	TSharedPtr<SRichTextBlock> MyRichTextBlock;

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface
};