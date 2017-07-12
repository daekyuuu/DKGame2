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
					//@note: 배열이 필요하다면 이곳에서...
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
	//@note: 생성자 안에서만 사용하는 함수입니다.
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

	// 자동으로 타이머를 시작하지 않는다. StartTimer함수를 호출하는 순간 부터 기능을 동작하도록 한다. (기본값: true)
	UPROPERTY(EditDefaultsOnly, Category = "TimerWidget")
	bool bTrigManually;

	// 지정된 시간이 지나면 등록된 위젯들 중 가장 오래된 위젯을 제거한다. (0 이하로 지정되면 비활성화를 뜻함. 기본값: 5.0f)
	//@note: 현재 이 값은 임시로 참고용으로 쓰이도록 의도하여 선언하였으며, 이 클래스 스스로 이 값을 이용하여 수행하는 작업은 없다.
	UPROPERTY(EditDefaultsOnly, Category = "TimerWidget")
	float LifeTimeInSeconds;
};


//@note: 반복 코드를 줄이기 위해 이 매크로를 사용하게 됨. PS4 컴파일시 호환성 여부 테스트 필요. 일단 GCC, Clang 스펙에 최대한 맞춤.
//@note: 이 매크로는 Widget Blueprint의 변수들 네이티브 구현시 참조할 수 있기 위한 것으로, 재차 블루프린트로 노출시키지 않도록 했으며, 노출하지 않아야함.
//@note: 블루프린트에서의 네이밍은 자유도가 높은데, CPP에 적합하도록 Property 이름을 잘 정해야한다.
// PBGetterWidgetBlueprintVariable(Return Type, Variable Name In Blueprint), GetBPV_VariableNameInBP 형태의 함수를 사용할 수 있게 된다. class 키워드 사용 금지!
#define PBGetterWidgetBlueprintVariable(ReturnType, BlueprintVariableName) private: class ReturnType *BPV_ ## BlueprintVariableName ## Cached = nullptr; public: class ReturnType *GetBPV_ ## BlueprintVariableName (bool bTryRefresh = false); protected: const FString BPV_WidgetName_ ## BlueprintVariableName = # BlueprintVariableName;
#define PBGetterWidgetBlueprintVariable_Implementation(ReturnType, NativeClassName, BlueprintVariableName) ReturnType *NativeClassName ## :: GetBPV_ ## BlueprintVariableName(bool bTryRefresh) { if (nullptr != BPV_ ## BlueprintVariableName ## Cached && bTryRefresh == false) return BPV_ ## BlueprintVariableName ## Cached; BPV_ ## BlueprintVariableName ## Cached = GetWidgetFromBP<ReturnType>(BPV_WidgetName_ ## BlueprintVariableName); if (nullptr == BPV_ ## BlueprintVariableName ## Cached) { 	ensureMsgf(nullptr != BPV_ ## BlueprintVariableName ## Cached, TEXT("Ooops!! " ## # BlueprintVariableName ## " couldn't find from Blueprint. Check out The Variable's Name correctly. Or you did access before initialize owner widget.")); UE_LOG(LogUI, Warning, TEXT("%s"), *(BPV_WidgetName_ ## BlueprintVariableName + TEXT(" has not cached from Blueprint side"))); } return BPV_ ## BlueprintVariableName ## Cached; }

// Blueprint Property
#define PBGetterWidgetBlueprintProperty(ReturnType, BlueprintPropertyName) private: class ReturnType *BPP_ ## BlueprintPropertyName ## Cached = nullptr; protected: const FString BPP_PropertyName ## BlueprintPropertyName = # BlueprintPropertyName; public: class ReturnType *GetBPP_ ## BlueprintPropertyName(bool bTryRefresh = false);
#define PBGetterWidgetBlueprintProperty_Implementation(ReturnType, NativeClassName, BlueprintPropertyName) ReturnType *NativeClassName :: GetBPP_ ## BlueprintPropertyName(bool bTryRefresh) { if (BPP_ ## BlueprintPropertyName ## Cached != nullptr && bTryRefresh == false) { return BPP_ ## BlueprintPropertyName ## Cached; } if (nullptr == (BPP_ ## BlueprintPropertyName ## Cached = GetPropertyFromBP<ReturnType>(# BlueprintPropertyName))) { ensureMsgf(nullptr != BPP_ ## BlueprintPropertyName ## Cached, TEXT("Oops!" ## # BlueprintPropertyName ## " couldn't find from the Property of class " ## # NativeClassName ## ". check out it!")); UE_LOG(LogUI, Warning, TEXT("%s%s"), TEXT(# BlueprintPropertyName), TEXT(" could not make a cache!")); } return BPP_ ## BlueprintPropertyName ## Cached; }
