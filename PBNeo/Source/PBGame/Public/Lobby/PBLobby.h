// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "PBLobby.generated.h"

UENUM(BlueprintType)
enum class EPageType : uint8
{
	MainMenu,
	Multiplay,
	WaitingRoom,
	Loadout,
	Option,
	CharSelect_Main,
	CharSelect_Left,
	CharSelect_Right,
	None,
};


struct FLobbyCamName
{
	static FName const MainMenu;
	static FName const Multiplay;
	static FName const Loadout;
	static FName const CharSelect_Main;
	static FName const CharSelect_Left;
	static FName const CharSelect_Right;
	static FName const LoadingScreen;
	static FName const None;


	static FName FromPageType(EPageType Type)
	{
		FName PageName = None;

		switch (Type)
		{
		case EPageType::MainMenu:
			PageName = MainMenu;
			break;
		case EPageType::Multiplay:
			PageName = Multiplay;
			break;
		case EPageType::WaitingRoom:
			PageName = Multiplay;
			break;
		case EPageType::Loadout:
			PageName = Loadout;
			break;
		case EPageType::Option:
			PageName = MainMenu;
			break;
		case EPageType::CharSelect_Main:
			PageName = CharSelect_Main;
			break;
		case EPageType::CharSelect_Left:
			PageName = CharSelect_Left;
			break;
		case EPageType::CharSelect_Right:
			PageName = CharSelect_Right;
			break;
		case EPageType::None:
			break;
		default:
			break;
		}

		return PageName;
	}

};



/**
* it contains data to make page
*/
USTRUCT(BlueprintType)
struct FPageData 
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Classes")
	EPageType PageType;

	UPROPERTY(EditAnywhere, Category = "Classes")
	TSubclassOf<class UPBMainWidget> HUDClass;

	UPROPERTY()
	class UPBWidgetPage* PageInstance;

public:

	FPageData()
		:PageType(EPageType::MainMenu),
		HUDClass(nullptr),
		PageInstance(nullptr)
	{
	}

	FPageData(EPageType InPageType)
		:PageType(InPageType),
		HUDClass(nullptr),
		PageInstance(nullptr)
	{	
	}
};

/**
 *
 */
UCLASS(Blueprintable, ClassGroup = (Custom))
class PBGAME_API UPBLobby : public UObject
{
	GENERATED_BODY()

public:

	UPBLobby();

public:

	void Init(class UPBGameInstance* OwningGI, class APlayerController* OwningPlayer);

	void Reset();

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void PushPage(EPageType PageType);

	// pop the page and return it. and set the focus to the last element
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	class UPBWidgetPage* PopPage();

	/**
	* Go to the target page.
	* If there's the target page in the stack, then pop the page until to met target page.
	*/
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	class UPBWidgetPage* GoToPage(EPageType PageType);

	// If you want to get specific pageInstance, call it
	class UPBWidgetPage* GetPageInstance(EPageType InType) const;
	const FPageData* GetPageData(EPageType InType) const;
	class UPBWidgetPage* GetCurrentPage() const;
	class UPBCharacterScene* GetCharacterScene() const;

private:
	// push new page and set focus
	void PushPage(class UPBWidgetPage* Page);

	// It's only called in constructor
	void AddNewPage(EPageType Type);
	void InitCharacterScene(class APlayerController* OwningPlayer);
	void InitPages(class APlayerController* OwningPlayer);
	void InitTourGuide(class APlayerController* OwningPlayer);

	// page factory function
	static class UPBWidgetPage* CreatePageInstance(EPageType Type);

	// Move the cam to the page's preset location.
	void TourCamFromTo(class UPBWidgetPage* CurrPage, class UPBWidgetPage* TargetPage);
	
public:
	void TourCamFromTo(const FName& InCurrCamName, const FName& InTargetCamName);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "PageData")
	TArray<FPageData> PageDatas;

	// If the page has no 2d hud, then add this widget for focus relay
	UPROPERTY(EditAnywhere, Category = "Defaults")
	TSubclassOf<class UPBMainWidget> DefaultHUDClassForRelay;

public:

	// The curve to specify the speed of turn overing page. If it is null then the lobby will use the default settings.
	UPROPERTY(EditAnywhere, Category = "Effects")
		class UCurveFloat* InterpCurve;

	// If the TourSpeedCurve is null, then lobby will use this value to set the page turn speed.
	UPROPERTY(EditAnywhere, Category = "Effects")
		float DefaultTourSpeed;

private:
	UPROPERTY()
	TArray<class UPBWidgetPage*> PageStack;

	UPROPERTY()
	class UPBGameInstance* OwningPBGameInstance;

	UPROPERTY()
	class UPBLobbyTourGuide* TourGuide;

	UPROPERTY()
	class UPBCharacterScene* CharacterScene;

public:
	class UWorld* GetPBWorld();
};
