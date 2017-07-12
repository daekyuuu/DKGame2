// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include <functional>
#include "Blueprint/UserWidget.h"
#include "UMG.h"
#include "PBUserWidget.generated.h"

template <typename T> inline FString GetPropertyTagName()
{
	UClass *CompareType = T::StaticClass();

	if (CompareType == UWidgetAnimation::StaticClass())
	{
		return TEXT("_INST");
	}

	return FString();
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPBButtonFocusedEvent, UPBUserWidget*, PBWidget, bool, bFocused);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPBButtonReleasedEvent, UPBUserWidget*, PBWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPBButtonCanceledEvent, UPBUserWidget*, PBWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPBButtonGotNaviInputEvent, UPBUserWidget*, PBWidget, FKey, PressedKey);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPBLobbyTabChanged, int32, SlotIndex);

/**
 * 
 */
UCLASS(Abstract)
class PBGAME_API UPBUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = WidgetGeometry)
	void DesignTimeConstruct();
	virtual void DesignTimeConstruct_Implementation();

	virtual void OnWidgetRebuilt() override;

	UFUNCTION(BlueprintNativeEvent, Category = WidgetGeometry)
	void SyncProperties();
	 
	void SynchronizeProperties() override;

public:
	UPBUserWidget(const FObjectInitializer& ObjectInitializer);

	virtual void RemoveFromParent() final;
	
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void OnFocusReceived();

	/* ------------------------------------------------------------------------------- */
	// Geometry cache updated every tick
	/* ------------------------------------------------------------------------------- */

private:
	void UpdateGeometryCache(const FGeometry& MyGeometry);
	FGeometry GeometryCache;

	/* ------------------------------------------------------------------------------- */
	// Local/Absolute/Viewport coordinate conversion
	/* ------------------------------------------------------------------------------- */

	// These functions are only useful if your UMG is in its native 2d "HUD" space
	// "ViewportLocal" is the space where widget registers themselves in the viewport
	// "ViewportPixel" is the space where the widget actually is situated in the game world
	// "ViewportLayout" space ----DPI-Scaling----> "ViewportPixel" space

public:
	UFUNCTION(BlueprintCallable, Category = WidgetGeometry)
	FVector2D FromLocalToViewportLayout(FVector2D Input);

	UFUNCTION(BlueprintCallable, Category = WidgetGeometry)
	FVector2D FromLocalToViewportPixel(FVector2D Input);

	UFUNCTION(BlueprintCallable, Category = WidgetGeometry)
	FVector2D FromViewportPixelToLocal(FVector2D Input);

	/* ------------------------------------------------------------------------------- */
	// Getting Local Size more easily
	/* ------------------------------------------------------------------------------- */

public:
	UFUNCTION(BlueprintCallable, Category = WidgetGeometry)
	FVector2D GetCachedLocalSize();

	/* ------------------------------------------------------------------------------- */
	// Player Input Management
	/* ------------------------------------------------------------------------------- */
	
	FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

public:
	// Our best effort to navigate the widget tree and disable input without affecting visual appearance of the ui...
	UFUNCTION(BlueprintCallable, Category = WidgetInput)
	static void SetLockInputForAllChildren(UWidget* Widget, bool ToLock);

	// UserWidget that uses UButton for input control should disalbe the buttons if its own input is locked...
	UFUNCTION(BlueprintCallable, Category = WidgetInput)
	bool ButtonSubWidgetShouldBeEnabled() const;

protected:
	void InitInputLocking();
	bool bInputLocked;

	/* ------------------------------------------------------------------------------- */
	// Player Focus Management - Navigating Focus from Widget
	/* ------------------------------------------------------------------------------- */

public:
	UFUNCTION(BlueprintCallable, Category = WidgetFocus)
	static FEventReply TransferUserFocusThroughReply(
	const FFocusEvent& AcceptedFocusEvent, UPARAM(ref) FEventReply& Reply, UWidget* FocusWidget, bool bInAllUsers = false);

	// I will send the focus to this widget if it's not null 
	UPROPERTY(BlueprintReadWrite, Category = WidgetFocus)
	UWidget* FocusTransferTarget;

	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	/* ------------------------------------------------------------------------------- */
	// Player Focus Management - Navigating Focus from Outside
	/* ------------------------------------------------------------------------------- */

public:
	UFUNCTION(BlueprintCallable, Category = WidgetFocus)
	static void NavigateUserFocus(ULocalPlayer* Player, UWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = WidgetFocus)
	static void ClearUserFocus(ULocalPlayer* Player);

public:
	static void NavigateUserFocus(ULocalPlayer* Player, TSharedPtr<SWidget> Widget);

public:
	UFUNCTION(BlueprintCallable, Category = WidgetFocus)
	static void NavigateKeyboardFocus(UWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = WidgetFocus)
	static void ClearKeyboardFocus();

public:
	static void NavigateKeyboardFocus(TSharedPtr<SWidget> Widget);
	
	/* ------------------------------------------------------------------------------- */
	// Player Focus Management - Checking Focus
	/* ------------------------------------------------------------------------------- */

public:
	UFUNCTION(BlueprintCallable, Category = WidgetFocus)
	bool ContainOwnerUserFocus();

public:
	UFUNCTION(BlueprintCallable, Category = WidgetFocus)
	static bool ContainUserFocus(ULocalPlayer* Player, UWidget* Widget);

	static bool ContainUserFocus(int ControllerIndex, SWidget& Widget);

public:
	UFUNCTION(BlueprintCallable, Category = WidgetFocus)
	static bool ConTainKeyboardFocus(UWidget* Widget);

	static bool ContainKeyboardFocus(SWidget& Widget);

	/* ------------------------------------------------------------------------------- */
	// Player Focus Management - Focus Moving Override
	/* ------------------------------------------------------------------------------- */

protected:
	FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:
	UFUNCTION(BlueprintCallable, Category = WidgetFocus)
	void SetWidgetNeighboursOverride(UWidget* Up, UWidget* Down, UWidget* Left, UWidget* Right);

	UPROPERTY(BlueprintReadOnly, Category = WidgetFocus)
	TWeakObjectPtr<UWidget> UpWidget;

	UPROPERTY(BlueprintReadOnly, Category = WidgetFocus)
	TWeakObjectPtr<UWidget> DownWidget;

	UPROPERTY(BlueprintReadOnly, Category = WidgetFocus)
	TWeakObjectPtr<UWidget> LeftWidget;

	UPROPERTY(BlueprintReadOnly, Category = WidgetFocus)
	TWeakObjectPtr<UWidget> RightWidget;

private:
	void InitFocusMoving();

	/* ------------------------------------------------------------------------------- */
	// Widget Message system helper.
	/* ------------------------------------------------------------------------------- */

protected:
	TArray<FDelegateHandle> AddedDelegates;
	TSharedPtr<FPBWidgetEventDispatcher> GetMessageDispatcher() const;

	/* ------------------------------------------------------------------------------- */
	// Utility getter and setters
	/* ------------------------------------------------------------------------------- */

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Player")
	void SetOwningPlayerController(APlayerController* Owner);

protected:

	UFUNCTION(BlueprintCallable, Category = "PBWidget")
	class UPBGameInstance* GetPBGameInstance() const;

	UFUNCTION(BlueprintCallable, Category = "PBWidget")
	class APBGameState* GetPBGameState() const;

	UFUNCTION(BlueprintCallable, Category = "PBWidget")
	class APBPlayerState* GetPBPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "PBWidget")
	class APBCharacter* GetOwningCharacter() const;

	UFUNCTION(BlueprintCallable, Category = "PBWidget")
	class APBPlayerController* GetOwningPBPlayer() const;

public:
	template <typename T>
	T *GetWidgetFromBP(const FString &InWidgetName)
	{
		TArray<UWidget *> Widgets;
		WidgetTree->GetAllWidgets(Widgets);

		for (auto CurWidget : Widgets)
		{
			auto Widget = Cast<T>(CurWidget);
			if (Widget 
				//&& Widget->bIsVariable 
				&& Widget->GetFName().ToString() == InWidgetName)
			{
				return Widget;
			}
		}

		return nullptr;
	}

	template <typename T> inline T *GetPropertyFromBP(const FString &InPropertyName)
	{
		//@ref: https://answers.unrealengine.com/questions/427948/get-widget-animation-in-c.html
		FString PropertyName = InPropertyName;
		UProperty *PropertyLink = GetClass()->PropertyLink;
		while (PropertyLink != nullptr)
		{
			if (PropertyLink->GetClass() == UObjectProperty::StaticClass())
			{
				auto ObjectProperty = Cast<UObjectProperty>(PropertyLink);
				if (nullptr != ObjectProperty && ObjectProperty->PropertyClass == T::StaticClass())
				{
					//@note: �迭�� �ʿ��ϴٸ� �̰�����...
					UObject *Object = ObjectProperty->GetObjectPropertyValue_InContainer(this);
					auto Property = Cast<T>(Object);
					if (nullptr != Property)
					{
						FString PropertyNameToCompare = PropertyName + GetPropertyTagName<T>();
						if (Property->GetFName().ToString() == PropertyNameToCompare)
						{
							return Property;
						}
					}
				}
			}

			PropertyLink = PropertyLink->PropertyLinkNext;
		}

		return nullptr;
	}
protected:
	//@note: ������ �ȿ����� ����ϴ� �Լ��Դϴ�.
	void SetEnableAutoDestroy(bool bInEnableAutoDestroy);
public:
	bool IsEnabledAutoDestroy() const;
private:
	bool bEnableAutoDestroy;
};


UCLASS()
class UPBTimerWidget : public UPBUserWidget
{
	GENERATED_BODY()

public:
	UPBTimerWidget(const FObjectInitializer &Initializer);
	virtual void NativeConstruct() override;

private:
	void EndTimer();

public:
	UFUNCTION(BlueprintCallable, Category = "TimerWidget")
	void StartTimer();

	int64 GetTimeStamp() const;
	void SetTimeStamp(int64 InTimeStamp);
private:
	int64 TimeStamp;
	FTimerHandle DisposableTimerHandle;
public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FEndTimer, UPBTimerWidget *)
	FEndTimer OnEndTimer;

	// �ڵ����� Ÿ�̸Ӹ� �������� �ʴ´�. StartTimer�Լ��� ȣ���ϴ� ���� ���� ����� �����ϵ��� �Ѵ�. (�⺻��: true)
	UPROPERTY(EditDefaultsOnly, Category = "TimerWidget")
	bool bTrigManually;

	// ������ �ð��� ������ ��ϵ� ������ �� ���� ������ ������ �����Ѵ�. (0 ���Ϸ� �����Ǹ� ��Ȱ��ȭ�� ����. �⺻��: 5.0f)
	//@note: ���� �� ���� �ӽ÷� ��������� ���̵��� �ǵ��Ͽ� �����Ͽ�����, �� Ŭ���� ������ �� ���� �̿��Ͽ� �����ϴ� �۾��� ����.
	UPROPERTY(EditDefaultsOnly, Category = "TimerWidget")
	float LifeTimeInSeconds;
};


//@note: �ݺ� �ڵ带 ���̱� ���� �� ��ũ�θ� ����ϰ� ��. PS4 �����Ͻ� ȣȯ�� ���� �׽�Ʈ �ʿ�. �ϴ� GCC, Clang ���忡 �ִ��� ����.
//@note: �� ��ũ�δ� Widget Blueprint�� ������ ����Ƽ�� ������ ������ �� �ֱ� ���� ������, ���� �������Ʈ�� �����Ű�� �ʵ��� ������, �������� �ʾƾ���.
//@note: �������Ʈ������ ���̹��� �������� ������, CPP�� �����ϵ��� Property �̸��� �� ���ؾ��Ѵ�.
// PBGetterWidgetBlueprintVariable(Return Type, Variable Name In Blueprint), GetBPV_VariableNameInBP ������ �Լ��� ����� �� �ְ� �ȴ�. class Ű���� ��� ����!
#define PBGetterWidgetBlueprintVariable(ReturnType, BlueprintVariableName) private: class ReturnType *BPV_ ## BlueprintVariableName ## Cached = nullptr; public: class ReturnType *GetBPV_ ## BlueprintVariableName (bool bTryRefresh = false); protected: const FString BPV_WidgetName_ ## BlueprintVariableName = # BlueprintVariableName;
#define PBGetterWidgetBlueprintVariable_Implementation(ReturnType, NativeClassName, BlueprintVariableName) ReturnType *NativeClassName ## :: GetBPV_ ## BlueprintVariableName(bool bTryRefresh) { if (nullptr != BPV_ ## BlueprintVariableName ## Cached && bTryRefresh == false) return BPV_ ## BlueprintVariableName ## Cached; BPV_ ## BlueprintVariableName ## Cached = GetWidgetFromBP<ReturnType>(BPV_WidgetName_ ## BlueprintVariableName); if (nullptr == BPV_ ## BlueprintVariableName ## Cached) { 	ensureMsgf(nullptr != BPV_ ## BlueprintVariableName ## Cached, TEXT("Ooops!! " ## # BlueprintVariableName ## " couldn't find from Blueprint. Check out The Variable's Name correctly. Or you did access before initialize owner widget.")); UE_LOG(LogUI, Warning, TEXT("%s"), *(BPV_WidgetName_ ## BlueprintVariableName + TEXT(" has not cached from Blueprint side"))); } return BPV_ ## BlueprintVariableName ## Cached; }

// Blueprint Property
#define PBGetterWidgetBlueprintProperty(ReturnType, BlueprintPropertyName) private: class ReturnType *BPP_ ## BlueprintPropertyName ## Cached = nullptr; protected: const FString BPP_PropertyName ## BlueprintPropertyName = # BlueprintPropertyName; public: class ReturnType *GetBPP_ ## BlueprintPropertyName(bool bTryRefresh = false);
#define PBGetterWidgetBlueprintProperty_Implementation(ReturnType, NativeClassName, BlueprintPropertyName) ReturnType *NativeClassName :: GetBPP_ ## BlueprintPropertyName(bool bTryRefresh) { if (BPP_ ## BlueprintPropertyName ## Cached != nullptr && bTryRefresh == false) { return BPP_ ## BlueprintPropertyName ## Cached; } if (nullptr == (BPP_ ## BlueprintPropertyName ## Cached = GetPropertyFromBP<ReturnType>(# BlueprintPropertyName))) { ensureMsgf(nullptr != BPP_ ## BlueprintPropertyName ## Cached, TEXT("Oops!" ## # BlueprintPropertyName ## " couldn't find from the Property of class " ## # NativeClassName ## ". check out it!")); UE_LOG(LogUI, Warning, TEXT("%s%s"), TEXT(# BlueprintPropertyName), TEXT(" could not make a cache!")); } return BPP_ ## BlueprintPropertyName ## Cached; }
