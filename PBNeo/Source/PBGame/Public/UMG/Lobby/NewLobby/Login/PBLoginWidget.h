// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBMainWidget.h"
#include "Online/PBNetClientInterface.h"
#include "PBLoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBLoginWidget : public UPBMainWidget
{
	GENERATED_BODY()
	
public :
	PBGetterWidgetBlueprintVariable(UEditableTextBox, InputID);
	PBGetterWidgetBlueprintVariable(UEditableTextBox, InputPassword);
	PBGetterWidgetBlueprintVariable(UButton, ButtonLogin);

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void BindEvents();
	void InitLoginData();

private:
	UFUNCTION()
	void HandleInputIDCommitted(const FText& InText, ETextCommit::Type InCommitType);

	UFUNCTION()
	void HandleInputPasswordCommitted(const FText& InText, ETextCommit::Type InCommitType);

	UFUNCTION()
	void HandleLoginButtonReleased();

	FDelegateHandle HandleClientConnect_Handle;
	void HandleClientConnect(EPBNetClientState PrevState, EPBNetClientState NewState);

	FDelegateHandle HandleClientLogin_Handle;
	void HandleClientLogin(EPBNetClientState PrevState, EPBNetClientState NewState);

	FDelegateHandle HandleGotUserData_Handle;
	void HandleGotUserData(bool NeedUpdateNickname);

	FString AccountID;
	FString Password;

	bool bLoginButtonLock;
};
