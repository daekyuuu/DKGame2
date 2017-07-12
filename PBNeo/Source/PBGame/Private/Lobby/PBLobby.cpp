// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLobby.h"
#include "PBWidgetPage.h"
#include "PBMultiplayPage.h"
#include "PBWaitingRoomPage.h"
#include "PBOptionPage.h"
#include "PBLoadoutPage.h"
#include "PBGameInstance.h"
#include "PBLobbyTourGuide.h"
#include "PBCharacterScene.h"
#include "PBCharSelect_MainPage.h"
#include "PBCharSelect_LeftPage.h"
#include "PBCharSelect_RightPage.h"
#include "PBMainMenuPage.h"

const FName FLobbyCamName::MainMenu("MainMenu");
const FName FLobbyCamName::Multiplay("Multiplay");
const FName FLobbyCamName::Loadout("Loadout");
const FName FLobbyCamName::CharSelect_Main("CharSelect_Main");
const FName FLobbyCamName::CharSelect_Left("CharSelect_Left");
const FName FLobbyCamName::CharSelect_Right("CharSelect_Right");
const FName FLobbyCamName::LoadingScreen("LoadingScreen");
const FName FLobbyCamName::None("None");


UPBLobby::UPBLobby()
{
	AddNewPage(EPageType::MainMenu);
	AddNewPage(EPageType::Multiplay);
	AddNewPage(EPageType::WaitingRoom);
	AddNewPage(EPageType::Loadout);
	AddNewPage(EPageType::Option);
	AddNewPage(EPageType::CharSelect_Main);
	AddNewPage(EPageType::CharSelect_Left);
	AddNewPage(EPageType::CharSelect_Right);


	DefaultTourSpeed = 30.0f;
	InterpCurve = nullptr;
}

UPBWidgetPage* UPBLobby::CreatePageInstance(EPageType Type)
{
	switch (Type)
	{
	case EPageType::MainMenu:
		return NewObject<UPBMainMenuPage>();
	case EPageType::Multiplay:
		return NewObject<UPBMultiplayPage>();
	case EPageType::WaitingRoom:
		return NewObject<UPBWaitingRoomPage>();
	case EPageType::Loadout:
		return NewObject<UPBLoadoutPage>();
	case EPageType::Option:
		return NewObject<UPBOptionPage>();
	case EPageType::CharSelect_Main:
		return NewObject<UPBCharSelect_MainPage>();
	case EPageType::CharSelect_Left:
		return NewObject<UPBCharSelect_LeftPage>();
	case EPageType::CharSelect_Right:
		return NewObject<UPBCharSelect_RightPage>();
	}

	return nullptr;
}

void UPBLobby::TourCamFromTo(class UPBWidgetPage* CurrPage, class UPBWidgetPage* TargetPage)
{
	EPageType TargetPageType = EPageType::None, CurrPageType = EPageType::None;

	for (const FPageData& D : PageDatas)
	{
		if (TargetPage && (D.PageInstance == TargetPage))
		{
			TargetPageType = D.PageType;
		}
		else if (CurrPage && (D.PageInstance == CurrPage))
		{
			CurrPageType = D.PageType;
		}

	}

	if (TourGuide)
	{
		TourGuide->StartTourFromTo(FLobbyCamName::FromPageType(CurrPageType), FLobbyCamName::FromPageType(TargetPageType));
	}



}

void UPBLobby::TourCamFromTo(const FName& InCurrCamName, const FName& InTargetCamName)
{
	if (TourGuide)
	{
		TourGuide->StartTourFromTo(InCurrCamName, InTargetCamName);
	}
}

UWorld* UPBLobby::GetPBWorld()
{
	if (OwningPBGameInstance)
	{
		return OwningPBGameInstance->GetWorld();
	}

	return nullptr;
}

void UPBLobby::AddNewPage(EPageType Type)
{
	FPageData PageData(Type);
	PageDatas.Add(PageData);
}

void UPBLobby::InitCharacterScene(class APlayerController* OwningPlayer)
{
	if (nullptr == CharacterScene)
	{
		CharacterScene = NewObject<UPBCharacterScene>(this);
		if (CharacterScene)
		{
			CharacterScene->Init(OwningPlayer);
		}
	}

	else
	{
		CharacterScene->Init(OwningPlayer);
	}
}

void UPBLobby::InitPages(class APlayerController* OwningPlayer)
{
	for (FPageData& D : PageDatas)
	{
		if (!D.PageInstance)
		{
			D.PageInstance = CreatePageInstance(D.PageType);
		}

		if (D.PageInstance)
		{
			TSubclassOf<UPBMainWidget> HUDClass = D.HUDClass ? D.HUDClass : DefaultHUDClassForRelay;
			D.PageInstance->Init(OwningPlayer, HUDClass);
		}
	}
}

void UPBLobby::InitTourGuide(class APlayerController* OwningPlayer)
{
	if (nullptr == TourGuide)
	{
		TourGuide = NewObject<UPBLobbyTourGuide>(this);
	}

	if (TourGuide)
	{
		TourGuide->Init(OwningPlayer, this);
	}
}

void UPBLobby::Init(class UPBGameInstance* OwningGI, class APlayerController* OwningPlayer)
{
	Reset();

	if (nullptr == OwningGI)
	{
		return;
	}

	if (nullptr == OwningPlayer)
	{
		return;
	}

	OwningPBGameInstance = OwningGI;

	InitPages(OwningPlayer);

	InitTourGuide(OwningPlayer);

	InitCharacterScene(OwningPlayer);

	FInputModeUIOnly InputMode;
	OwningPlayer->SetInputMode(InputMode);

	// Set MainMenu as initial page
	PageStack.Empty();
	PushPage(GetPageInstance(EPageType::MainMenu));
}

void UPBLobby::Reset()
{
	PageStack.Empty();
	OwningPBGameInstance = nullptr;
	TourGuide = nullptr;
	CharacterScene = nullptr;
}

void UPBLobby::PushPage(UPBWidgetPage* NewPage)
{
	if (nullptr == GetPBWorld())
	{
		return;
	}

	if (nullptr == NewPage)
	{
		return;
	}

	if (GetCurrentPage() == NewPage)
	{
		return;
	}

	if (PageStack.Num() < 1)
	{
		PageStack.Push(NewPage);
		TourCamFromTo(nullptr, NewPage);
		NewPage->Enter();
	}
	else
	{
		if (PageStack.Last())
		{
			PageStack.Last()->Exit();
		}
		TourCamFromTo(PageStack.Last(), NewPage);
		PageStack.Push(NewPage);
		NewPage->Enter();
	}
}

void UPBLobby::PushPage(EPageType PageType)
{
	auto Page = GetPageInstance(PageType);
	if (Page)
	{
		PushPage(Page);
	}
}

UPBWidgetPage* UPBLobby::PopPage()
{
	if (nullptr == GetPBWorld())
	{
		return nullptr;
	}

	if (PageStack.Num() < 1)
	{
		return nullptr;
	}


	else if (PageStack.Num() == 1)
	{
		if (PageStack.Last())
		{
			PageStack.Last()->Exit();
		}

		return PageStack.Pop();

	}
	else
	{
		if (PageStack.Last())
		{
			PageStack.Last()->Exit();
		}

		auto PopedPage = PageStack.Pop();
		if (PageStack.Last())
		{
			TourCamFromTo(PopedPage, PageStack.Last());
			PageStack.Last()->Enter();
		}

		return PopedPage;
	}

	return nullptr;

}

class UPBWidgetPage* UPBLobby::GoToPage(EPageType PageType)
{
	auto TargetPage = GetPageInstance(PageType);
	if (nullptr == TargetPage)
	{
		return nullptr;
	}
	
	if (false == PageStack.Contains(TargetPage))
	{
		return nullptr;
	}

	while (PageStack.Num() > 0)
	{
		if (PageStack.Last() == TargetPage)
		{
			return PageStack.Last();
		}
		else
		{
			PopPage();
		}
	}

	return nullptr;

}

UPBWidgetPage* UPBLobby::GetPageInstance(EPageType InType) const
{
	for (const FPageData& D : PageDatas)
	{
		if (D.PageType == InType)
		{
			return D.PageInstance;
		}
	}

	return nullptr;
}

const FPageData* UPBLobby::GetPageData(EPageType InType) const
{
	for (const FPageData& D : PageDatas)
	{
		if (D.PageType == InType)
		{
			return &D;
		}
	}

	return nullptr;
}

UPBWidgetPage* UPBLobby::GetCurrentPage() const
{
	if (PageStack.Num() > 0)
	{
		return PageStack.Last();
	}
	
	return nullptr;
}

class UPBCharacterScene* UPBLobby::GetCharacterScene() const
{
	return CharacterScene;
}
