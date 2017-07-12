// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoginWidget.h"
#include "PBGameInstance.h"
#include "EditableTextBox.h"
#include "Online/PBNetClientInterface.h"
#include "PBGameUserSettings.h"

//Engine
#include "ContentWidget.h"
#include "Button.h"


PBGetterWidgetBlueprintVariable_Implementation(UEditableTextBox, UPBLoginWidget, InputID);
PBGetterWidgetBlueprintVariable_Implementation(UEditableTextBox, UPBLoginWidget, InputPassword);
PBGetterWidgetBlueprintVariable_Implementation(UButton, UPBLoginWidget, ButtonLogin);

void UPBLoginWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BindEvents();
	InitLoginData();

	bLoginButtonLock = false;
}

void UPBLoginWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UPBLoginWidget::BindEvents()
{
	GetBPV_InputID()->OnTextCommitted.AddUniqueDynamic(this, &UPBLoginWidget::HandleInputIDCommitted);
	GetBPV_InputPassword()->OnTextCommitted.AddUniqueDynamic(this, &UPBLoginWidget::HandleInputPasswordCommitted);
	GetBPV_ButtonLogin()->OnReleased.AddUniqueDynamic(this, &UPBLoginWidget::HandleLoginButtonReleased);
}

void UPBLoginWidget::InitLoginData()
{
	bool IsEmptyID = true;
	bool IsEmptyPW = true;

	UPBGameUserSettings* Settings = Cast<UPBGameUserSettings>(GEngine->GetGameUserSettings());

	if (!Settings->AccountID.IsEmpty())
	{
		AccountID = Settings->AccountID;
		GetBPV_InputID()->SetText(FText::FromString(AccountID));
		IsEmptyID = false;
	}

	if (!Settings->AccountPW.IsEmpty())
	{
		Password = Settings->AccountPW;
		GetBPV_InputPassword()->SetText(FText::FromString(Password));
		IsEmptyPW = false;
	}

	// ID, PW 정보가 없을 시 ID 입력란에 포커싱
	if (IsEmptyID || IsEmptyPW)
	{
		GetBPV_InputID()->SetKeyboardFocus();
	}
	// ID, PW 정보가 모두 있을 시 로그인 버튼에 포커싱
	else if (!IsEmptyID && !IsEmptyPW)
	{
		GetBPV_ButtonLogin()->SetKeyboardFocus();
	}
}

void UPBLoginWidget::HandleInputIDCommitted(const FText& InText, ETextCommit::Type InCommitType)
{
	AccountID = InText.ToString();
}

void UPBLoginWidget::HandleInputPasswordCommitted(const FText& InText, ETextCommit::Type InCommitType)
{
	Password = InText.ToString();
}

void UPBLoginWidget::HandleLoginButtonReleased()
{
	if (bLoginButtonLock)
		return;

	if (AccountID.IsEmpty() || Password.IsEmpty())
	{
		print("Please, Input AccountID / Password");
		return;
	}

	UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(this);
	ensure(ClientInterface);

	print("Connecting to server...");
	ClientInterface->ConnectToServer("192.168.2.21", 36100);
	HandleClientConnect_Handle = ClientInterface->OnStateChangedEvt.AddUObject(this, &UPBLoginWidget::HandleClientConnect);
	bLoginButtonLock = true;
	return;
}

void UPBLoginWidget::HandleClientConnect(EPBNetClientState PrevState, EPBNetClientState NewState)
{
	if (PrevState == EPBNetClientState::NotConnected && NewState == EPBNetClientState::Connected)
	{
		UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(this);
		ensure(ClientInterface);

		ClientInterface->OnStateChangedEvt.Remove(HandleClientConnect_Handle);

		print("Logging in...");

		// Login
		ClientInterface->Login(AccountID, Password);
		HandleClientLogin_Handle = ClientInterface->OnStateChangedEvt.AddUObject(this, &UPBLoginWidget::HandleClientLogin);
	}
	else
	{
		bLoginButtonLock = false;
	}
}

void UPBLoginWidget::HandleClientLogin(EPBNetClientState PrevState, EPBNetClientState NewState)
{
	if (PrevState == EPBNetClientState::Connected && NewState == EPBNetClientState::LoggedIn)
	{
		UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(this);
		ensure(ClientInterface);

		ClientInterface->OnStateChangedEvt.Remove(HandleClientLogin_Handle);

		print("Getting UserInfo...");
		ClientInterface->GetUserInfo();
		HandleGotUserData_Handle = ClientInterface->OnGotUserInfoEvt.AddUObject(this, &UPBLoginWidget::HandleGotUserData);
	}
	else
	{
		bLoginButtonLock = false;
	}
}

void UPBLoginWidget::HandleGotUserData(bool NeedUpdateNickname)
{
	UPBNetClientInterface* ClientInterface = UPBNetClientInterface::Get(this);
	ensure(ClientInterface);

	ClientInterface->OnStateChangedEvt.Remove(HandleGotUserData_Handle);

	if (NeedUpdateNickname)
	{
		// 계정 ID와 닉네임 동일
		ClientInterface->UpdateNickname(AccountID);
	}

	// ID, Password 를 DefaultGameUserSettings Config에 등록
	UPBGameUserSettings* Settings = Cast<UPBGameUserSettings>(GEngine->GetGameUserSettings());
	Settings->AccountID = AccountID;
	Settings->AccountPW = Password;
	Settings->SaveSettings();

	print("Enter game...");
	GetPBGameInstance()->GotoState(PBGameInstanceState::MainMenu);
	bLoginButtonLock = false;
}