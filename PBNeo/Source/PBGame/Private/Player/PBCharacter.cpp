// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PBGame.h"
#include "Player/PBPlayerState.h"
#include "Player/PBPlayerController.h"
#include "Player/PBCharacterTypes.h"
#include "Player/PBPlayerCameraManager.h"

#include "Online/PBUserInfo.h"
#include "Effects/PBImpactEffect.h"
#include "WidgetComponent.h"
#include "PBPlayerNameTag.h"
#include "Table/Item/PBItemTableManager.h"
#include "PBGameInstance.h"
#include "PBHUDWidget.h"
#include "FPBWidgetEventDispatcher.h"
#include "PBPersistentUser.h"
#include "PBBot.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundNodeLocalPlayer.h"
#include "FPBWidgetEventDispatcher.h"

#include "Weapons/Bullet/PBBulletBase.h"
#include "Weapons/Projectile/PBProj_Bomb.h"

#include "Weapons/PBWeaponTypes.h"
#include "Weapons/Attachment/PBWeapAttachment.h"
#include "Weapons/PBWeapon.h"
#include "Weapons/PBWeap_Throw.h"
#include "Weapons/PBDamageType.h"
#include "Weapons/PBWeap_Bomb.h"
#include "Weapons/PBWeap_DefuseKit.h"

#include "Mode/Mission/PBMission_BombMissionAttack.h"
#include "Mode/Mission/PBMission_BombMissionDefense.h"
#include "Mode/PBGameMode.h"
#include "Mode/PBMode_BombMission.h"
#include "Mode/Trigger/PBTrigger_BombSite.h"

#include "Kismet/KismetMathLibrary.h"
#include "PBActionPickup.h"
#include "Player/PBCheatManager.h"

const float DefaultRespawnDelay = 5.0f;

UPBSceneComponent::UPBSceneComponent()
{
	bAutoActivate = true;
	bWantsOnUpdateTransform = true;
	bWorldToComponentUpdated = true;
}

APBCharacter::APBCharacter(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPBCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON_PIERCE, ECR_Ignore);

	BaseEyeHeight = 64.f;
	CrouchedEyeHeight = 32.f;

	// Third Person Mesh
	GetMesh3P()->bOnlyOwnerSee = false;
	GetMesh3P()->bOwnerNoSee = true;
	GetMesh3P()->bReceivesDecals = false;
	GetMesh3P()->SetCollisionObjectType(ECC_Pawn);
	GetMesh3P()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh3P()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	GetMesh3P()->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	GetMesh3P()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh3P()->SetCollisionResponseToChannel(COLLISION_WEAPON_PIERCE, ECR_Overlap);
	//@note: 시작할 때 신체에 카메라가 파뭍여보이는 문제가 발생하지 않도록 우선 보이지 않도록 설정한다.
	GetMesh3P()->SetHiddenInGame(true, true);

	NameTagWidgetComp = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("NameTagWidget"));
	if (NameTagWidgetComp)
	{
		NameTagWidgetComp->SetupAttachment(GetCapsuleComponent());
		NameTagWidgetComp->Deactivate();
	}
	
	bIsZooming = false;
	bWantsToZooming = false;	
	bWantsToWalk = false;
	bWantsToSprint = false;
	bWantsToFireTrigger[0] = false;
	bWantsToFireTrigger[1] = false;
	bWantsToMeleeTrigger = false;
	bWantsToQuickThrowing = false;

	LowHealthPercentage = 0.5f;

	bIsAimingTargeted = false;

	InitLookingAround();
	InitUnBeatableSys();
	InitRecoilSystem();
	InitDetection();
	InitFirstPersonViewpoint();
	InitThirdPersonViewpoint();
	InitDebugCamViewpoint();
	InitCameraSys();
	InitComponentDamageSys();
	InitBackstabDamageSys();
	InitDeathImpulse();
	InitBombPlanting();
	InitBombDefusing();
	InitActionPickup();

	// First Person Mesh
	Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PawnMesh1P"));
	Mesh1P->SetupAttachment(FirstPersonViewScene);
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Mesh1P->SetCollisionObjectType(ECC_Pawn);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);

	//DebugComponent = ObjectInitializer.CreateDefaultSubobject<UPBDebugActorComponent>(this, TEXT("DebugComponent"));

	GetCharacterMovement()->MaxAcceleration = 2250.f;

	UseTableDataToConfig = true;
	DamagedSpeedModifier = 0.f;

	CurrentWeaponSlot = EWeaponSlot::Primary;
	CurrentWeaponSlotIdx = 0;
	CurrentWeaponMode = EWeaponMode::Mode1;
	CurrentQuickWeapon = nullptr;
	CurrentGrenadeSlotIdx = 0;

	CurrentSelectedQuickWeapSlot = EWeaponSlot::Grenade;

	bTriedBecomeSpectatorOnAlive = false;

	bShiftDebugViewInput = false;

	CurrentCameraStyle = EPBCameraStyle::None;
	bIsViewOwnself = true;

	FallOffDamageTypeClass = UPBFallOffDamageType::StaticClass();
	FallOffDamageSpeed = 1700.0f;
	FallOffDamageSlope = 0.051f;
	FallOffDamageMin = 5.0f;
	FallOffDeathTime = 5.0f;
	bIsFalling = false;
}

void APBCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// only to local owner: weapon change requests are locally instigated, other clients don't need it
	DOREPLIFETIME_CONDITION(APBCharacter, Inventory, COND_OwnerOnly);

	// everyone except local owner: flag change is locally instigated
	DOREPLIFETIME_CONDITION(APBCharacter, bIsZooming, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APBCharacter, bWantsToWalk, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APBCharacter, bWantsToSprint, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APBCharacter, CurrentQuickWeapon, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(APBCharacter, LastTakeHitInfo, COND_Custom);

	// everyone
	DOREPLIFETIME(APBCharacter, CurrentWeapon);
	DOREPLIFETIME(APBCharacter, Health);
	DOREPLIFETIME(APBCharacter, bUnbeatable);
	DOREPLIFETIME(APBCharacter, BombsitePresenceCount);
	
	DOREPLIFETIME(APBCharacter, SyncPlayAnimData);	

	DOREPLIFETIME_CONDITION(APBCharacter, bTriedBecomeSpectatorOnAlive, COND_OwnerOnly);

	DOREPLIFETIME(APBCharacter, CurrentCameraStyle);
	DOREPLIFETIME(APBCharacter, bIsViewOwnself);

	DOREPLIFETIME(APBCharacter, FirstPersonViewTransform);
	DOREPLIFETIME(APBCharacter, ThirdPersonViewTransform);

	DOREPLIFETIME(APBCharacter, TargetViewPlayer);
	DOREPLIFETIME(APBCharacter, AimingTargetCache);
}

void APBCharacter::BeginPlay()
{
	Super::BeginPlay();

	OverlappedBombSite = nullptr;
	BombPlantActionComponent = nullptr;
	BombDefuseActionComponent = nullptr;
	TargetViewPlayer = nullptr;
	AimingTargetCache = nullptr;

	//@note: 자신이 아닌경우 Mesh를 보이도록한다.
	if (false == IsLocallyControlled())
	{
		GetMesh3P()->SetHiddenInGame(false, true);
	}
}

void APBCharacter::BeginDestroy()
{
	Super::BeginDestroy();

	if (!GExitPurge)
	{
		const uint32 UniqueID = GetUniqueID();
		FAudioThread::RunCommandOnAudioThread([UniqueID]()
		{
			USoundNodeLocalPlayer::GetLocallyControlledActorCache().Remove(UniqueID);
		});
	}
}

void APBCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Role == ROLE_Authority)
	{
		Health = GetMaxHealth();
		SpawnDefaultInventory();
	}

	// create material instance for setting team colors (3rd person view)
	for (int32 iMat = 0; iMat < GetMesh3P()->GetNumMaterials(); iMat++)
	{
		MeshMIDs.Add(GetMesh3P()->CreateAndSetMaterialInstanceDynamic(iMat));
	}

	const bool bLocallyControlled = IsLocallyControlled();
	const uint32 UniqueID = GetUniqueID();
	FAudioThread::RunCommandOnAudioThread([UniqueID, bLocallyControlled]()
	{
		USoundNodeLocalPlayer::GetLocallyControlledActorCache().Add(UniqueID, bLocallyControlled);
	});

	if (GetNetMode() != NM_DedicatedServer)
	{
		InitVoiceSound();

		auto GI = Cast<UPBGameInstance>(UPBGameplayStatics::GetGameInstance(this));

		// play respawn effects
		if (RespawnFX)
		{
			UPBGameplayStatics::SpawnEmitterAtLocation(this, RespawnFX, GetActorLocation(), GetActorRotation());
		}	

		if (RespawnSound && GI && GI->IsCurrentState(PBGameInstanceState::Playing))
		{
			UPBGameplayStatics::PlaySoundAtLocation(this, RespawnSound, GetActorLocation());
		}
	}
	
	// Reset walk, crouched walk movement speed
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed * (1 + CharacterData.WalkSpeedModifier);
		GetCharacterMovement()->MaxWalkSpeedCrouched = GetCharacterMovement()->MaxWalkSpeed * (1 + CharacterData.CrouchedWalkSpeedModifier);
	}	

	StartUpDetection();
}

//@note: 캐릭터 메쉬의 show, hide 는 이 함수 안에서만 처리한다. 외부에서 SetHiddenInGame 를 호출하지 말것.
//또한 내부에서 사용되는 IsFirstPerson함수는 카메라의 모드(EPBCameraStyle)가 갱신된 이후에 사용되는게 좋으므로, 
//Enter_*_CamMode계열 함수가 호출된 이후에 이 함수를 호출하는 것이 좋다.
void APBCharacter::UpdateMeshVisibility(bool bForce)
{
	bool bIsFirstPersonView = GetViewerIsFirstPerson();

	GetMesh1P()->MeshComponentUpdateFlag = bIsFirstPersonView ? EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones : EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	GetMesh1P()->SetOwnerNoSee(!bIsFirstPersonView);

	GetMesh3P()->MeshComponentUpdateFlag = bIsFirstPersonView ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	GetMesh3P()->SetOwnerNoSee(bIsFirstPersonView);

	if (bIsFirstPersonView)
	{
		GetMesh1P()->SetHiddenInGame(!bForce, true);
		GetMesh3P()->SetHiddenInGame(bForce, true);
	}
	else
	{
		GetMesh1P()->SetHiddenInGame(bForce, true);
		GetMesh3P()->SetHiddenInGame(!bForce, true);
	}

	UpdateWeaponVisibility(bForce);
}

void APBCharacter::UpdateWeaponVisibility(bool bForce)
{
	if (GetCurrentWeapon())
	{
		GetCurrentWeapon()->UpdateMeshVisibilitys(bForce);
	}
}

void APBCharacter::ForceMeshVisibilityWithWeapon(bool bVisible, bool bFirstPerson/*=false*/)
{
	USkeletalMeshComponent* PawnMesh = GetSpecificPawnMesh(bFirstPerson);
	if (PawnMesh)
	{
		PawnMesh->SetHiddenInGame(!bVisible);
	}

	if (GetCurrentWeapon())
	{
		GetCurrentWeapon()->ForceMeshVisibilitys(bVisible, bFirstPerson);
	}
}


bool APBCharacter::GetUseTableTata()
{
	return UseTableDataToConfig && GetItemID() > 0;
}

bool APBCharacter::ApplyTableData(const struct FPBCharacterTableData* Data)
{
	if (!GetUseTableTata())
	{
		return false;
	}		

	CharacterData.Gender = EPBCharacterGender(Data->GenderEnum);

	//Todo

	return true;
}


struct FPBCharacterTableData* APBCharacter::GetTableData()
{
	if (RefTableData)
	{
		return RefTableData;
	}

	auto TM = UPBItemTableManager::Get(this);
	RefTableData = TM ? TM->GetCharacterTableData(GetItemID()) : nullptr;
	return RefTableData;
}

void APBCharacter::Destroyed()
{
	Super::Destroyed();
	DestroyInventory();

	if (NameTagWidgetComp)
	{
		NameTagWidgetComp->Deactivate();
		NameTagWidgetComp->DestroyComponent();
	}

	ShutDownLookAround();
	ShutDownDetection();
}

void APBCharacter::InitStartUpCamera()
{
	if (IsLocallyControlled())
	{
		StartUpLookAround();

		if (FirstPersonViewScene)
		{
			FirstPersonViewScene->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight), false);
		}

		EnterFPSCamMode(this);
	}
}

// Called only Localplayer
void APBCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();	

	// set team colors for 1st person view
	UMaterialInstanceDynamic* Mesh1PMID = GetMesh1P()->CreateAndSetMaterialInstanceDynamic(0);
	UpdateTeamColors(Mesh1PMID);

	// reattach weapon if needed
	SetCurrentWeapon(CurrentWeapon);
	if (CurrentWeapon)
	{
		CurrentWeapon->FillAmmo();
	}

	if (IsLocallyControlled())
	{
		APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
		if (MyPC && MyPC->WidgetEventDispatcher.IsValid())
		{
			MyPC->WidgetEventDispatcher->OnSpawn.Broadcast(Cast<APBPlayerState>(MyPC->PlayerState));
		}
	}

	InitStartUpCamera();
}

// Called only Server ( GameMode::StartNewPlayer -> RestartPlayer -> PossessedBy )
void APBCharacter::PossessedBy(class AController* InController)
{
	Super::PossessedBy(InController);

	// [server] as soon as PlayerState is assigned, set team colors of this pawn for local player
	UpdateTeamColorsAllMIDs();

	AdjustLookAroundInputSetting(Cast<APlayerController>(InController));

	SetCharacterPtrToNameTag();

	ForgetWhoKilledMe();

	// Spawn PBLocalPlayer's Weapons
	SpawnUserInventory(InController);
}

// other player info
void APBCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// [client] as soon as PlayerState is assigned, set team colors of this pawn for local player
	if (PlayerState != NULL)
	{
		UpdateTeamColorsAllMIDs();
	}

	SetCharacterPtrToNameTag();
}

FRotator APBCharacter::Get1PAimOffset() const
{
	return (GetBaseAimRotation().Quaternion() * GetViewRotation().Quaternion().Inverse()).Rotator();
}

FRotator APBCharacter::Get3PAimOffset() const
{
	if (IsCameraStyle(EPBCameraStyle::TPS) && IsViewingOwnself())
	{
		return GetBaseAimRotation();
	}

	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}

bool APBCharacter::IsEnemyFor(AController* TestPC) const
{
	if (TestPC == Controller || TestPC == NULL)
	{
		return false;
	}

	APBPlayerState* TestPlayerState = Cast<APBPlayerState>(TestPC->PlayerState);
	APBPlayerState* MyPlayerState = Cast<APBPlayerState>(PlayerState);

	bool bIsEnemy = true;
	APBGameState* GS = GetWorld() ? GetWorld()->GetGameState<APBGameState>() : nullptr;
	if (GS && GS->GameModeClass)
	{
		const APBGameMode* GM = GS->GameModeClass->GetDefaultObject<APBGameMode>();
		if (GM && MyPlayerState && TestPlayerState)
		{
			bIsEnemy = GM->CanDealDamage(TestPlayerState, MyPlayerState);
		}
	}

	return bIsEnemy;
}


bool APBCharacter::IsBot()
{
	return !IsPlayerControlled();
}

//////////////////////////////////////////////////////////////////////////
// Meshes

void APBCharacter::UpdateTeamColors(UMaterialInstanceDynamic* UseMID)
{
	if (UseMID)
	{
		APBPlayerState* MyPlayerState = Cast<APBPlayerState>(PlayerState);
		if (MyPlayerState != NULL)
		{
			float MaterialParam = (float)MyPlayerState->GetTeamType();
			UseMID->SetScalarParameterValue(TEXT("Team Color Index"), MaterialParam);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Damage & death

void APBCharacter::Suicide()
{
	KilledBy(this);
}

void APBCharacter::KilledBy(APawn* EventInstigator)
{
	if (Role == ROLE_Authority && !bIsDying)
	{
		AController* Killer = NULL;
		if (EventInstigator != NULL)
		{
			Killer = EventInstigator->Controller;
			LastHitBy = NULL;
		}

		Die(Health, FDamageEvent(UDamageType::StaticClass()), Killer, NULL);
	}
}

void APBCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	Die(Health, FDamageEvent(dmgType.GetClass()), NULL, NULL);
}

float APBCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC && MyPC->HasGodMode())
	{
		return 0.f;
	}

	if (Health <= 0.f)
	{
		return 0.f;
	}

	float ActualDamage = Damage;

	//@note: 현재 추락 데미지 계산은 땅에 닿는 타이밍(APBCharacter::Landed)에 별도로 하고 있다.
	if (false == DamageEvent.DamageTypeClass->IsChildOf(UPBFallOffDamageType::StaticClass()))
	{
		ActualDamage = ModifyDamageByHitComp(ActualDamage, DamageEvent, EventInstigator);
		ActualDamage = ModifyDamageForBackstab(ActualDamage, DamageEvent, EventInstigator);

		// Modify based on game rules.
		APBGameMode* const Game = GetWorld() ? GetWorld()->GetAuthGameMode<APBGameMode>() : nullptr;
		ActualDamage = Game ? Game->ModifyDamage(ActualDamage, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;

		ActualDamage = Super::TakeDamage(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
	}

	OnApplyDamage(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
	
	return ActualDamage;
}

void APBCharacter::OnApplyDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	// seungyoon.ji
	// NOTE: Intended by game design reason.
	Damage = FMath::CeilToInt(Damage);

	if (Damage > 0.f)
	{
		// 무적모드 : hit (O), damage(X)
		if (!HasUnbeatable())
		{
			Health -= Damage;
		}
		else
		{
			Damage = 0.f;
		}

		if (Health <= 0)
		{
			Die(Damage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			PlayHit(Damage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
		}

		MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);
	}
}

bool APBCharacter::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const
{
	if ( bIsDying										// already dying
		|| IsPendingKill()								// already destroyed
		|| Role != ROLE_Authority						// not authority
		|| GetWorld() && GetWorld()->GetAuthGameMode<AGameMode>() == nullptr
		|| GetWorld() && GetWorld()->GetAuthGameMode<AGameMode>()->GetMatchState() == MatchState::LeavingMap)	// level transition occurring
	{
		return false;
	}

	return true;
}

bool APBCharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
	{
		return false;
	}

	Health = FMath::Min(0.0f, Health);

	// if this is an environmental death then refer to the previous killer so that they receive credit (knocked into lava pits, etc)
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());

	FTakeDamageInfo Info;
	Info.ActualDamage = KillingDamage;
	Info.bIsHeadShot = IsHeadShot(DamageEvent);
	Info.bFallOff = IsFallOff(DamageEvent);
	APBBulletBase* Bullet = Cast<APBBulletBase>(DamageCauser);
	Info.ItemId = Bullet ? Bullet->WeaponId : 0;
	Info.Victim = this;

	APBGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<APBGameMode>() : nullptr;
	if (GM)
	{
		GM->Killed(Killer, KilledPlayer, DamageType, DamageCauser, Info);

		if (Info.bFallOff)
		{
			GM->UpdateKillMark(Killer, { KilledPlayer }, this, { Info });
		}
	}	

	NetUpdateFrequency = GetDefault<APBCharacter>()->NetUpdateFrequency;
	GetCharacterMovement()->ForceReplicationUpdate();

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);

	// Show HitEffect, Death by Fall Off
	auto PC = Cast<APBPlayerController>(GetController());
	if (DamageEvent.DamageTypeClass->IsChildOf(UPBFallOffDamageType::StaticClass()) && PC)
	{
		PlayFeedbackEffects(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);
	}

	return true;
}

void APBCharacter::OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	if (bIsDying)
	{
		return;
	}
	bIsDying = true;

	if (Role == ROLE_Authority)
	{
		ReplicateHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);

		APBPlayerController* PC = Cast<APBPlayerController>(GetController());
		if (PC)
		{
			bReplicateMovement = false;
		}
	}

	SpawnHitCrosshair(PawnInstigator, true);
	
	UPBDamageType *DamageType = Cast<UPBDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
	if (DamageType)
	{
		if (DamageType->KilledForceFeedback && Role == ROLE_Authority)
		{
			APlayerController* PC = Cast<APlayerController>(Controller);
			if (PC)
			{
				PC->ClientPlayForceFeedback(DamageType->KilledForceFeedback, false, "Damage");
			}
		}
	}
	
	if (GetNetMode() != NM_DedicatedServer)
	{
		if (DeathSound && GetMesh1P() && GetMesh1P()->IsVisible())
			UPBGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());

		PlayVoice(EPBVoiceType::Death);
	}

	// spawn the pickupWeapon
	if (PickupAction && Role == ROLE_Authority)
	{
		PickupAction->SpawnPickupWeapon(GetWeapon(EWeaponSlot::Primary));
	}

	// remove all weapons
	DestroyInventory();
	
	StopAllAnimMontages();

	if (LowHealthWarningPlayer && LowHealthWarningPlayer->IsPlaying())
	{
		LowHealthWarningPlayer->Stop();
	}

	if (RunLoopAC)
	{
		RunLoopAC->Stop();
	}

	// Death anim
	float DeathAnimDuration = PlayAnimMontage(DeathAnim);

	if (IsLocallyControlled())
	{
		EnterRagdollCamMode(this);
	}

	// Rag-doll
	if (DeathAnimDuration > 0.f)
	{
		// Use a local timer handle as we don't need to store it for later but we don't need to look for something to clear
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &APBCharacter::BecomeRagDoll, FMath::Min(0.1f, DeathAnimDuration), false);
	}
	else
	{
		BecomeRagDoll();
	}

	// disable collisions on capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	UPBPlayerNameTag* NameTag = GetNameTagWidget();
	if (NameTag)
	{
		NameTag->Hide();
	}
}

void APBCharacter::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);	

	if (Role == ROLE_Authority)
	{
		ReplicateHit(DamageTaken, DamageEvent, PawnInstigator, DamageCauser, false);
	}

	PlayFeedbackEffects(DamageTaken, DamageEvent, PawnInstigator, DamageCauser);

	//with blueprint event (at hit animation.)
	PlayHitEffects();
}

void APBCharacter::PlayFeedbackEffects(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);

	if (Role == ROLE_Authority)
	{
		// play the force feedback effect on the client player controller 
		if (MyPC)
		{
			UPBDamageType *DamageType = DamageEvent.DamageTypeClass ? Cast<UPBDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject()) : nullptr;

			// GamePad vibration
			if (DamageType && DamageType->HitForceFeedback)
			{
				MyPC->ClientPlayForceFeedback(DamageType->HitForceFeedback, false, "Damage");
			}

			// Camera shake
			if (DamageType && DamageType->HitCameraShake)
			{
				float CameraShakingScale = 1.0f;
				if (FallOffCameraShakingScaleCurve)
				{
					CameraShakingScale = FallOffCameraShakingScaleCurve->GetFloatValue(DamageTaken);

					if (UPBCheatManager::EnabledDebugFallOffDamage)
					{
						FColor MsgColor = (FLinearColor(FColor::Green) * 0.5f).ToFColor(true);
						GEngine->AddOnScreenDebugMessage(-1, 10.0f, MsgColor
							, FString::Printf(TEXT("[DebugFallOffDamage] DamageTaken: %d, CameraShakingScale: %0.2f")
								, DamageTaken, CameraShakingScale));
					}

				}

				MyPC->ClientPlayCameraShake(DamageType->HitCameraShake, CameraShakingScale);
			}
		}
	}

	bool bIsFallenOff = DamageEvent.DamageTypeClass->IsChildOf(UPBFallOffDamageType::StaticClass());
	if (false == bIsFallenOff)
	{
		SpawnHitCrosshair(PawnInstigator, false);
	}

	if (MyPC)
	{
		if (PawnInstigator)
		{
			MyPC->WidgetEventDispatcher->OnHit.Broadcast(DamageTaken, PawnInstigator->GetActorLocation(), bIsFallenOff);
		}
	}
}

void APBCharacter::PlayHitEffects_Implementation()
{
	// PlayVoice(EPBVoiceType::Damage); // Anim Event notify 에서 처리 해야함 
}

void APBCharacter::SpawnHitCrosshair(class APawn* EventInstigator, bool bIsKilledByHit)
{
	if (EventInstigator)
	{
		// spawn hit crosshair on instigator's hud
		APBCharacter* InstigatorCharacter = Cast<APBCharacter>(EventInstigator);
		if (InstigatorCharacter && InstigatorCharacter->IsLocallyControlled())
		{
			InstigatorCharacter->SpawnHitcrosshair(bIsKilledByHit);
		}
	}
	
}

void APBCharacter::SpawnHitcrosshair(bool bIsKilledByHit)
{
	auto PBController = Cast<APBPlayerController>(GetController());
	if (IsSafe(PBController))
	{
		if (PBController->WidgetEventDispatcher.IsValid())
		{
			PBController->WidgetEventDispatcher->OnHitSuccess.Broadcast(bIsKilledByHit);
		}
	}
}

void APBCharacter::PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	// Only replicate this property for a short duration after it changes so join in progress players don't get spammed with fx when joining late
	DOREPLIFETIME_ACTIVE_OVERRIDE(APBCharacter, LastTakeHitInfo, GetWorld() && GetWorld()->GetTimeSeconds() < LastTakeHitTimeTimeout);
	DOREPLIFETIME_ACTIVE_OVERRIDE(APBCharacter, SyncPlayAnimData, SyncPlayAnimData.RepCounter > 0);
}

void APBCharacter::ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser, bool bKilled)
{
	const float TimeoutTime = GetWorld() ? GetWorld()->GetTimeSeconds() + 0.5f : 0.f;

	FDamageEvent const& LastDamageEvent = LastTakeHitInfo.GetDamageEvent();
	if ((PawnInstigator == LastTakeHitInfo.PawnInstigator.Get()) && (LastDamageEvent.DamageTypeClass == LastTakeHitInfo.DamageTypeClass) && (LastTakeHitTimeTimeout == TimeoutTime))
	{
		// same frame damage
		if (bKilled && LastTakeHitInfo.bKilled)
		{
			// Redundant death take hit, just ignore it
			return;
		}

		// otherwise, accumulate damage done this frame
		Damage += LastTakeHitInfo.ActualDamage;
	}

	LastTakeHitInfo.ActualDamage = Damage;
	LastTakeHitInfo.PawnInstigator = Cast<APBCharacter>(PawnInstigator);
	LastTakeHitInfo.DamageCauser = DamageCauser;
	LastTakeHitInfo.SetDamageEvent(DamageEvent);
	LastTakeHitInfo.bKilled = bKilled;
	LastTakeHitInfo.EnsureReplication();

	LastTakeHitTimeTimeout = TimeoutTime;
}

void APBCharacter::OnRep_LastTakeHitInfo()
{
	if (LastTakeHitInfo.bKilled)
	{
		OnDeath(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
	else
	{
		PlayHit(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
}

void APBCharacter::BecomeRagDoll()
{
	// No point rotating our capsule now
	bUseControllerRotationYaw = false;

	// This is the fixed amount of time we will be rag-doll before spirit leaves body
	const float RagDollPeriod = 2.0;

	if (GetMesh3P())
	{
		GetMesh3P()->SetCollisionObjectType(COLLISION_RAGDOLL);
		GetMesh3P()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh3P()->SetCollisionResponseToAllChannels(ECR_Ignore);
		GetMesh3P()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		GetMesh3P()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		GetMesh3P()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		GetMesh3P()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
		GetMesh3P()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	}
	SetActorEnableCollision(true);

	GetMesh3P()->SetIsReplicated(false);
	GetMesh3P()->SetAllBodiesSimulatePhysics(true);
	GetMesh3P()->SetSimulatePhysics(true);
	GetMesh3P()->WakeAllRigidBodies();
	GetMesh3P()->bBlendPhysics = true;

	UPBDamageType* DamageType = Cast<UPBDamageType>(LastTakeHitInfo.GetDamageEvent().DamageTypeClass->GetDefaultObject());
	APawn* PawnInstigator = LastTakeHitInfo.PawnInstigator.Get();

	FVector DeathImpulse = FVector::ZeroVector;
	if (DamageType && PawnInstigator)
	{
		if (DamageType->KilledForceFeedback && Role == ROLE_Authority)
		{
			APlayerController* PC = Cast<APlayerController>(Controller);
			if (PC)
			{
				PC->ClientPlayForceFeedback(DamageType->KilledForceFeedback, false, "Damage");
			}
		}
	}

	ProcessDeathRagdollImpulse();

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &APBCharacter::SpiritLeaveBody, RagDollPeriod, false);
}

void APBCharacter::BecomeSpectatorOnAlive()
{
	auto GS = GetWorld() ? GetWorld()->GetGameState<APBGameState>() : nullptr;
	if (nullptr == GS)
		return;

	auto PS = Cast<APBPlayerState>(PlayerState);
	if (nullptr == PS)
		return;

	//// it could not be a spectator, TeamPlayers only you one person or have no player.
	//EPBTeamType TeamType = PS->GetTeamType();
	//auto TeamPlayers = GS->GetTeamPlayers((int32)(TeamType));
	//int32 TeamPlayerCount = TeamPlayers.Num();
	//if (TeamPlayerCount <= 1)
	//	return;

	if (CurrentWeapon)
	{
		CurrentWeapon->SetActorEnableCollision(false);
		CurrentWeapon->SetActorHiddenInGame(true);
	}

	auto SkelMesh = GetMesh3P();
	if (SkelMesh)
	{
		SkelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	// remove all weapons
	DestroyInventory();

	StopAllAnimMontages();

	BecomeFollower();

	UpdateMeshVisibility(false);
}

bool APBCharacter::ServerBecomeSpectatorOnAlive_Validate()
{
	return true;
}

void APBCharacter::ServerBecomeSpectatorOnAlive_Implementation()
{
	if (false == HasAuthority())
		return;

	bTriedBecomeSpectatorOnAlive = true;
	OnRep_bTriedBecomeSpectatorOnAlive();
}

void APBCharacter::OnRep_bTriedBecomeSpectatorOnAlive()
{
	if (IsLocallyControlled() && bTriedBecomeSpectatorOnAlive)
	{
		BecomeSpectatorOnAlive();
	}
}

void APBCharacter::SpiritLeaveBody()
{
	auto World = GetWorld();
	if (nullptr == World)
		return;

	if (IsLocallyControlled())
	{
		if (LastTakeHitInfo.PawnInstigator.IsValid())
		{
			EnterDeathCamMode(this, LastTakeHitInfo.PawnInstigator.Get());
		}
		else
		{
			EnterDeathCamMode(this, nullptr);
		}
	}

	AGameStateBase const* const GameState = World->GetGameState();
	float RespawnDelay = (GameState ? GameState->GetPlayerRespawnDelay(Controller) : DefaultRespawnDelay) * 0.5f;

	APBMission* Mission = GetOwnMission();
	bool bIsRespawnableMission = Mission && Mission->bPlayerCanRespawn;
	FTimerHandle Handle_RestartPlayerCharacter;
	GetWorldTimerManager().SetTimer(Handle_RestartPlayerCharacter, this
		, bIsRespawnableMission ? &APBCharacter::Respawn : &APBCharacter::BecomeFollower
		, RespawnDelay, false);
}

void APBCharacter::BecomeFollower()
{
	if (IsLocallyControlled())
	{
		SetSpectator(true);

		AGameStateBase const* const GameState = GetWorld() ? GetWorld()->GetGameState() : nullptr;
		float RespawnDelay = (GameState ? GameState->GetPlayerRespawnDelay(Controller) : DefaultRespawnDelay) * 0.5f;
		FollowNextPlayer(RespawnDelay);

		APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
		if (MyPC && MyPC->WidgetEventDispatcher.IsValid())
		{
			MyPC->WidgetEventDispatcher->OnUpdateFollowCamera.Broadcast(TargetViewPlayer, true, true);
		}
	}
}

void APBCharacter::OnFollowPreviousPlayer()
{
	if ((IsSpectator() || !IsAlive()) && TargetViewPlayer)
	{
		FollowPlayer(false, 0.0f);
	}
}

void APBCharacter::OnFollowNextPlayer()
{
	if ((IsSpectator() || !IsAlive()) && TargetViewPlayer)
	{
		FollowPlayer(true, 0.0f);
	}
}

void APBCharacter::UpdateFollowTargetOnSpectatorMode()
{
	auto &TM = GetWorldTimerManager();
	if (IsSpectator()
		&& TargetViewPlayer
		&& TargetViewPlayer->OwnerCharacter
		&& false == TM.IsTimerActive(UpdateFollowerTimerHandle))
	{
		if (false == TargetViewPlayer->OwnerCharacter->IsAlive())
		{
			AGameStateBase const* const GameState = GetWorld() ? GetWorld()->GetGameState() : nullptr;
			float RespawnDelay = GameState ? GameState->GetPlayerRespawnDelay(Controller) * 0.5f : 2.5f;
			FollowNextPlayer(RespawnDelay);
		}
	}
}

void APBCharacter::FollowPlayer(bool bTryToGetNextPlayer, float InDelay)
{
	APBGameState* GS = GetWorld() ? Cast<APBGameState>(GetWorld()->GetGameState()) : nullptr;
	if (nullptr == GS || false == IsLocallyControlled())
		return;

	if (GetWorldTimerManager().IsTimerActive(UpdateFollowerTimerHandle))
		return;

	if (bTryToGetNextPlayer)
	{
		CandidatedFollowPlayer = DecideNextPlayerToFollow();
	}
	else
	{
		CandidatedFollowPlayer = DecidePreviousPlayerToFollow();
	}

	if (nullptr == CandidatedFollowPlayer || TargetViewPlayer == CandidatedFollowPlayer)
		return;

	if (IsSpectator() && TargetViewPlayer && TargetViewPlayer->OwnerCharacter && InDelay > 0.5f)
	{
		auto LastHitInstigator = TargetViewPlayer->OwnerCharacter->LastTakeHitInfo.PawnInstigator.Get();
		EnterRagdollCamMode(TargetViewPlayer);
		TargetViewPlayer->OwnerCharacter->GetMesh3P()->SetHiddenInGame(true);
	}

	auto FO_UpdateFollowTarget = [&]()
	{
		TargetViewPlayer = CandidatedFollowPlayer;

		APBCharacter *FollowingTarget = TargetViewPlayer ? TargetViewPlayer->OwnerCharacter : nullptr;
		//if (false != ensure(nullptr != FollowingTarget))
		if (nullptr == FollowingTarget)
			return;
		
		EnterTPSCamMode(FollowingTarget);

		//@note: TargetViewPlayer가 nullptr인 경우는 없어야한다.
		APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
		if (MyPC && MyPC->WidgetEventDispatcher.IsValid())
		{
			MyPC->WidgetEventDispatcher->OnUpdateFollowCamera.Broadcast(TargetViewPlayer, bTryToGetNextPlayer, true);
		}
	};

	if (InDelay <= 0)
	{
		FO_UpdateFollowTarget();
	}
	else
	{
		//@note: TargetViewPlayer가 nullptr이지만, 우선 GUI를 표시하기 위하여 Broadcast한다.
		APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
		if (MyPC && MyPC->WidgetEventDispatcher.IsValid())
		{
			MyPC->WidgetEventDispatcher->OnUpdateFollowCamera.Broadcast(CandidatedFollowPlayer, bTryToGetNextPlayer, false);
		}

		GetWorldTimerManager().SetTimer(UpdateFollowerTimerHandle, FO_UpdateFollowTarget, InDelay, false);
	}
}

void APBCharacter::FollowPreviousPlayer(float InDelay)
{
	FollowPlayer(false, InDelay);
}

void APBCharacter::FollowNextPlayer(float InDelay)
{
	FollowPlayer(true, InDelay);
}

APBPlayerState *APBCharacter::GetTargetViewPlayer() const
{
	return TargetViewPlayer;
}

void GetCandidatedArrayOfPlayerStateToFollow(const APBPlayerState *InOwnPlayerState, const APBGameState *InGameState, TArray<APBPlayerState *> &OutTeamPlayers, TArray<APBPlayerState *> &OutEnemyPlayers)
{
	if (!InGameState) return;

	if (!InOwnPlayerState) return;

	EPBTeamType OwnPlayerTeam = InOwnPlayerState->GetTeamType();
	TArray<APlayerState*> TeamPlayers = InGameState->PlayerArray;
	for (int32 i = 0; i < TeamPlayers.Num(); ++i)
	{
		APBPlayerState* CurPS = Cast<APBPlayerState>(TeamPlayers[i]);
		if (CurPS && CurPS != InOwnPlayerState)
		{
			auto CurOC = CurPS->OwnerCharacter;
			if (CurOC && CurOC->IsAlive() && false == CurOC->IsSpectator())
			{
				if (CurPS->GetTeamType() == OwnPlayerTeam)
				{
					OutTeamPlayers.Add(CurPS);
				}
				else
				{
					OutEnemyPlayers.Add(CurPS);
				}
			}
		}
	}
}

APBPlayerState *GetTargetViewPlayer(const TArray<APBPlayerState *> &InPlayers, APBPlayerState *const &CurrentTargetViewPlayer, const std::function<int(int CurrentTargetPlayerIndex)> &InceaserForNextPlayerIndex)
{
	bool bAnyAlivePlayers = InPlayers.Num() >= 1;
	if (bAnyAlivePlayers)
	{
		int32 CurrentTargetPlayerIndex = InPlayers.Find(CurrentTargetViewPlayer);
		if (INDEX_NONE == CurrentTargetPlayerIndex)
		{
			return InPlayers[0];
		}
		else
		{
			if (InceaserForNextPlayerIndex)
			{
				CurrentTargetPlayerIndex = InceaserForNextPlayerIndex(CurrentTargetPlayerIndex);
			}

			if (CurrentTargetPlayerIndex < 0)
			{
				CurrentTargetPlayerIndex = InPlayers.Num() - 1;
			}
			else if (CurrentTargetPlayerIndex >= InPlayers.Num())
			{
				CurrentTargetPlayerIndex = 0;
			}

			return InPlayers[CurrentTargetPlayerIndex];
		}
	}

	return nullptr;
}

APBPlayerState *GetNextTargetViewPlayer(const TArray<APBPlayerState *> &InPlayers, APBPlayerState *const &CurrentTargetViewPlayer)
{ return GetTargetViewPlayer(InPlayers, CurrentTargetViewPlayer, [](int InIndex) { return InIndex + 1; }); }

APBPlayerState *GetPreviousTargetViewPlayer(const TArray<APBPlayerState *> &InPlayers, APBPlayerState *const &CurrentTargetViewPlayer)
{ return GetTargetViewPlayer(InPlayers, CurrentTargetViewPlayer, [](int InIndex) { return InIndex - 1; }); }

APBPlayerState *APBCharacter::GetPlayerStateToFollow(APBPlayerState *(*InFunctionAsIncreaser)(const TArray<APBPlayerState *> &, APBPlayerState *const &))
{
	if (false == ensure(InFunctionAsIncreaser))
		return nullptr;

	UWorld *World = GetWorld();
	if (!World) return nullptr;

	APBPlayerState* PS = Cast<APBPlayerState>(PlayerState);
	if (!PS) return nullptr;

	APBGameState* GS = Cast<APBGameState>(World->GetGameState());
	if (!GS) return nullptr;

	TArray<APBPlayerState*> TeamPlayers;
	TArray<APBPlayerState*> EnemyPlayers;
	GetCandidatedArrayOfPlayerStateToFollow(PS, GS, TeamPlayers, EnemyPlayers);

	auto NextTargetViewPlayer = InFunctionAsIncreaser(TeamPlayers, TargetViewPlayer);
	if (nullptr == NextTargetViewPlayer)
	{
		NextTargetViewPlayer = InFunctionAsIncreaser(EnemyPlayers, TargetViewPlayer);
		if (false == ensureMsgf(NextTargetViewPlayer, TEXT(R"(Didn't get a Player to follow. has no valid players!)")))
		{
			//@todo: 아군과 적군 전체를 통틀어 유효한 플레이어를 얻어낼 수 없었을 때의 처리... (없어야 하는, 있을 수 없는 상황)
		}
	}

	return NextTargetViewPlayer;
}

APBPlayerState *APBCharacter::DecideNextPlayerToFollow() { return GetPlayerStateToFollow(&GetNextTargetViewPlayer); }

APBPlayerState *APBCharacter::DecidePreviousPlayerToFollow() { return GetPlayerStateToFollow(&GetPreviousTargetViewPlayer); }

void APBCharacter::Respawn()
{
	APBPlayerController* PC = Cast<APBPlayerController>(GetController());
	if (PC && PC->Role == ROLE_Authority)
	{
		PC->RestartPlayerCharacter();
	}
}

FVector APBCharacter::GetBoneLocation(FName BoneName)
{
	return GetMesh3P() ? GetMesh3P()->GetBoneLocation(BoneName) : FVector::ZeroVector;
}

FQuat APBCharacter::GetBoneQuaternion(FName BoneName)
{
	return GetMesh3P() ? GetMesh3P()->GetBoneQuaternion(BoneName) : FQuat::Identity;
}

float APBCharacter::GetHitAngle(const FVector& AttackPos)
{
	FVector TargetDir = AttackPos - GetCapsuleComponent()->GetComponentLocation();
	const float Rad = FMath::Atan2(TargetDir.Y, TargetDir.X);
	const float Deg = FMath::RadiansToDegrees(Rad);
	const float Diff = FRotator::NormalizeAxis(Deg - GetCapsuleComponent()->GetComponentRotation().Yaw);//(-180 ~ 180)
	
	//print(FString::Printf(TEXT("Y : %f, X : %f, Rad : %f, Deg : %f, Diff : %f"), TargetDir.Y, TargetDir.X, Rad, Deg, Diff));
	
	return Diff;
}

float APBCharacter::GetMoveDirectionAngle() const
{
	if (GetVelocity().Size() > 0.f)
	{
		const FVector MoveDirection = GetVelocity().GetSafeNormal();	
		return GetBetweenAngle(GetActorForwardVector(), MoveDirection);

		//const FVector MoveDirection = GetVelocity().GetSafeNormal();
		//const FVector ForwardVector = FRotationMatrix(GetActorRotation()).GetUnitAxis(EAxis::X);
		//const float Cos_Theta = FVector::DotProduct(MoveDirection, ForwardVector);
		//const float Theta = FMath::Acos(Cos_Theta);			//(0 ~ 3.14 Radian)
		//const float Angle = FMath::RadiansToDegrees(Theta); //(0 ~ 180 Degree)
	}
	return 0.f;
}

float APBCharacter::GetBetweenAngleForwardAnd(FVector Direction2) const
{
	return GetBetweenAngle(GetActorForwardVector(), Direction2);
}

float APBCharacter::GetBetweenAngle(FVector Direction1, FVector Direction2) const
{
	const float Angle = FRotator(FQuat::FindBetween(Direction1, Direction2)).Yaw; //(-180 ~ 180 degree)
	//print(FString::Printf(TEXT("angle : %f"), Angle));
	return Angle;
}

float APBCharacter::GetSpeed() const
{
	return GetVelocity().Size();
}

float APBCharacter::GetSpeed_H() const
{
	return FVector2D(GetVelocity().X, GetVelocity().Y).Size();
}

float APBCharacter::GetSpeed_V() const
{
	return GetVelocity().Z;
}

bool APBCharacter::IsStandingStill() const
{
	if (GetCharacterMovement())
	{
		 return GetSpeed_H() == 0.f && GetCharacterMovement()->GetCurrentAcceleration().Size() == 0.f;
	}
	return GetSpeed_H() == 0.f;
}

void APBCharacter::ForgetWhoKilledMe()
{
	APBPlayerState* ps = Cast<APBPlayerState>(PlayerState);
	if (ps)
	{ 
		ps->GetKillMarkProperties().PlayerWhoKilledMe = nullptr;
	}
}

void APBCharacter::SpawnDefaultInventory()
{
	if (GetWorld() == nullptr)
		return;

	if (Role < ROLE_Authority)
	{
		return;
	}

	if (!GetWorld()->IsPlayInEditor())
	{
		return;
	}

	Inventory.Empty();

	AddWeaponBySlotClasses(EWeaponSlot::Primary,	PrimaryWeaponClasses);
	AddWeaponBySlotClasses(EWeaponSlot::Secondary,	SecondaryWeaponClasses);
	AddWeaponBySlotClasses(EWeaponSlot::Melee,		MeleeWeaponClasses);
	AddWeaponBySlotClasses(EWeaponSlot::Grenade,	GrenadeWeaponClasses);
	AddWeaponBySlotClasses(EWeaponSlot::Special,	SpecialWeaponClasses);

	// Add GameMode Specific Weapon
	APBGameMode* GM = Cast<APBGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->AddSpecifyWeaponsTo(this);
	}

	EquipWeapon(EWeaponSlot::Primary);
}

//only server
void APBCharacter::SpawnUserInventory(AController* PlayerController)
{
	if (GetWorld() == nullptr)
		return;

	if (PlayerController == nullptr || Role < ROLE_Authority)
	{
		return;
	}	

	if (GetWorld()->IsPlayInEditor())
	{
		return;
	}

	Inventory.Empty();

	UPBItemTableManager* TM = UPBItemTableManager::Get(this);
	if (TM)
	{
		APBPlayerState* PS = Cast<APBPlayerState>(PlayerController->PlayerState);
		if (PS)
		{
			for (int32 WeapItemID : PS->ReplicateEquippedWeapItems)
			{
				TM->ApplyWeapItemToPawn(this, WeapItemID);
			}
		}
	}

	APBGameMode* GM = Cast<APBGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->AddSpecifyWeaponsTo(this);
	}

	EquipWeapon(EWeaponSlot::Primary);
}

void APBCharacter::AddWeaponBySlotClasses(EWeaponSlot eSlot, TArray<FWeaponModeSetClass>& SlotWeaponClasses)
{
	for (FWeaponModeSetClass& WeapModeSetClass : SlotWeaponClasses)
	{	
		AddWeaponByModeSetClass(eSlot, WeapModeSetClass);
	}
}

void APBCharacter::AddWeaponByModeSetClass(EWeaponSlot eSlot, FWeaponModeSetClass& WeapModeSetClass, int32 SlotIdx)
{
	FWeaponModeSet WeapModeSet;

	for (int32 Mode = 0; Mode < WeapModeSetClass.Classes.Num(); Mode++)
	{
		APBWeapon* Weapon = NewWeapon(WeapModeSetClass.Classes[Mode]);
		if (Weapon)
		{
			WeapModeSet.Weapons.Add(Weapon);
		}
	}
	AddWeapon(eSlot, WeapModeSet, SlotIdx);
}

APBWeapon* APBCharacter::NewWeapon(TSubclassOf<APBWeapon> WeapClass)
{
	if (GetWorld() == nullptr)
		return nullptr;

	if (WeapClass)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		return GetWorld()->SpawnActor<APBWeapon>(WeapClass, SpawnInfo);
	}
	return nullptr;
}

void APBCharacter::AddWeapon(EWeaponSlot eSlot, FWeaponModeSet WeapModeSet, int32 SlotIdx)
{
	if (Role == ROLE_Authority)
	{
		////////////////////////////////////////////
		// if already exist, remove
		if (SlotIdx != -1)
		{
			RemoveWeaponSlotIdx(eSlot, SlotIdx);
		}

		////////////////////////////////////////////
		// New Add
		for (APBWeapon* Weapon : WeapModeSet.Weapons)
		{
			if (Weapon)
			{
				Weapon->OnEnterInventory(this);
			}
		}

		// add item (Emplace == Add)
		int32 Slot = int32(eSlot);

		if (!Inventory.IsValidIndex(Slot))
		{
			Inventory.SetNum(Slot+1);
		}

		TArray<FWeaponModeSet>& SlotWeapons = Inventory[Slot].SlotWeapons;

		if (SlotIdx == -1)
		{
			SlotWeapons.Add(WeapModeSet);
		}
		else
		{
			if (!SlotWeapons.IsValidIndex(SlotIdx))
			{
				SlotWeapons.SetNum(SlotIdx + 1);
			}

			SlotWeapons.EmplaceAt(SlotIdx, WeapModeSet);
		}		
	}
}

void APBCharacter::RemoveWeaponSlot(EWeaponSlot eSlot)
{
	if (Role == ROLE_Authority)
	{
		TArray<FWeaponModeSet>& SlotWeapons = GetSlotWeapons(eSlot);

		for (FWeaponModeSet& WeapModeSet : SlotWeapons)
		{
			DestroyWeapon(WeapModeSet);
		}

		Inventory.RemoveAt(int32(eSlot));
	}
}

void APBCharacter::RemoveWeaponSlotIdx(EWeaponSlot eSlot, int32 SlotIdx)
{
	if (Role == ROLE_Authority)
	{
		TArray<FWeaponModeSet>& SlotWeapons = GetSlotWeapons(eSlot);

		if (SlotWeapons.IsValidIndex(SlotIdx))
		{
			FWeaponModeSet& WeapModeSet = SlotWeapons[SlotIdx];
			DestroyWeapon(WeapModeSet);

			SlotWeapons.RemoveAt(SlotIdx);
		}
	}
}

void APBCharacter::DestroyWeapon(FWeaponModeSet& WeapModeSet)
{
	for (APBWeapon* Weapon : WeapModeSet.Weapons)
	{
		if (Weapon && !Weapon->IsPendingKill())
		{
			Weapon->OnLeaveInventory();
			Weapon->Destroy();
		}
	}
}

void APBCharacter::DestroyInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	// remove all weapons from inventory and destroy them
	for (int32 i = Inventory.Num() - 1; i >= 0; i--)
	{
		RemoveWeaponSlot(EWeaponSlot(i));
	}
}

bool APBCharacter::SwitchToBestWeapon()
{
	for (int32 Slot = 0; Slot < Inventory.Num(); Slot++)
	{
		TArray<FWeaponModeSet>& SlotWeapons = Inventory[Slot].SlotWeapons;

		for (int32 SlotIdx = 0; SlotIdx < SlotWeapons.Num(); SlotIdx++)
		{
			FWeaponModeSet& WeapModeSet = SlotWeapons[SlotIdx];

			for (int32 Mode = 0; Mode < WeapModeSet.Weapons.Num(); Mode++)
			{
				APBWeapon* Weapon = WeapModeSet.Weapons[Mode];
				if (Weapon && Weapon->HasAmmo())
				{
					if (SwitchWeapon(EWeaponSlot(Slot), SlotIdx, EWeaponMode(Mode)))
					{
						return true;
					}
				}
			}
		}
	}

	return SwitchWeapon(EWeaponSlot(0), 0, EWeaponMode::Mode1);
}

void APBCharacter::InitActionPickup()
{
	PickupAction = CreateDefaultSubobject<UPBActionPickup>(TEXT("PickupAction"));
}



//called only server
void APBCharacter::PickedUpWeapon(TSubclassOf<APBWeapon> WeaponClass)
{
	if (WeaponClass == nullptr)
	{
		return;
	}

	EWeaponSlot WantsWeapSlot = EWeaponSlot::Primary;

	// if exist, fill ammo
	APBWeapon* Weapon = FindWeaponByType(WeaponClass);
	if (Weapon)
	{
		Weapon->FillAmmo();

		int32 OutSlotIdx;
		EWeaponMode OutMode;
		if (GetWeaponSlotWithIdxMode(Weapon, WantsWeapSlot, OutSlotIdx, OutMode))
		{
			SwitchWeapon(WantsWeapSlot, OutSlotIdx, OutMode);
			return;
		}
	}
	
	// New weapon
	if (GetWorld() && !GetWorld()->IsPlayInEditor())
	{
		Weapon = Cast<APBWeapon>(WeaponClass->GetDefaultObject());
		FPBWeaponTableData* WeaponData = Weapon ? Weapon->GetTableData() : nullptr;
		if (WeaponData)
		{
			WantsWeapSlot = EWeaponSlot(WeaponData->WeaponSlotEnum);
		}
	}

	FWeaponModeSetClass NewWeaponSet;
	NewWeaponSet.Classes.Add(WeaponClass);
	AddWeaponByModeSetClass(WantsWeapSlot, NewWeaponSet, 0);

	SwitchWeapon(WantsWeapSlot, 0);
}

void APBCharacter::PickedUpWeapon(TSubclassOf<APBWeapon> WeaponClass, int32 InCurrentAmmo, int32 InCurrentAmmoInClip)
{
	PickedUpWeapon(WeaponClass);
	if (GetCurrentWeapon())
	{
		// Set the pickup weapon's ammo count same as previous owner's last ammo count at the time of the death.
		GetCurrentWeapon()->SetAmmoForcibly(InCurrentAmmo, InCurrentAmmoInClip);
	}

}

void APBCharacter::ToggleSwitchWeaponMode()
{
	EWeaponMode NextMode = CurrentWeaponMode == EWeaponMode::Mode1 ? EWeaponMode::Mode2 : EWeaponMode::Mode1;
	SwitchWeaponMode(NextMode);
}

bool APBCharacter::SwitchWeaponMode(EWeaponMode eMode)
{
	return SwitchWeapon(CurrentWeaponSlot, CurrentWeaponSlotIdx, eMode);
}

bool APBCharacter::SwitchWeapon(EWeaponSlot eSlot, int32 SlotIdx, EWeaponMode eMode)
{
	APBWeapon* Weapon = GetWeapon(eSlot, SlotIdx, eMode);

	if (Weapon && Weapon->CanEquip() && Weapon != GetCurrentWeapon())
	{
		EquipWeapon(eSlot, SlotIdx, eMode);
		return true;
	}
	return false;
}

void APBCharacter::EquipWeaponItem(int32 ItemID, int32 ForceSlotIdx /*= -1*/)
{
	auto TM = UPBItemTableManager::Get(this);
	if (TM)
	{
		TM->ApplyWeapItemToPawn(this, ItemID, true, ForceSlotIdx);
	}	
}

// 조건 체크없이 그냥 Equip 시키는 함수 (조건이 필요하다면 SwitchWeapon 를 사용하세요)
void APBCharacter::EquipWeapon(EWeaponSlot eSlot, int32 SlotIdx, EWeaponMode eMode)
{
	APBWeapon* Weapon = GetWeapon(eSlot, SlotIdx, eMode);
	if (Weapon)
	{
		CurrentWeaponSlot = eSlot;
		CurrentWeaponSlotIdx = SlotIdx;
		CurrentWeaponMode = eMode;
		
		EquipWeaponInternal(Weapon);
	}	
}

void APBCharacter::EquipWeaponInternal(APBWeapon* Weapon)
{
	if (Weapon && Weapon != GetCurrentWeapon())
	{
		if (Role == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon, CurrentWeapon);
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}
}

bool APBCharacter::ServerEquipWeapon_Validate(APBWeapon* Weapon)
{
	return true;
}

void APBCharacter::ServerEquipWeapon_Implementation(APBWeapon* Weapon)
{
	EquipWeaponInternal(Weapon);
}

void APBCharacter::NotifyEquipWeapon_Implementation(APBWeapon* EquippedWeapon)
{
	
}

void APBCharacter::OnRep_CurrentWeapon(APBWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void APBCharacter::SetCurrentWeapon(APBWeapon* NewWeapon, APBWeapon* LastWeapon)
{
	APBWeapon* LocalLastWeapon = NULL;
	
	if (LastWeapon != NULL)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	// unequip previous
	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;

	// equip new one
	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);	// Make sure weapon's MyPawn is pointing back to us. During replication, we can't guarantee APawn::CurrentWeapon will rep after AWeapon::MyPawn!

		NewWeapon->OnEquip(LastWeapon);

		if (IsLocallyControlled())
		{
			APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
			if (MyPC && MyPC->WidgetEventDispatcher.IsValid())
			{
				MyPC->WidgetEventDispatcher->OnWeaponChange.Broadcast();
			}
		}
	}

	//
	if (CurrentQuickWeapon)
	{
		CurrentQuickWeapon->OnUnEquip();
		CurrentQuickWeapon = nullptr;
	}
}

// called local & server
// 무기 블루프린트에서 QuickThrowTriggerAction 를 설정 해야 작동한다.("QuickThrowing" 을 검색)
bool APBCharacter::StartQuickThrow(APBWeap_Throw* ThrowWeapon)
{
	if (CurrentWeapon && CurrentWeapon->IsMeleeAttacking())
	{
		return false;
	}

	if (ThrowWeapon && ThrowWeapon->CanFire() && !ThrowWeapon->IsInFireInterval() && ThrowWeapon->HasAmmoInClip())
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->OnUnEquip();
		}

		CurrentQuickWeapon = ThrowWeapon;

		ThrowWeapon->OnEquip(nullptr);
		ThrowWeapon->SetQuickThrowing();
		ThrowWeapon->StartFire(EFireTrigger::Fire1);
		return true;
	}

	return false;
}

// called local & server
void APBCharacter::StopQuickThrow(APBWeap_Throw* ThrowWeapon)
{
	if (ThrowWeapon)
	{
		ThrowWeapon->EndFire(EFireTrigger::Fire1);
	}
}

void APBCharacter::OnQuickThrowEnd()
{
	if (CurrentQuickWeapon)
	{
		if (CurrentWeapon && CurrentWeapon->CanEquip())
		{
			CurrentWeapon->OnEquip(CurrentQuickWeapon);
		}

		CurrentQuickWeapon->OnUnEquip();
		CurrentQuickWeapon = nullptr;
	}
}

void APBCharacter::OnRep_CurrentQuickWeapon(class APBWeapon* LastQuickWeapon)
{
	//Current Weapon pointer 와 CurrentWeaponSlot 을 그대로 유지한 상태로 어태치, 디데치를 한다
	if (CurrentQuickWeapon)
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->OnUnEquip();
		}
	}
	else
	{
		if (CurrentWeapon && CurrentWeapon->CanEquip())
		{
			CurrentWeapon->OnEquip(LastQuickWeapon);
		}
	}

	//Quick Throwing Weapon
	if (CurrentQuickWeapon)
	{
		CurrentQuickWeapon->OnEquip(nullptr);
		CurrentQuickWeapon->StartFire(EFireTrigger::Fire1);
	}
	else if (LastQuickWeapon)
	{
		LastQuickWeapon->OnUnEquip();
	}
}

float APBCharacter::GetRunningSpeedModifier() const
{
	return CharacterData.RunSpeedModifier;
}

float APBCharacter::GetWalkingSpeedModifier() const
{
	return CharacterData.WalkSpeedModifier;
}

void APBCharacter::SetWalking(bool bNewWalking)
{
	bWantsToWalk = bNewWalking;

	if (Role < ROLE_Authority)
	{
		ServerSetWalking(bNewWalking);
	}
}

bool APBCharacter::ServerSetWalking_Validate(bool bNewWalking)
{
	return true;
}

void APBCharacter::ServerSetWalking_Implementation(bool bNewWalking)
{
	SetWalking(bNewWalking);
}

float APBCharacter::GetSprintingSpeedModifier() const
{
	return CharacterData.SprintingSpeedModifier;
}

void APBCharacter::SetSprinting(bool bNewSprinting)
{
	bWantsToSprint = bNewSprinting;

	if (Role < ROLE_Authority)
	{
		ServerSetSprinting(bNewSprinting);
	}

	UpdateSprintSounds(bNewSprinting);
}

bool APBCharacter::ServerSetSprinting_Validate(bool bNewSprinting)
{
	return true;
}

void APBCharacter::ServerSetSprinting_Implementation(bool bNewSprinting)
{
	SetSprinting(bNewSprinting);
}

bool APBCharacter::CanSprint() const
{
	//dot product
	return (bWantsToSprint && (GetVelocity().GetSafeNormal2D() | GetActorForwardVector()) > -0.1f);
}

EPBMovementType APBCharacter::GetWantsMovementType() const
{
	if (GetVelocity().IsZero())
	{
		return EPBMovementType::Standing;
	}
	else if (bWantsToWalk)
	{
		return EPBMovementType::Walking;
	}
	else if (CanSprint())
	{
		return EPBMovementType::Sprinting;
	}
	else
	{
		return EPBMovementType::Running;
	}
}

bool APBCharacter::IsRunning() const
{
	return (GetWantsMovementType() == EPBMovementType::Running);
}

bool APBCharacter::IsWalking() const
{
	return (GetWantsMovementType() == EPBMovementType::Walking);
}

bool APBCharacter::IsSprinting() const
{
	return (GetWantsMovementType() == EPBMovementType::Sprinting);
}

void APBCharacter::InitLookingAround()
{
	EyeSpeed_Normal = nullptr;

	JoystickDeadZone = 0.1f;
	ZoomEyeSpeedMult = 0.5f;
	NoZoomHorzEyeSpeedMult = 2.f;
	FiringEyeSpeedMult = 1.f;

	CachedAimSensitivity = 1.f;
}

void APBCharacter::RegisterLookAroundHandlers()
{
	InputComponent->BindAxis("Turn", this, &APBCharacter::Turn);
	InputComponent->BindAxis("TurnRate", this, &APBCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APBCharacter::LookUp);
	InputComponent->BindAxis("LookUpRate", this, &APBCharacter::LookUpAtRate);
}

void APBCharacter::StartUpLookAround()
{
	APBPlayerController* PC = Cast<APBPlayerController>(GetController());
	if (PC)
	{
		UPBPersistentUser* PU = PC->GetPersistentUser();
		if (PU)
		{
			CachedAimSensitivity = FMath::Max(SMALL_NUMBER, PU->GetAimSensitivity());
			UserSettingChangedEvtHandle = PU->OnUserSettingUpdated.AddUObject(this, &APBCharacter::HandleUserSettingChanged);
		}
	}
}

void APBCharacter::HandleUserSettingChanged()
{
	APBPlayerController* PC = Cast<APBPlayerController>(GetController());
	if (PC)
	{
		UPBPersistentUser* PU = PC->GetPersistentUser();
		if (PU)
		{
			CachedAimSensitivity = FMath::Max(SMALL_NUMBER, PU->GetAimSensitivity());
		}
	}
}

void APBCharacter::ShutDownLookAround()
{
	APBPlayerController* PC = Cast<APBPlayerController>(GetController());
	if (PC)
	{
		UPBPersistentUser* PU = PC->GetPersistentUser();
		if (PU)
		{
			PU->OnUserSettingUpdated.Remove(UserSettingChangedEvtHandle);
		}
	}
}

void APBCharacter::AdjustLookAroundInputSetting(APlayerController* NewController)
{
	if (!NewController)
		return;

	UPlayerInput* PlayerInput = NewController->PlayerInput;
	if (!PlayerInput)
		return;

	for (int32 Idx = 0; Idx < PlayerInput->AxisMappings.Num(); Idx++)
	{
		FInputAxisKeyMapping &AxisMapping = PlayerInput->AxisMappings[Idx];
		if (AxisMapping.AxisName == "LookupRate" || AxisMapping.AxisName == "TurnRate")
		{
			FInputAxisProperties AxisPropertiesCache;
			PlayerInput->GetAxisProperties(AxisMapping.Key, AxisPropertiesCache);
			AxisPropertiesCache.DeadZone = JoystickDeadZone;
			PlayerInput->SetAxisProperties(AxisMapping.Key, AxisPropertiesCache);
		}
	}
}

void APBCharacter::Turn(float Val)
{
#if WITH_EDITOR
	APBCharacter *FollowingTarget = TargetViewPlayer ? TargetViewPlayer->OwnerCharacter : this;

	if (IsCameraStyle(EPBCameraStyle::DebugFreeCam))
	{
		if (nullptr != FollowingTarget)
		{
			const float Sensitive = 10.0f;

			FollowingTarget->DebugFreeCamComp->SetWorldRotation(FollowingTarget->DebugFreeCamComp->GetComponentRotation().Add(0, CachedAimSensitivity * Val * Sensitive, 0));

			return;
		}
	}
	else if (IsCameraStyle(EPBCameraStyle::DebugSphericalCam))
	{
		if (nullptr != FollowingTarget)
		{
			const float Sensitive = 0.1f;

			FollowingTarget->DebugSphericalCamSpringArm->AddWorldRotation(FQuat(FVector::UpVector, CachedAimSensitivity * Val * Sensitive));

			return;
		}
	}
#endif

	float MultiplierToApply = 1.f;
	if (IsZooming())
	{
		MultiplierToApply *= ZoomEyeSpeedMult;
		if (GetCurrentWeapon())
		{
			MultiplierToApply *= GetCurrentWeapon()->GetZoomEyeSpeedMultiplier();
		}
	}
	if (IsFiring())
	{
		MultiplierToApply *= FiringEyeSpeedMult;
		if (GetCurrentWeapon())
		{
			MultiplierToApply *= GetCurrentWeapon()->GetFireEyeSpeedMultiplier();
		}
	}

	AddControllerYawInput(CachedAimSensitivity * MultiplierToApply *  Val);
	//MulticastAddControllerYawInput(CachedAimSensitivity * MultiplierToApply *  Val);
}

void APBCharacter::TurnAtRate(float Val)
{
	if (EyeSpeed_Normal)
	{
		float MultiplierToApply = 1.f;
		if (!IsZooming())
		{
			MultiplierToApply *= NoZoomHorzEyeSpeedMult;
		}

		const float TimeDelta = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.f;
		Turn(FMath::Sign(Val) * EyeSpeed_Normal->GetFloatValue(FMath::Abs(Val))	* MultiplierToApply * TimeDelta);
	}
}

void APBCharacter::LookUp(float Val)
{
#if WITH_EDITOR
	APBCharacter *FollowingTarget = TargetViewPlayer ? TargetViewPlayer->OwnerCharacter : this;

	if (IsCameraStyle(EPBCameraStyle::DebugFreeCam))
	{
		const float Sensitive = 10.0f;

		FollowingTarget->DebugFreeCamComp->SetWorldRotation(FollowingTarget->DebugFreeCamComp->GetComponentRotation().Add(CachedAimSensitivity * -Val * Sensitive, 0, 0));

		return;
	}
	else if (IsCameraStyle(EPBCameraStyle::DebugSphericalCam))
	{
		const float Sensitive = 0.1f;

		FVector FollowingTargetRightVector = FollowingTarget->DebugSphericalCamSpringArm->GetRightVector();
		FollowingTarget->DebugSphericalCamSpringArm->AddWorldRotation(FQuat(FollowingTargetRightVector, CachedAimSensitivity * Val * Sensitive));

		return;
	}
#endif

	float MultiplierToApply = 1.f;
	if (IsZooming())
	{
		MultiplierToApply *= ZoomEyeSpeedMult;
		if (GetCurrentWeapon())
		{
			MultiplierToApply *= GetCurrentWeapon()->GetZoomEyeSpeedMultiplier();
		}
	}
	if (IsFiring())
	{
		MultiplierToApply *= FiringEyeSpeedMult;
		if (GetCurrentWeapon())
		{
			MultiplierToApply *= GetCurrentWeapon()->GetFireEyeSpeedMultiplier();
		}
	}

	AddControllerPitchInput(CachedAimSensitivity * MultiplierToApply * Val);
	//MulticastAddControllerPitchInput(CachedAimSensitivity * MultiplierToApply * Val);
}

void APBCharacter::LookUpAtRate(float Val)
{
	if (EyeSpeed_Normal)
	{
		const float TimeDelta = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.f;

		LookUp(FMath::Sign(Val) * EyeSpeed_Normal->GetFloatValue(FMath::Abs(Val)) * TimeDelta);
	}
}

void APBCharacter::UnbeatableOn(float Period /*= 0*/)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Unbeatable);
	}

	bUnbeatable = true;

	if (Period > SMALL_NUMBER)
	{
		APBGameMode* const GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<APBGameMode>() : nullptr;
		if (GameMode)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_Unbeatable, this, &APBCharacter::UnbeatableOff, Period, false);
		}
	}
}

void APBCharacter::UnbeatableOff()
{
	bUnbeatable = false;
}

bool APBCharacter::HasUnbeatable()
{
	return bUnbeatable;
}

void APBCharacter::InitUnBeatableSys()
{
	bUnbeatable = false;
}

FRotator APBCharacter::GetViewRotation() const
{
	auto Rtn = Super::GetViewRotation();

	if (GetCurrentWeapon())
	{
		FRotator PitchCorrector(InterpedRecoilPitchCorr * GetCurrentWeapon()->GetCamRecoilRatio(), 0, 0);
		Rtn = (Rtn.Quaternion() * PitchCorrector.Quaternion()).Rotator();

		FRotator YawCorrector(0, InterpedRecoilYawCorr * GetCurrentWeapon()->GetCamRecoilRatio(), 0);
		Rtn = (Rtn.Quaternion() * YawCorrector.Quaternion()).Rotator();
	}

	return Rtn;
}

FRotator APBCharacter::GetBaseAimRotation() const
{
	// This will get us a well-replicated rotation but it includes the camera recoil rotation offset
	auto Rtn = Super::GetBaseAimRotation();

	// Remove the camera recoil rotation offset
	if (GetCurrentWeapon())
	{
		FRotator PitchCorrector(InterpedRecoilPitchCorr * GetCurrentWeapon()->GetCamRecoilRatio(), 0, 0);
		Rtn = (Rtn.Quaternion() * PitchCorrector.Quaternion().Inverse()).Rotator();

		FRotator YawCorrector(0, InterpedRecoilYawCorr * GetCurrentWeapon()->GetCamRecoilRatio(), 0);
		Rtn = (Rtn.Quaternion() * YawCorrector.Quaternion().Inverse()).Rotator();
	}

	// Add the weapon recoil rotation
	{
		FRotator PitchCorrector(InterpedRecoilPitchCorr, 0, 0);
		Rtn = (Rtn.Quaternion() * PitchCorrector.Quaternion()).Rotator();

		FRotator YawCorrector(0, InterpedRecoilYawCorr, 0);
		Rtn = (Rtn.Quaternion() * YawCorrector.Quaternion()).Rotator();
	}

	return Rtn;
}

void APBCharacter::NotifyWeaponBurst(int BurstCounter, const FWeaponRecoilData& WeaponData)
{
	RecoilPitchCorr += WeaponData.RecoilVertMinAddition 
		+ RecoilRandomStream.GetFraction() * FMath::Max(WeaponData.RecoilVertAngle - WeaponData.RecoilVertMinAddition, 0.f);

	RecoilPitchCorr = FMath::Clamp(RecoilPitchCorr, 0.f, WeaponData.RecoilVertMaxAngle);

	if (WeaponData.RecoilHorzBeginAtShot - 1 <= BurstCounter)
	{
		auto RandFact = RecoilRandomStream.GetFraction();
		RecoilYawCorr += (1 - 2 * RandFact) * WeaponData.RecoilHorzAngle;
		RecoilYawCorr = FMath::Clamp(RecoilYawCorr, -WeaponData.RecoilHorzMaxAngle, WeaponData.RecoilHorzMaxAngle);
	}
}

void APBCharacter::InitRecoilSystem()
{
	const int32 RandomSeed = FMath::Rand();
	RecoilRandomStream = FRandomStream(RandomSeed);
}

void APBCharacter::InterpRecoilCorr(float DeltaTime)
{
	if (GetCurrentWeapon())
	{
		auto RecoilCorrRate = GetCurrentWeapon()->GetRecoilReturnRate();
		if (RecoilCorrRate > SMALL_NUMBER)
		{
			RecoilPitchCorr = FMath::FInterpTo(RecoilPitchCorr, 0, DeltaTime, RecoilCorrRate);
			RecoilYawCorr = FMath::FInterpTo(RecoilYawCorr, 0, DeltaTime, RecoilCorrRate);
		}

		auto CamInterpRate = GetCurrentWeapon()->GetCamSyncRate();
		if (CamInterpRate > SMALL_NUMBER)
		{
			InterpedRecoilPitchCorr = FMath::FInterpTo(InterpedRecoilPitchCorr, RecoilPitchCorr, DeltaTime, CamInterpRate);
			InterpedRecoilYawCorr = FMath::FInterpTo(InterpedRecoilYawCorr, RecoilYawCorr, DeltaTime, CamInterpRate);
		}
	}
}

void APBCharacter::RaiseDetectableEvent()
{
	LastDetectableEventRaisedAt = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
}

float APBCharacter::GetTimeSinceLastDetectableEvent()
{
	return GetWorld() ? (GetWorld()->GetTimeSeconds() - LastDetectableEventRaisedAt) : 0.f;
}

void APBCharacter::InitDetection()
{
	LastDetectableEventRaisedAt = 0.f;
}

void APBCharacter::StartUpDetection()
{
	// Initialise the LastDetectableEventRaisedAt value
	RaiseDetectableEvent();

	GetWorldTimerManager().SetTimer(CheckDetectable_Handle, this, &APBCharacter::CheckDetectable, 0.1f, true);
}

void APBCharacter::ShutDownDetection()
{
	GetWorldTimerManager().ClearTimer(CheckDetectable_Handle);
}

void APBCharacter::CheckDetectable()
{
	if (IsSprinting()
		|| IsFiring()
		|| (GetCharacterMovement() && GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling))
	{
		RaiseDetectableEvent();
	}
}

void APBCharacter::OnStartCrouch(float HeightAdjust, float ScaledHeightAdjust)
{
	// Engine Bug Note:
	// This is originally CameraComp->SetRelativeLocation(FVector(0, 0, CameraComp->RelativeLocation.Z + HeightAdjust)).
	// But the code would be skipped. (because the world position has not changed?) It seems like an optimization-induced bug.

	FirstPersonViewScene->RelativeLocation.Z += HeightAdjust;
	FirstPersonViewScene->UpdateComponentToWorld();

	Super::OnStartCrouch(HeightAdjust, ScaledHeightAdjust);
}

void APBCharacter::OnEndCrouch(float HeightAdjust, float ScaledHeightAdjust)
{
	Super::OnEndCrouch(HeightAdjust, ScaledHeightAdjust);

	FirstPersonViewScene->RelativeLocation.Z -= HeightAdjust;
	FirstPersonViewScene->UpdateComponentToWorld();
}

void APBCharacter::InitFirstPersonViewpoint()
{
	FirstPersonViewScene = CreateDefaultSubobject<USceneComponent>(TEXT("FirstPersonViewScene"));
	FirstPersonViewScene->SetupAttachment(GetCapsuleComponent());
	FirstPersonViewScene->RelativeLocation = FVector(0, 0, BaseEyeHeight);
	FirstPersonViewScene->SetIsReplicated(true);
}

FVector APBCharacter::GetRagdollViewpointPosition()
{
	FName HeadSocketName = FName(TEXT("EyeSocket"));
	return GetMesh3P()->GetSocketTransform(HeadSocketName, RTS_World).GetLocation();
}

FRotator APBCharacter::GetRagdollViewpointRotation()
{
	FName HeadSocketName = FName(TEXT("EyeSocket"));
	return GetMesh3P()->GetSocketTransform(HeadSocketName, RTS_World).GetRotation().Rotator();
}

void APBCharacter::UpdateSpectatorCameraRotationSync()
{

	if (IsNetMode(NM_ListenServer) || IsNetMode(NM_DedicatedServer))
	{
		if (nullptr != FirstPersonViewScene)
		{
			FirstPersonViewTransform = FirstPersonViewScene->GetComponentTransform();
			FirstPersonViewTransform.SetRotation(GetViewRotation().Quaternion());
		}

		if (nullptr != ThirdPersonSpringArm)
		{
			ThirdPersonViewTransform = ThirdPersonSpringArm->GetSocketTransform(USpringArmComponent::SocketName);
		}
	}

	if (nullptr != FirstPersonViewScene)
	{
		if (IsLocallyControlled())
		{
			FirstPersonViewScene->SetWorldRotation(GetViewRotation());
		}
		else
		{
			FirstPersonViewTransform.SetLocation(FirstPersonViewScene->GetComponentLocation());
			FirstPersonViewScene->SetWorldRotation(FirstPersonViewTransform.GetRotation());
		}
	}
}

FTransform APBCharacter::GetFirstPersonViewTransform() const
{
	if (IsNetMode(NM_DedicatedServer) || IsNetMode(NM_ListenServer)
		|| (IsLocallyControlled() && IsViewingOwnself()))
	{
		if (nullptr != FirstPersonViewScene)
		{
			return FirstPersonViewScene->GetComponentTransform();
		}
	}

	return FirstPersonViewTransform;
}

FTransform APBCharacter::GetThirdPersonViewTransform() const
{
	if (IsNetMode(NM_DedicatedServer) || IsNetMode(NM_ListenServer)
		|| (IsLocallyControlled() && IsViewingOwnself()))
	{
		if (nullptr != ThirdPersonSpringArm)
		{
			return ThirdPersonSpringArm->GetSocketTransform(USpringArmComponent::SocketName);
		}
	}

	return ThirdPersonViewTransform;
}

/*
void APBCharacter::AddControllerYawInput(float Val)
{
	Super::AddControllerYawInput(Val);

	if (Role < ROLE_Authority)
	{
		ServerAddControllerYawInput(Val);
	}
}

bool APBCharacter::ServerAddControllerYawInput_Validate(float Val)
{
	return true;
}

void APBCharacter::ServerAddControllerYawInput_Implementation(float Val)
{
	if (GetViewerIsFirstPerson())
	{
		AddControllerYawInput(Val);
	}
	else
	{
		auto PC = Cast<APBPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		if (nullptr != PC)
		{
			PC->AddYawInput(Val);
		}
	}
}

void APBCharacter::AddControllerPitchInput(float Val)
{
	Super::AddControllerPitchInput(Val);

	if (Role < ROLE_Authority)
	{
		ServerAddControllerPitchInput(Val);
	}
}

bool APBCharacter::ServerAddControllerPitchInput_Validate(float Val)
{
	return true;
}

void APBCharacter::ServerAddControllerPitchInput_Implementation(float Val)
{
	if (GetViewerIsFirstPerson())
	{
		AddControllerPitchInput(Val);
	}
	else
	{
		auto PC = Cast<APBPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		if (nullptr != PC)
		{
			PC->AddPitchInput(Val);
		}
	}
}
*/

void APBCharacter::InitThirdPersonViewpoint()
{
	ThirdPersonSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdPersonSpringArm"));
	ThirdPersonSpringArm->SetIsReplicated(true);
	//@note: EyeSocket
	//ThirdPersonSpringArm->SetupAttachment(GetMesh3P(), TEXT("EyeSocket"));
	//@note: Non-EyeSocket
	{
		ThirdPersonSpringArm->SetupAttachment(GetCapsuleComponent());
		ThirdPersonSpringArm->RelativeLocation = FVector(0, 0, BaseEyeHeight);
	}
	ThirdPersonSpringArm->bUsePawnControlRotation = true;
	ThirdPersonSpringArm->RelativeRotation = FRotator(30.0f, 0, 0);
}

void APBCharacter::InitDebugCamViewpoint()
{
	DebugSphericalCamSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("DebugSphericalCamSpringArm"));
	//@note: EyeSocket
	//DebugSphericalCamSpringArm->SetupAttachment(GetMesh3P(), TEXT("EyeSocket"));
	//@note: Non-EyeSocket
	{
		DebugSphericalCamSpringArm->SetupAttachment(GetCapsuleComponent());
		DebugSphericalCamSpringArm->RelativeLocation = FVector(0, 0, BaseEyeHeight);
	}
	DebugSphericalCamSpringArm->bDoCollisionTest = false;
	DebugSphericalCamSpringArm->bShouldUpdatePhysicsVolume = false;
	DebugSphericalCamSpringArm->RelativeRotation = FRotator(-40.0f, 0, 0);
	DebugSphericalCamSpringArm->TargetArmLength = 300.0f;
	DebugSphericalCamSpringArm->bEnableCameraLag = true;
	DebugSphericalCamSpringArm->CameraLagSpeed = 0;
	DebugSphericalCamSpringArm->bEnableCameraRotationLag = true;
	DebugSphericalCamSpringArm->CameraRotationLagSpeed = 0;
}

void APBCharacter::InitCameraSys()
{
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComp->SetupAttachment(FirstPersonViewScene, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = true;
	CameraComp->SetIsReplicated(true);

	ThirdPersonCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComp->SetupAttachment(ThirdPersonSpringArm, USpringArmComponent::SocketName);
	ThirdPersonCameraComp->bUsePawnControlRotation = true;
	ThirdPersonCameraComp->SetIsReplicated(true);

	DebugFreeCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("DebugFreeCamera"));
	DebugFreeCamComp->SetupAttachment(ThirdPersonSpringArm, USpringArmComponent::SocketName);
	DebugFreeCamComp->bUsePawnControlRotation = false;
	DebugFreeCamComp->bShouldUpdatePhysicsVolume = false;

	DebugSphericalCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("DebugSphericalCamera"));
	DebugSphericalCamComp->SetupAttachment(DebugSphericalCamSpringArm, USpringArmComponent::SocketName);
	DebugSphericalCamComp->bUsePawnControlRotation = false;
	DebugSphericalCamComp->bShouldUpdatePhysicsVolume = false;
}

void APBCharacter::EnterRagdollCamMode(AActor *Target)
{
	APBPlayerController* PC = Cast<APBPlayerController>(GetController());
	if (nullptr == PC || false == PC->IsLocalController())
	{
		UpdateMeshVisibility(true);
		return;
	}

	APBPlayerCameraManager* PCM = Cast<APBPlayerCameraManager>(PC->PlayerCameraManager);
	if (nullptr == PCM)
		return;

	CameraComp->SetActive(true);
	ThirdPersonCameraComp->SetActive(false);
	DebugFreeCamComp->SetActive(false);
	DebugSphericalCamComp->SetActive(false);

	PCM->EnterRagdollMode(Cast<APawn>(Target));
	SetCameraStyle(PCM->GetCameraStyle());

	bIsViewOwnself = (this == Target);
	if (bIsViewOwnself)
	{
		PCM->SetViewTarget(this);

		UpdateMeshVisibility(false);
	}
	else
	{
		auto TC = Cast<APBCharacter>(Target);
		if (nullptr == TC)
			return;

		CameraComp->SetActive(false);

		TC->CameraComp->SetActive(true);
		TC->ThirdPersonCameraComp->SetActive(false);
		TC->DebugFreeCamComp->SetActive(false);
		TC->DebugSphericalCamComp->SetActive(false);
		PCM->SetViewTarget(Target);

		TC->UpdateMeshVisibility(false);
	}
}

void APBCharacter::EnterDeathCamMode(AActor *Target, AActor* Killer)
{
	APBPlayerController* PC = Cast<APBPlayerController>(GetController());
	if (nullptr == PC || false == PC->IsLocalController())
	{
		UpdateMeshVisibility(true);
		return;
	}

	APBPlayerCameraManager* PCM = Cast<APBPlayerCameraManager>(PC->PlayerCameraManager);
	if (nullptr == PCM)
		return;

	CameraComp->SetActive(true);
	ThirdPersonCameraComp->SetActive(false);
	DebugFreeCamComp->SetActive(false);
	DebugSphericalCamComp->SetActive(false);

	PCM->EnterDeathCamMode(Cast<APawn>(Target), Killer);
	SetCameraStyle(PCM->GetCameraStyle());

	bIsViewOwnself = (this == Target);
	if (bIsViewOwnself)
	{
		PCM->SetViewTarget(this);
	}
	else
	{
		auto TC = Cast<APBCharacter>(Target);
		if (nullptr == TC)
			return;

		CameraComp->SetActive(false);

		TC->CameraComp->SetActive(true);
		TC->ThirdPersonCameraComp->SetActive(false);
		TC->DebugFreeCamComp->SetActive(false);
		TC->DebugSphericalCamComp->SetActive(false);
		PCM->SetViewTarget(Target);
	}
}

void APBCharacter::EnterFPSCamMode(AActor *Target)
{
	APBPlayerController* PC = Cast<APBPlayerController>(GetController());
	if (nullptr == PC || false == PC->IsLocalController())
	{
		return;
	}

	APBPlayerCameraManager* PCM = Cast<APBPlayerCameraManager>(PC->PlayerCameraManager);
	if (nullptr == PCM)
		return;

	PCM->EnterFPSMode(Cast<APawn>(Target));
	SetCameraStyle(PCM->GetCameraStyle());

	bIsViewOwnself = (this == Target);
	if (bIsViewOwnself)
	{
		CameraComp->SetActive(true);
		ThirdPersonCameraComp->SetActive(false);
		DebugFreeCamComp->SetActive(false);
		DebugSphericalCamComp->SetActive(false);

		PCM->SetViewTarget(this);
		if (GetCurrentWeapon())
		{
			GetCurrentWeapon()->OnEquipFinished();
		}
		UpdateMeshVisibility(true);
	}
	else
	{
		auto TC = Cast<APBCharacter>(Target);
		if (nullptr == TC)
			return;

		CameraComp->SetActive(false);

		TC->CameraComp->SetActive(true);
		TC->ThirdPersonCameraComp->SetActive(false);
		TC->DebugFreeCamComp->SetActive(false);
		TC->DebugSphericalCamComp->SetActive(false);

		PCM->SetViewTarget(Target);
		if (TC->GetCurrentWeapon())
		{
			TC->GetCurrentWeapon()->OnEquipFinished();
		}
		TC->UpdateMeshVisibility(true);
	}
}

void APBCharacter::EnterTPSCamMode(AActor *Target)
{
	APBPlayerController* PC = Cast<APBPlayerController>(GetController());
	if (nullptr == PC || false == PC->IsLocalController())
		return;

	APBPlayerCameraManager* PCM = Cast<APBPlayerCameraManager>(PC->PlayerCameraManager);
	if (nullptr == PCM)
		return;

	PCM->EnterTPSMode(Cast<APawn>(Target));
	SetCameraStyle(PCM->GetCameraStyle());

	bIsViewOwnself = (this == Target);
	if (bIsViewOwnself)
	{
		CameraComp->SetActive(false);
		ThirdPersonCameraComp->SetActive(true);
		DebugFreeCamComp->SetActive(false);
		DebugSphericalCamComp->SetActive(false);

		ThirdPersonSpringArm->bUsePawnControlRotation = true;

		PCM->SetViewTarget(this);
		if (GetCurrentWeapon())
		{
			GetCurrentWeapon()->OnEquipFinished();
		}
		UpdateMeshVisibility(true);
	}
	else
	{
		auto TC = Cast<APBCharacter>(Target);
		if (nullptr == TC)
			return;

		ThirdPersonCameraComp->SetActive(false);

		TC->CameraComp->SetActive(false);
		TC->ThirdPersonCameraComp->SetActive(true);
		TC->DebugFreeCamComp->SetActive(false);
		TC->DebugSphericalCamComp->SetActive(false);

		PCM->SetViewTarget(Target);
		if (TC->GetCurrentWeapon())
		{
			TC->GetCurrentWeapon()->OnEquipFinished();
		}
		TC->UpdateMeshVisibility(true);
	}
}

void APBCharacter::EnterDebugFreeCamMode(AActor *Target)
{
	APBPlayerController* PC = Cast<APBPlayerController>(GetController());
	if (nullptr == PC || false == PC->IsLocalController())
		return;

	APBPlayerCameraManager* PCM = Cast<APBPlayerCameraManager>(PC->PlayerCameraManager);
	if (nullptr == PCM)
		return;

	CameraComp->SetActive(false);
	ThirdPersonCameraComp->SetActive(false);
	DebugFreeCamComp->SetActive(true);
	DebugSphericalCamComp->SetActive(false);

	PCM->EnterDebugFreeCam(Cast<APawn>(Target));
	SetCameraStyle(PCM->GetCameraStyle());

	bIsViewOwnself = (this == Target);
	if (bIsViewOwnself)
	{
		ThirdPersonSpringArm->bUsePawnControlRotation = true;

		PCM->SetViewTarget(this);
		UpdateMeshVisibility(true);

		if (GetCurrentWeapon())
		{
			GetCurrentWeapon()->OnEquipFinished();
		}
	}
	else
	{
		auto TC = Cast<APBCharacter>(Target);
		if (nullptr == TC)
			return;

		TC->UpdateMeshVisibility(true);

		if (TC->GetCurrentWeapon())
		{
			TC->GetCurrentWeapon()->OnEquipFinished();
		}
	}
}

void APBCharacter::EnterDebugSphericalCamMode(AActor *Target)
{
	APBPlayerController* PC = Cast<APBPlayerController>(GetController());
	if (nullptr == PC || false == PC->IsLocalController())
		return;

	APBPlayerCameraManager* PCM = Cast<APBPlayerCameraManager>(PC->PlayerCameraManager);
	if (nullptr == PCM)
		return;

	CameraComp->SetActive(false);
	ThirdPersonCameraComp->SetActive(true);
	DebugFreeCamComp->SetActive(false);
	DebugSphericalCamComp->SetActive(false);

	PCM->EnterDebugSphericalCam(Cast<APawn>(Target));
	SetCameraStyle(PCM->GetCameraStyle());

	bIsViewOwnself = (this == Target);
	if (bIsViewOwnself)
	{
		PCM->SetViewTarget(this);
		UpdateMeshVisibility(true);

		if (GetCurrentWeapon())
		{
			GetCurrentWeapon()->OnEquipFinished();
		}
	}
	else
	{
		auto TC = Cast<APBCharacter>(Target);
		if (nullptr == TC)
			return;

		TC->UpdateMeshVisibility(true);

		if (TC->GetCurrentWeapon())
		{
			TC->GetCurrentWeapon()->OnEquipFinished();
		}
	}
}

class UPBPlayerNameTag* APBCharacter::GetNameTagWidget()
{
	if (NameTagWidgetComp)
	{
		return Cast<UPBPlayerNameTag>(NameTagWidgetComp->GetUserWidgetObject());
	}
	return nullptr;
}

void APBCharacter::SetCharacterPtrToNameTag()
{
	if (NameTagWidgetComp)
	{
		UPBPlayerNameTag* NameTag = Cast<UPBPlayerNameTag>(NameTagWidgetComp->GetUserWidgetObject());
		if (NameTag)
		{
			NameTag->SetOwnerCharacter(this);
		}
	}
}


void APBCharacter::InitComponentDamageSys()
{
	HeadShotDmgAdjust = 34.f;
	BodyShotDmgAdjust = 0.2f;
	ArmShotDmgAdjust = -0.2f;
	LegShotDmgAdjust = -0.2f;

	HeadBones.Add(FName(TEXT("Bip001-Head")));
	HeadBones.Add(FName(TEXT("Bip001-Neck")));

	BodyBones.Add(FName(TEXT("Bip001-Pelvis")));
	BodyBones.Add(FName(TEXT("Bip001-Spine")));
	BodyBones.Add(FName(TEXT("Bip001-Spine1")));

	ArmBones.Add(FName(TEXT("Bip001-R-UpperArm")));
	ArmBones.Add(FName(TEXT("Bip001-R-Forearm")));
	ArmBones.Add(FName(TEXT("Bip001-R-Hand")));

	ArmBones.Add(FName(TEXT("Bip001-L-UpperArm")));
	ArmBones.Add(FName(TEXT("Bip001-L-Forearm")));
	ArmBones.Add(FName(TEXT("Bip001-L-Hand")));

	LegBones.Add(FName(TEXT("Bip001-R-Thigh")));
	ArmBones.Add(FName(TEXT("Bip001-R-Calf")));
	ArmBones.Add(FName(TEXT("Bip001-R-Foot")));

	LegBones.Add(FName(TEXT("Bip001-L-Thigh")));
	ArmBones.Add(FName(TEXT("Bip001-L-Calf")));
	ArmBones.Add(FName(TEXT("Bip001-L-Foot")));
}

bool APBCharacter::IsHeadShot(FDamageEvent const& DamageEvent)
{
	if (DamageEvent.GetTypeID() == FPointDamageEvent::ClassID)
	{
		FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)(&DamageEvent);
		if (HeadBones.Contains(PointDamageEvent->HitInfo.BoneName))
		{
			return true;
		}		
	}

	return false;
}

bool APBCharacter::IsBodyShot(FDamageEvent const& DamageEvent)
{
	if (DamageEvent.GetTypeID() == FPointDamageEvent::ClassID)
	{
		FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)(&DamageEvent);
		if (BodyBones.Contains(PointDamageEvent->HitInfo.BoneName))
		{
			return true;
		}
	}

	return false;
}

bool APBCharacter::IsArmShot(FDamageEvent const& DamageEvent)
{
	if (DamageEvent.GetTypeID() == FPointDamageEvent::ClassID)
	{
		FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)(&DamageEvent);
		if (ArmBones.Contains(PointDamageEvent->HitInfo.BoneName))
		{
			return true;
		}
	}

	return false;
}

bool APBCharacter::IsLegShot(FDamageEvent const& DamageEvent)
{
	if (DamageEvent.GetTypeID() == FPointDamageEvent::ClassID)
	{
		FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)(&DamageEvent);
		if (LegBones.Contains(PointDamageEvent->HitInfo.BoneName))
		{
			return true;
		}
	}

	return false;
}

bool APBCharacter::IsFallOff(FDamageEvent const& DamageEvent)
{
	if (DamageEvent.DamageTypeClass == UPBFallOffDamageType::StaticClass())
	{
		return true;
	}

	return false;
}

float APBCharacter::ModifyDamageByHitComp(float OriginalDamage, FDamageEvent const& DamageEvent, AController* InstigatorController)
{
	// Original damage for bot
	if (!Cast<APlayerController>(InstigatorController))
	{
		return OriginalDamage;
	}

	if (IsHeadShot(DamageEvent))
	{
		return OriginalDamage * (1 + HeadShotDmgAdjust);
	}
	else if (IsBodyShot(DamageEvent))
	{
		return OriginalDamage * (1 + BodyShotDmgAdjust);
	}
	else if (IsArmShot(DamageEvent))
	{
		return OriginalDamage * (1 + ArmShotDmgAdjust);
	}
	else if (IsLegShot(DamageEvent))
	{
		return OriginalDamage * (1 + LegShotDmgAdjust);
	}
	else
	{
		return OriginalDamage;
	}
}

float APBCharacter::GetBackstabDmgAdjust(FDamageEvent const& DamageEvent)
{
	if (DamageEvent.GetTypeID() == FPointDamageEvent::ClassID)
	{
		FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)(&DamageEvent);
		FVector HitDir = (PointDamageEvent->HitInfo.TraceEnd - PointDamageEvent->HitInfo.TraceStart).GetSafeNormal();
		return FMath::Max(FVector::DotProduct(HitDir, GetCapsuleComponent()->GetForwardVector()) * MaxBackstabDamage, 0.f);
	}

	return 0.f;
}

void APBCharacter::InitBackstabDamageSys()
{
	MaxBackstabDamage = 0.5f;
}

float APBCharacter::ModifyDamageForBackstab(float OriginalDamage, FDamageEvent const& DamageEvent, AController* InstigatorController)
{
	// Original damage for bot
	if (!Cast<APlayerController>(InstigatorController))
	{
		return OriginalDamage;
	}

	return OriginalDamage * (1 + GetBackstabDmgAdjust(DamageEvent));
}

void APBCharacter::InitDeathImpulse()
{
	DefaultImpulseReceivingBone = FName(TEXT("Bip001-Head"));

	ImpulseReceivingBones.Add(FBoneImpulseReceptor(FName(TEXT("Bip001-Head")), 1.f));
	ImpulseReceivingBones.Add(FBoneImpulseReceptor(FName(TEXT("Bip001-Pelvis")), 2.f));
	ImpulseReceivingBones.Add(FBoneImpulseReceptor(FName(TEXT("Bip001-L-Hand")), 0.5f));
	ImpulseReceivingBones.Add(FBoneImpulseReceptor(FName(TEXT("Bip001-R-Hand")), 0.5f));
	ImpulseReceivingBones.Add(FBoneImpulseReceptor(FName(TEXT("Bip001-L-Foot")), 0.5f));
	ImpulseReceivingBones.Add(FBoneImpulseReceptor(FName(TEXT("Bip001-R-Foot")), 0.5f));
}

void APBCharacter::ProcessDeathRagdollImpulse()
{
	APawn* PawnInstigator = LastTakeHitInfo.PawnInstigator.Get();
	UPBDamageType* DamageType = Cast<UPBDamageType>(LastTakeHitInfo.GetDamageEvent().DamageTypeClass->GetDefaultObject());
	if (GetMesh3P() && DamageType && PawnInstigator)
	{
		if (DamageType->DamageImpulse > 0)
		{
			if (LastTakeHitInfo.PointDamageDataValid())
			{
				UPBPointDamageType* PointDamageType = Cast<UPBPointDamageType>(DamageType);
				if (PointDamageType)
				{
					FVector DeathImpulse = FVector::ZeroVector;

					DeathImpulse +=
						PointDamageType->ImpulseMultiplier_ShootDir *
						DamageType->DeathImpulseOverNormalImpulse * DamageType->DamageImpulse
						* (GetActorLocation() - LastTakeHitInfo.GetPointDamageEvent().HitInfo.TraceStart).GetSafeNormal();

					DeathImpulse +=
						PointDamageType->ImpulseMultiplier_HitDir *
						DamageType->DeathImpulseOverNormalImpulse * DamageType->DamageImpulse
						* LastTakeHitInfo.GetPointDamageEvent().ShotDirection;

					DeathImpulse +=
						DamageType->DeathJumpImpulseSize * FVector(0, 0, 1);

					if (LastTakeHitInfo.GetPointDamageEvent().HitInfo.BoneName != NAME_None)
					{
						GetMesh3P()->AddImpulse(DeathImpulse, LastTakeHitInfo.GetPointDamageEvent().HitInfo.BoneName, true);
					}
					else
					{
						GetMesh3P()->AddImpulse(DeathImpulse, DefaultImpulseReceivingBone, true);
					}
					return;
				}
			}
			
			if (LastTakeHitInfo.RadialDamageDataValid())
			{
				UPBRadialDamageType* RadialDamageType = Cast<UPBRadialDamageType>(DamageType);
				if (RadialDamageType)
				{
					for (auto It = ImpulseReceivingBones.CreateIterator(); It; ++It)
					{
						FVector ExplosionToBoneVector = GetMesh3P()->GetBoneLocation(It->BoneName) - LastTakeHitInfo.GetRadialDamageEvent().Origin;

						float DistanceFromOuterRadius = FMath::Max(1.f - (ExplosionToBoneVector.Size() / LastTakeHitInfo.GetRadialDamageEvent().Params.OuterRadius), 0.f);

						FVector ImpulseToApply = 
							FMath::Pow(DistanceFromOuterRadius, LastTakeHitInfo.GetRadialDamageEvent().Params.DamageFalloff)
							* DamageType->DamageImpulse * ExplosionToBoneVector.GetSafeNormal();

						GetMesh3P()->AddImpulse(DamageType->DeathImpulseOverNormalImpulse * ImpulseToApply * It->ImpulseReceptionWeight, It->BoneName, true);
					}

					{
						FVector ExplosionToDefaultBoneVector = GetMesh3P()->GetBoneLocation(DefaultImpulseReceivingBone) - LastTakeHitInfo.GetRadialDamageEvent().Origin;

						float DistanceFromOuterRadius = FMath::Max(1.f - (ExplosionToDefaultBoneVector.Size() / LastTakeHitInfo.GetRadialDamageEvent().Params.OuterRadius), 0.f);

						FVector ImpulseToApply =
							FMath::Pow(DistanceFromOuterRadius, LastTakeHitInfo.GetRadialDamageEvent().Params.DamageFalloff)
							* RadialDamageType->ExtraDeathJumpImpulseFromDistance * FVector(0, 0, 1);

						GetMesh3P()->AddImpulse(ImpulseToApply * FVector(0, 0, 1), DefaultImpulseReceivingBone, true);
					}

					GetMesh3P()->AddImpulse(DamageType->DeathJumpImpulseSize * FVector(0, 0, 1), DefaultImpulseReceivingBone, true);
					return;
				}
			}

			// If we cannot process the impulse using point/radial damage data
			// We perform the default impulse processing
			{
				FVector DeathImpulse = FVector::ZeroVector;

				DeathImpulse += DamageType->DeathImpulseOverNormalImpulse * DamageType->DamageImpulse
					* (GetActorLocation() - PawnInstigator->GetActorLocation()).GetSafeNormal();

				DeathImpulse += DamageType->DeathJumpImpulseSize * FVector(0, 0, 1);

				GetMesh3P()->AddImpulse(DeathImpulse, DefaultImpulseReceivingBone, true);
			}
		}
	}
}

APBMission* APBCharacter::GetOwnMission()
{
	if (!GetWorld()) return nullptr;

	APBGameState* GS = Cast<APBGameState>(GetWorld()->GetGameState());
	if (!GS)
	{
		return nullptr;
	}

	APBPlayerState* PS = Cast<APBPlayerState>(PlayerState);
	if (!PS)
	{
		return nullptr;
	}

	return GS->GetTeamMission(PS->GetTeamType());
}

void APBCharacter::InitBombPlanting()
{
	BombsitePresenceCount = 0;
	bIsPlantingBomb = false;
}

APBWeapon* APBCharacter::FindBombFromInventory()
{
	return GetWeapon(BombMissionWeaponSlot, BombMissionBombSlotIdx, EWeaponMode::Mode1);
}

void APBCharacter::SwitchToBombWeapon()
{
	SwitchWeapon(BombMissionWeaponSlot, BombMissionBombSlotIdx, EWeaponMode::Mode1);
}

void APBCharacter::NotifyEnterBombSite(const APBTrigger_BombSite *BombSite)
{
	if (AimToPlantBomb())
	{
		++BombsitePresenceCount;
		OnRep_BombsitePresenceCount(BombsitePresenceCount - 1);

		OverlappedBombSite = BombSite;
	}
}

void APBCharacter::NotifyLeaveBombSite()
{
	if (AimToPlantBomb())
	{
		--BombsitePresenceCount;
		OnRep_BombsitePresenceCount(BombsitePresenceCount + 1);
	}

	OverlappedBombSite = nullptr;
}

bool APBCharacter::AimToPlantBomb()
{
	return Cast<APBMission_BombMissionAttack>(GetOwnMission()) != nullptr;
}

void APBCharacter::OnRep_BombsitePresenceCount(int OriginalValue)
{
	if (IsLocallyControlled())
	{
		if (BombsitePresenceCount >= 1 && OriginalValue == 0)
		{
			APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
			if (MyPC)
			{
				MyPC->WidgetEventDispatcher->OnCanPlantBomb.Broadcast(true);
			}
		}

		if (BombsitePresenceCount == 0 && OriginalValue >= 1)
		{
			APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
			if (MyPC)
			{
				MyPC->WidgetEventDispatcher->OnCanPlantBomb.Broadcast(false);
			}
		}
	}
}

bool APBCharacter::IsInTimePlantableBomb()
{
	if (IsPlantingBomb())
		return true;

	APBWeap_Bomb* Bomb = Cast<APBWeap_Bomb>(FindBombFromInventory());
	if (!Bomb)
	{
		return false;
	}

	auto GS = GetWorld() ? Cast<APBGameState>(GetWorld()->GetGameState()) : nullptr;
	if (GS == nullptr)
	{
		return false;
	}		

	int32 RemainingTime = GS->RemainingTime;

	TSubclassOf<class APBBulletBase> BulletClass = Bomb->GetWeaponAttachment()->GetBulletClass(EWeaponPoint::Point_R);
	if (BulletClass == nullptr)
	{
		return false;
	}

	APBProj_Bomb *ProjBomb = BulletClass->GetDefaultObject<APBProj_Bomb>();
	if (ProjBomb == nullptr)
	{
		return false;
	}

	FProjectileData ProjData = ProjBomb->GetProjectileData();
	int32 ProjectileLife = ProjData.ProjectileLife;
	int32 BombEquipDuration = Bomb->GetEquipDuration();
	bool IsInPlantableTime = RemainingTime > (BombEquipDuration + ProjectileLife);

	return IsInPlantableTime;
}

bool APBCharacter::CanPlantBomb()
{
	APBWeap_Bomb* Bomb = Cast<APBWeap_Bomb>(FindBombFromInventory());
	APBMission_BombMissionAttack* AttackMission = Cast<APBMission_BombMissionAttack>(GetOwnMission());

	return (Bomb && AttackMission && AttackMission->CanPlantBomb() && BombsitePresenceCount > 0 && IsInTimePlantableBomb());
}

float APBCharacter::GetBombPlantingTime()
{
	APBWeap_Bomb* BombWeap = Cast<APBWeap_Bomb>(FindBombFromInventory());
	if (BombWeap)
	{
		return BombWeap->GetEquipDuration();
	}

	return 0.f;
}

void APBCharacter::OnStartBombPlanting()
{
	APBPlayerController* PC = Cast<APBPlayerController>(Controller);
	if (PC->IsGameInputAllowed() && CanPlantBomb())
	{
		if (!bIsPlantingBomb)
		{
			if (IsCrouching())
			{
				OnUnCrouch();
			}

			bIsPlantingBomb = true;
			if (PC)
			{
				PC->WidgetEventDispatcher->OnPlantingBomb.Broadcast(GetBombPlantingTime());
			}

			StashedWeaponSlot = CurrentWeaponSlot;
			StashedWeaponSlot = CurrentWeaponSlot;
			StashedWeaponMode = CurrentWeaponMode;

			SwitchToBombWeapon();

			GetWorldTimerManager().SetTimer(Handle_UpdateBombPlanting, this, &APBCharacter::UpdateBombPlanting, 0.2, true);

			if (GetNetMode() != NM_DedicatedServer)
			{
				if (BombPlantActionSound && nullptr == BombPlantActionComponent)
				{
					BombPlantActionComponent = UPBGameplayStatics::SpawnSoundAtLocation(GetWorld(), this, BombPlantActionSound, GetActorLocation());
				}
			}
		}
	}
}

void APBCharacter::UpdateBombPlanting()
{
	if (Cast<APBWeap_Bomb>(CurrentWeapon) && CurrentWeapon->IsEquipped())
	{
		StartWeaponFire(EFireTrigger::Fire1);
	}

	if (!CanPlantBomb())
	{
		OnEndBombPlanting();
	}
}

void APBCharacter::OnEndBombPlanting()
{
	if (bIsPlantingBomb)
	{
		StopWeaponFire(EFireTrigger::Fire1);
		SwitchWeapon(StashedWeaponSlot, StashedWeaponSlotIdx, StashedWeaponMode);

		bIsPlantingBomb = false;
		APBPlayerController* PC = Cast<APBPlayerController>(Controller);
		if (PC)
		{
			PC->WidgetEventDispatcher->OnBombPlantingEnd.Broadcast();
		}

		GetWorldTimerManager().ClearTimer(Handle_UpdateBombPlanting);

		if (GetNetMode() != NM_DedicatedServer)
		{
			if (BombPlantActionComponent)
			{
				BombPlantActionComponent->Stop();
				BombPlantActionComponent = nullptr;
			}
		}
	}
}

bool APBCharacter::IsPlantingBomb()
{
	return bIsPlantingBomb;
}

void APBCharacter::OnStartPlantBomb()
{
	if (bShiftDebugViewInput)
		return;

	if (Cast<APBMission_BombMissionAttack>(GetOwnMission()))
	{
		OnStartBombPlanting();
	}
}

void APBCharacter::OnEndPlantBomb()
{
	if (Cast<APBMission_BombMissionAttack>(GetOwnMission()))
	{
		OnEndBombPlanting();
	}
}

void APBCharacter::InitBombDefusing()
{
	bIsDefusingBomb = false;
}

APBWeapon* APBCharacter::FindDefuseKitFromInventory()
{
	return GetWeapon(BombMissionWeaponSlot, BombMissionDefuseKitSlotIdx, EWeaponMode::Mode1);
}

void APBCharacter::SwitchToDefuseKitWeapon()
{
	SwitchWeapon(BombMissionWeaponSlot, BombMissionDefuseKitSlotIdx, EWeaponMode::Mode1);
}

const APBTrigger_BombSite *APBCharacter::GetOverlappedBombSite() const
{
	return OverlappedBombSite;
}

void APBCharacter::NotifyEnterBombRadius(APBProj_Bomb* Bomb)
{
	if (AimToDefuseBomb())
	{
		APBWeap_DefuseKit* DefuseKit = Cast<APBWeap_DefuseKit>(FindDefuseKitFromInventory());
		if (DefuseKit)
		{
			// Only care about the first bomb we get in contact with
			if (!DefuseKit->GetTargetBomb())
			{
				DefuseKit->SetTargetBomb(Bomb);
			}
		}
	}
}

void APBCharacter::NotifyLeaveBombRadius(APBProj_Bomb* Bomb)
{
	if (AimToDefuseBomb())
	{
		APBWeap_DefuseKit* DefuseKit = Cast<APBWeap_DefuseKit>(FindDefuseKitFromInventory());
		if (DefuseKit)
		{
			if (DefuseKit->GetTargetBomb() == Bomb)
			{
				DefuseKit->SetTargetBomb(nullptr);
			}
		}

	}
}

bool APBCharacter::AimToDefuseBomb()
{
	return Cast<APBMission_BombMissionDefense>(GetOwnMission()) != nullptr;
}

bool APBCharacter::CanDefuseBomb()
{
	APBWeap_DefuseKit* DefuseKit = Cast<APBWeap_DefuseKit>(FindDefuseKitFromInventory());
	if (DefuseKit)
	{		
		return (DefuseKit->CanDefuseBomb());
	}

	return false;
}

float APBCharacter::GetBombDefusingTime()
{
	APBWeap_DefuseKit* DefuseKit = Cast<APBWeap_DefuseKit>(FindDefuseKitFromInventory());
	if (DefuseKit)
	{
		return DefuseKit->GetEquipDuration();
	}

	return 0.f;
}

void APBCharacter::OnStartBombDefusing()
{
	APBPlayerController* PC = Cast<APBPlayerController>(Controller);
	if (PC->IsGameInputAllowed() && CanDefuseBomb())
	{
		if (!bIsDefusingBomb)
		{
			if (IsCrouching())
			{
				OnUnCrouch();
			}

			bIsDefusingBomb = true;
			if (PC)
			{
				PC->WidgetEventDispatcher->OnDefusingBomb.Broadcast(GetBombDefusingTime());
			}

			StashedWeaponSlot = CurrentWeaponSlot;
			StashedWeaponSlot = CurrentWeaponSlot;
			StashedWeaponMode = CurrentWeaponMode;

			SwitchToDefuseKitWeapon();

			GetWorldTimerManager().SetTimer(Handle_UpdateBombPlanting, this, &APBCharacter::UpdateBombDefusing, 0.2, true);

			if (GetNetMode() != NM_DedicatedServer)
			{
				if (BombDefuseActionSound && nullptr == BombDefuseActionComponent)
				{
					BombDefuseActionComponent = UPBGameplayStatics::SpawnSoundAtLocation(GetWorld(), this, BombDefuseActionSound, GetActorLocation());
				}
			}
		}
	}
}

void APBCharacter::UpdateBombDefusing()
{
	if (Cast<APBWeap_DefuseKit>(CurrentWeapon) && CurrentWeapon->IsEquipped())
	{
		StartWeaponFire(EFireTrigger::Fire1);
	}

	if (!CanDefuseBomb())
	{
		OnEndBombDefusing();
	}
}

void APBCharacter::OnEndBombDefusing()
{
	if (bIsDefusingBomb)
	{
		StopWeaponFire(EFireTrigger::Fire1);
		SwitchWeapon(StashedWeaponSlot, StashedWeaponSlotIdx, StashedWeaponMode);

		bIsDefusingBomb = false;
		APBPlayerController* PC = Cast<APBPlayerController>(Controller);
		if (PC)
		{
			PC->WidgetEventDispatcher->OnBombDefusingEnd.Broadcast();
		}

		GetWorldTimerManager().ClearTimer(Handle_UpdateBombDefusing);

		if (BombDefuseActionComponent)
		{
			BombDefuseActionComponent->Stop();
			BombDefuseActionComponent = nullptr;
		}
	}
}

bool APBCharacter::IsDefusingBomb()
{
	return bIsDefusingBomb;
}

void APBCharacter::OnStartDefuseBomb()
{
	if (bShiftDebugViewInput)
		return;

	if (Cast<APBMission_BombMissionDefense>(GetOwnMission()))
	{
		OnStartBombDefusing();
	}
}

void APBCharacter::OnEndDefuseBomb()
{
	if (Cast<APBMission_BombMissionDefense>(GetOwnMission()))
	{
		OnEndBombDefusing();
	}
}

// request Crouch
void APBCharacter::OnCrouch()
{
	//make sure check the CanCrouch in Blueprint Details view (CharacterMovement -> NavMovement -> MovementCapabilities -> CanCrouch checkbox)
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed() && !IsPlantingBomb() && !IsDefusingBomb())
	{
		GetCharacterMovement()->bWantsToCrouch = true;
	}
}

// request UnCrouch
void APBCharacter::OnUnCrouch()
{
	//make sure check the CanCrouch in Blueprint Details view (CharacterMovement -> NavMovement -> MovementCapabilities -> CanCrouch checkbox)
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed() && !IsPlantingBomb() && !IsDefusingBomb())
	{
		GetCharacterMovement()->bWantsToCrouch = false;
	}
}

void APBCharacter::OnCrouchToggle()
{
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed() && !IsPlantingBomb() && !IsDefusingBomb() && !IsSprinting())
	{
		GetCharacterMovement()->bWantsToCrouch = !GetCharacterMovement()->IsCrouching();
	}
}

bool APBCharacter::IsCrouching() const
{
	return GetCharacterMovement()->IsCrouching();
}

void APBCharacter::PlaySoundMissionStart()
{
	if (MissionStartSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), MissionStartSound);
	}
}

void APBCharacter::UpdateSprintSounds(bool bNewSprinting)
{
	bool bVelocityIsNearlyZero = GetCharacterMovement()->Velocity.IsNearlyZero();
	if (bVelocityIsNearlyZero)
	{
		if (RunLoopAC)
		{
			RunLoopAC->Stop();
		}

		return;
	}

	if (bNewSprinting)
	{
		if (!RunLoopAC && RunLoopSound)
		{
			RunLoopAC = UPBGameplayStatics::SpawnSoundAttached(RunLoopSound, GetRootComponent());
			if (RunLoopAC)
			{
				RunLoopAC->bAutoDestroy = false;
			}
			
		}
		else if (RunLoopAC)
		{
			RunLoopAC->Play();
		}
	}
	else
	{
		if (RunLoopAC)
		{
			RunLoopAC->Stop();
		}

		if (RunStopSound)
		{
			UPBGameplayStatics::SpawnSoundAttached(RunStopSound, GetRootComponent());
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Animations

void APBCharacter::OnRep_PlayAnimation()
{
	if (SyncPlayAnimData.AnimMontage == nullptr)
	{
		return;
	}

	if (SyncPlayAnimData.bPlay)
	{
		PlayAnimMontage(SyncPlayAnimData.AnimMontage, 1.f, SyncPlayAnimData.SectionName);
	}
	else
	{
		if (SyncPlayAnimData.SectionName != NAME_None)
		{
			MontageJumpToSection(SyncPlayAnimData.AnimMontage, SyncPlayAnimData.SectionName);
		}			
		else
		{
			StopAnimMontage(SyncPlayAnimData.AnimMontage);
		}			
	}	
}

bool APBCharacter::ServerPlayAnimation_Validate(class UAnimMontage* AnimMontage, float InPlayRate /*= 1.f*/, FName StartSectionName /*= NAME_None*/)
{
	return true;
}

void APBCharacter::ServerPlayAnimation_Implementation(class UAnimMontage* AnimMontage, float InPlayRate /*= 1.f*/, FName StartSectionName /*= NAME_None*/)
{
	float const Duration = PlayAnimMontage(AnimMontage, InPlayRate, StartSectionName);

	if (Duration > 0.f)
	{
		SyncPlayAnimData.AnimMontage = AnimMontage;
		SyncPlayAnimData.bPlay = true;
		SyncPlayAnimData.SectionName = StartSectionName;
		SyncPlayAnimData.RepCounter++;
	}
}

bool APBCharacter::ServerStopAnimation_Validate(class UAnimMontage* AnimMontage, bool GotoEndSection)
{
	return true;
}

void APBCharacter::ServerStopAnimation_Implementation(class UAnimMontage* AnimMontage, bool GotoEndSection)
{
	FName StartSectionName = GotoEndSection ? FName(TEXT("End")) : NAME_None;

	if (StartSectionName != NAME_None)
	{
		MontageJumpToSection(AnimMontage, StartSectionName);
	}
	else
	{
		StopAnimMontage(AnimMontage);
	}	

	SyncPlayAnimData.AnimMontage = AnimMontage;
	SyncPlayAnimData.bPlay = false;
	SyncPlayAnimData.SectionName = StartSectionName;
	SyncPlayAnimData.RepCounter++;
}

float APBCharacter::PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName) 
{
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	UAnimInstance * AnimInstance = (UseMesh) ? UseMesh->GetAnimInstance() : nullptr;
	if (AnimMontage && UseMesh && AnimInstance)
	{
		float const Duration = AnimInstance->Montage_Play(AnimMontage, InPlayRate);

		if (Duration > 0.f)
		{
			// Start at a given Section.
			if (StartSectionName != NAME_None)
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, AnimMontage);
			}

			return Duration;
		}
	}

	return 0.0f;
}

void APBCharacter::StopAnimMontage(class UAnimMontage* AnimMontage)
{
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	UAnimInstance * AnimInstance = (UseMesh) ? UseMesh->GetAnimInstance() : nullptr;
	if (AnimMontage && UseMesh && AnimInstance && AnimInstance->Montage_IsPlaying(AnimMontage))
	{
		AnimInstance->Montage_Stop(AnimMontage->BlendOut.GetBlendTime());
	}
}

void APBCharacter::StopAllAnimMontages()
{
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	UAnimInstance * AnimInstance = (UseMesh) ? UseMesh->GetAnimInstance() : nullptr;
	if (UseMesh && AnimInstance)
	{
		AnimInstance->Montage_Stop(0.0f);
	}
}

void APBCharacter::MontageJumpToSection(class UAnimMontage* AnimMontage, FName StartSectionName)
{
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	UAnimInstance * AnimInstance = (UseMesh) ? UseMesh->GetAnimInstance() : nullptr;
	if (AnimMontage && UseMesh && AnimInstance && AnimInstance->Montage_IsPlaying(AnimMontage))
	{
		// Start at a given Section.
		if (StartSectionName != NAME_None)
		{
			AnimInstance->Montage_JumpToSection(StartSectionName, AnimMontage);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void APBCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	RegisterLookAroundHandlers();

	PlayerInputComponent->BindAxis("MoveForward", this, &APBCharacter::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APBCharacter::OnMoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &APBCharacter::OnMoveUp);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &APBCharacter::OnStartWalking);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &APBCharacter::OnStopWalking);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APBCharacter::OnStartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APBCharacter::OnStopSprinting);
	PlayerInputComponent->BindAction("SprintToggle", IE_Pressed, this, &APBCharacter::OnSprintToggle);

	// mouse left, right
	PlayerInputComponent->BindAction("FireTrigger1", IE_Pressed, this, &APBCharacter::OnStartFireTrigger1);
	PlayerInputComponent->BindAction("FireTrigger1", IE_Released, this, &APBCharacter::OnEndFireTrigger1);
	PlayerInputComponent->BindAction("FireTrigger2", IE_Pressed,	this, &APBCharacter::OnStartFireTrigger2);
	PlayerInputComponent->BindAction("FireTrigger2", IE_Released, this, &APBCharacter::OnEndFireTrigger2);
	
	PlayerInputComponent->BindAction("MeleeAttack", IE_Pressed, this, &APBCharacter::OnStartMeleeAttackTrigger);
	PlayerInputComponent->BindAction("MeleeAttack", IE_Released, this, &APBCharacter::OnEndMeleeAttackTrigger);

	PlayerInputComponent->BindAction("QuickThrowing", IE_Pressed, this, &APBCharacter::HandleStartQuickThrow);
	PlayerInputComponent->BindAction("QuickThrowing", IE_Released, this, &APBCharacter::OnStopQuickThrowTrigger);

	PlayerInputComponent->BindAction("SwitchQuickWeapon", IE_Released, this, &APBCharacter::OnSwitchQuickWeapon);

	PlayerInputComponent->BindAction("WeapFuncTrigger", IE_Pressed, this, &APBCharacter::OnWeapFuncTrigger);

	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this,	&APBCharacter::OnNextWeapon);
	PlayerInputComponent->BindAction("PrevWeapon", IE_Pressed, this,	&APBCharacter::OnPrevWeapon);

	PlayerInputComponent->BindAction("PC_PrimaryWeapon", IE_Pressed, this,	&APBCharacter::OnSwitchWeapon);
	PlayerInputComponent->BindAction("PC_SecondaryWeapon", IE_Pressed, this, &APBCharacter::OnSwitchWeapon);
	PlayerInputComponent->BindAction("PC_MeleeWeapon", IE_Pressed, this,	&APBCharacter::OnSwitchWeapon);
	PlayerInputComponent->BindAction("PC_GrenadeWeapon", IE_Pressed, this,	&APBCharacter::OnSwitchWeapon);
	PlayerInputComponent->BindAction("PC_SpecialWeapon", IE_Pressed, this,	&APBCharacter::OnSwitchWeapon);
	PlayerInputComponent->BindAction("PC_SpecialWeapon2", IE_Pressed, this,	&APBCharacter::OnSwitchWeapon);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APBCharacter::OnReload);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APBCharacter::OnStartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APBCharacter::OnStopJump);
	
	//PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APBCharacter::OnCrouch);
	//PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APBCharacter::OnUnCrouch);	
	PlayerInputComponent->BindAction("CrouchToggle", IE_Pressed, this, &APBCharacter::OnCrouchToggle);


	PlayerInputComponent->BindAction("PlantBomb", IE_Pressed, this, &APBCharacter::OnStartPlantBomb);
	PlayerInputComponent->BindAction("PlantBomb", IE_Released, this, &APBCharacter::OnEndPlantBomb);

	PlayerInputComponent->BindAction("DefuseBomb", IE_Pressed, this, &APBCharacter::OnStartDefuseBomb);
	PlayerInputComponent->BindAction("DefuseBomb", IE_Released, this, &APBCharacter::OnEndDefuseBomb);

	PlayerInputComponent->BindAction("FollowPreviousPlayer", IE_Released, this, &APBCharacter::OnFollowPreviousPlayer);
	PlayerInputComponent->BindAction("FollowNextPlayer", IE_Released, this, &APBCharacter::OnFollowNextPlayer);

	//@note: for debugging
	//InputComponent->BindAction("ShowPlayerDebugContexts", IE_Released, this, &APBCharacter::OnShowPlayerDebugContexts);

	PlayerInputComponent->BindAction("ShiftDebugViewInput", IE_Pressed, this, &APBCharacter::OnPress_ShiftDebugViewInput);
	PlayerInputComponent->BindAction("ShiftDebugViewInput", IE_Released, this, &APBCharacter::OnRelease_ShiftDebugViewInput);

	PlayerInputComponent->BindAction("ChangePreviousView_Debug", IE_Released, this, &APBCharacter::OnChangePreviousView_Debug);
	PlayerInputComponent->BindAction("ChangeNextView_Debug", IE_Released, this, &APBCharacter::OnChangeNextView_Debug);
	PlayerInputComponent->BindAction("ChangeViewOnSpectator", IE_Released, this, &APBCharacter::OnChangeViewOnSpectator);
	PlayerInputComponent->BindAction("ResetToFPView", IE_Released, this, &APBCharacter::OnResetToFPView);

	PlayerInputComponent->BindAxis("DebugViewZoom", this, &APBCharacter::OnDebugViewZoom);


	// Pickup 
	PlayerInputComponent->BindAction("Pickup", IE_Pressed, this, &APBCharacter::OnPressPickup);
	PlayerInputComponent->BindAction("Pickup", IE_Released, this, &APBCharacter::OnReleasePickup);


}

//void APBCharacter::OnShowPlayerDebugContexts()
//{
//}


void APBCharacter::OnPress_ShiftDebugViewInput() { bShiftDebugViewInput = true; }
void APBCharacter::OnRelease_ShiftDebugViewInput() { bShiftDebugViewInput = false; }

void APBCharacter::ChangeView(EPBCameraStyle CameraStyle)
{
	if (GetNetMode() == ENetMode::NM_DedicatedServer && false == IsLocallyControlled())
		return;

	if (GEngine)
	{
		FString CameraStyleName = EnumToString(EPBCameraStyle, CameraStyle);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, CameraStyleName);
	}

	APBCharacter *FollowingTarget = TargetViewPlayer ? TargetViewPlayer->OwnerCharacter : this;
	bIsViewOwnself = (FollowingTarget == this);

	if (CameraStyle == EPBCameraStyle::FPS)
	{
		EnterFPSCamMode(FollowingTarget);
	}
	else if (CameraStyle == EPBCameraStyle::TPS)
	{
		EnterTPSCamMode(FollowingTarget);
	}
	else if (CameraStyle == EPBCameraStyle::DebugFreeCam)
	{
		EnterDebugFreeCamMode(FollowingTarget);
	}
	else if (CameraStyle == EPBCameraStyle::DebugSphericalCam)
	{
		EnterDebugSphericalCamMode(FollowingTarget);
	}
}

EPBCameraStyle APBCharacter::GetMinCameraStyle() const
{
	if (IsViewingOwnself())
	{
#if WITH_EDITOR
		return EPBCameraStyle::FPS;
#else
		return EPBCameraStyle::FPS;
#endif
	}

#if WITH_EDITOR
	//@note: 테스트용도.
	return EPBCameraStyle::FPS;
#else
	//@note: 아직 FPS뷰가 안정적이지 못함.
	return EPBCameraStyle::TPS;
#endif
}

EPBCameraStyle APBCharacter::GetMaxCameraStyle() const
{
#if WITH_EDITOR
	return EPBCameraStyle::DebugSphericalCam;
#else
	return EPBCameraStyle::TPS;
#endif
}

void APBCharacter::OnChangePreviousView_Debug()
{
#if WITH_EDITOR
	//if (!bShiftDebugViewInput)
	//	return;

	auto World = GetWorld();
	if (nullptr == World)
		return;

	if (IsNetMode(ENetMode::NM_DedicatedServer) && false == IsLocallyControlled())
		return;

	auto PCM = Cast<APBPlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(World, 0));
	int32 PrevCameraStyle = (int32)(PCM->GetCameraStyle()) - 1;
	if (PrevCameraStyle < (int32)(EPBCameraStyle::DebugFreeCam))
	{
		PrevCameraStyle = (int32)(EPBCameraStyle::DebugSphericalCam);
	}

	ChangeView((EPBCameraStyle)(PrevCameraStyle));
#endif
}

void APBCharacter::OnChangeNextView_Debug()
{
#if WITH_EDITOR
	//if (!bShiftDebugViewInput)
	//	return;

	auto World = GetWorld();
	if (nullptr == World)
		return;

	if (IsNetMode(ENetMode::NM_DedicatedServer) && false == IsLocallyControlled())
		return;

	auto PCM = Cast<APBPlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(World, 0));
	int32 NextCameraStyle = (int32)(PCM->GetCameraStyle()) + 1;
	if (NextCameraStyle > (int32)(EPBCameraStyle::DebugSphericalCam))
	{
		NextCameraStyle = (int32)(EPBCameraStyle::DebugFreeCam);
	}

	ChangeView((EPBCameraStyle)(NextCameraStyle));
#endif
}

void APBCharacter::OnChangeViewOnSpectator()
{
	auto World = GetWorld();
	if (nullptr == World)
		return;

	if (IsViewingOwnself())
		return;

	if (IsNetMode(ENetMode::NM_DedicatedServer) && false == IsLocallyControlled())
		return;

	if (false == IsCameraStyle(EPBCameraStyle::FPS))
	{
		ChangeView(EPBCameraStyle::FPS);
	}
	else
	{
		ChangeView(EPBCameraStyle::TPS);
	}
}

void APBCharacter::OnResetToFPView()
{
	ChangeView(EPBCameraStyle::FPS);
}

void APBCharacter::OnDebugViewZoom(float Val)
{
#if WITH_EDITOR
	APBCharacter *FollowingTarget = TargetViewPlayer ? TargetViewPlayer->OwnerCharacter : this;
	
	if (!bShiftDebugViewInput)
		return;

	const float Sensitive = 50.0f;

	if (IsCameraStyle(EPBCameraStyle::DebugSphericalCam) && FollowingTarget)
	{
		float DebugSphericalCamSpringArmLength = FollowingTarget->DebugSphericalCamSpringArm->TargetArmLength;
		FollowingTarget->DebugSphericalCamSpringArm->TargetArmLength = FMath::Clamp(DebugSphericalCamSpringArmLength + Val * Sensitive, 100.0f, 500.0f);
	}
#endif
}

void APBCharacter::OnMoveForward(float Val)
{
	if (bShiftDebugViewInput)
	{
		APBCharacter *FollowingTarget = TargetViewPlayer ? TargetViewPlayer->OwnerCharacter : this;
		if (nullptr == FollowingTarget)
			return;

		const float Sensitive = 50.0f;
		
		if (IsCameraStyle(EPBCameraStyle::DebugFreeCam))
		{
			FollowingTarget->DebugFreeCamComp->SetWorldLocation(FollowingTarget->DebugFreeCamComp->GetComponentLocation() + FollowingTarget->DebugFreeCamComp->GetForwardVector() * Val * Sensitive);

			return;
		}
	}

	if (Controller && Val != 0.f && !IsPlantingBomb() && !IsDefusingBomb())
	{
		// Limit pitch when walking or falling
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());
		const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Val);
	}

	ConditionalStopSprint(Val <= 0.f);
}

void APBCharacter::ConditionalStopSprint(bool bForceStop)
{
	if (bForceStop || !CanSprint())
	{
		if (bWantsToSprint)
		{
			OnStopSprinting();
		}
	}
}

void APBCharacter::OnMoveRight(float Val)
{
	if (bShiftDebugViewInput)
	{
		APBCharacter *FollowingTarget = TargetViewPlayer ? TargetViewPlayer->OwnerCharacter : this;
		if (nullptr == FollowingTarget)
			return;

		const float Sensitive = 50.0f;
		
		if (IsCameraStyle(EPBCameraStyle::DebugFreeCam))
		{
			FollowingTarget->DebugFreeCamComp->SetWorldLocation(FollowingTarget->DebugFreeCamComp->GetComponentLocation() + FollowingTarget->DebugFreeCamComp->GetRightVector() * Val * Sensitive);

			return;
		}
	}

	if (Val != 0.f && !IsPlantingBomb() && !IsDefusingBomb())
	{
		const FQuat Rotation = GetActorQuat();
		const FVector Direction = FQuatRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Val);
	}
}

void APBCharacter::OnMoveUp(float Val)
{
	if (Val != 0.f && !IsPlantingBomb() && !IsDefusingBomb())
	{
		// Not when walking or falling.
		if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
		{
			return;
		}

		AddMovementInput(FVector::UpVector, Val);
	}
}

void APBCharacter::OnStartWalking()
{
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed() && !IsPlantingBomb() && !IsDefusingBomb())
	{
		SetWalking(true);
	}
}

void APBCharacter::OnStopWalking()
{
	SetWalking(false);
}

void APBCharacter::OnStartSprinting()
{
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed() && !IsPlantingBomb() && !IsDefusingBomb() && !IsCrouching())
	{
		if (IsZooming())
		{
			SetZooming(false);
		}
		StopWeaponFire();

		if (IsCrouching())
		{
			
		}
		SetSprinting(true);
	}
}

void APBCharacter::OnStopSprinting()
{
	SetSprinting(false);
}

void APBCharacter::OnSprintToggle()
{
	if (IsSprinting())
	{
		OnStopSprinting();
	}
	else
	{
		OnStartSprinting();
	}
}

bool APBCharacter::IsFiring() const
{
	APBWeapon* Weapon = GetCurrentWeapon();
	if (Weapon)
	{
		return Weapon->IsFiring(); /*|| bWantsToFireTrigger*/;
	}
	return false;
};

bool APBCharacter::CanFire() const
{
	return IsAlive() && false == IsSpectator();
}

void APBCharacter::OnStartFireTrigger1()
{
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed() && !IsPlantingBomb() && !IsDefusingBomb())
	{
		if (IsSprinting())
		{
			SetSprinting(false);
		}
		StartWeaponFire(EFireTrigger::Fire1);
	}
}

void APBCharacter::OnEndFireTrigger1()
{
	StopWeaponFire(EFireTrigger::Fire1);
}

void APBCharacter::OnStartFireTrigger2()
{
#if WITH_EDITOR
	if (bShiftDebugViewInput && IsViewingOwnself() && IsFirstPersonView())
		return;
#endif

	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed() && !IsPlantingBomb() && !IsDefusingBomb())
	{
		if (IsSprinting())
		{
			SetSprinting(false);
		}
		StartWeaponFire(EFireTrigger::Fire2);
	}
}

void APBCharacter::OnEndFireTrigger2()
{
#if WITH_EDITOR
	if (bShiftDebugViewInput && IsViewingOwnself() && IsFirstPersonView())
		return;
#endif

	StopWeaponFire(EFireTrigger::Fire2);
}

void APBCharacter::OnWeapFuncTrigger()
{
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed() && !IsPlantingBomb() && !IsDefusingBomb())
	{
		if (IsSprinting())
		{
			SetSprinting(false);
		}

		if (CurrentWeapon)
		{
			CurrentWeapon->FuncTriggerBegin();
		}
	}
}

void APBCharacter::OnStartMeleeAttackTrigger()
{
#if WITH_EDITOR
	if (bShiftDebugViewInput && IsViewingOwnself() && IsFirstPersonView())
		return;
#endif

	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed() && !IsPlantingBomb() && !IsDefusingBomb())
	{
		if (IsSprinting())
		{
			SetSprinting(false);
		}

		if (!bWantsToMeleeTrigger)
		{
			bWantsToMeleeTrigger = true;

			if (CurrentWeapon)
			{
				CurrentWeapon->MeleeAttackTriggerBegin();
			}
		}
	}
}

void APBCharacter::OnEndMeleeAttackTrigger()
{
	if (bWantsToMeleeTrigger)
	{
		bWantsToMeleeTrigger = false;

		if (CurrentWeapon)
		{
			CurrentWeapon->MeleeAttackTriggerEnd();
		}
	}
}

void APBCharacter::OnStartQuickThrowTrigger(EWeaponType Type)
{
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed() && !IsPlantingBomb() && !IsDefusingBomb())
	{
		if (IsSprinting())
		{
			SetSprinting(false);
		}

		if (!bWantsToQuickThrowing)
		{
			bWantsToQuickThrowing = true;

			if (CurrentWeapon)
			{
				CurrentWeapon->QuickThrowTriggerBegin(Type);
			}
		}
	}
}

void APBCharacter::OnStopQuickThrowTrigger()
{
	if (bWantsToQuickThrowing)
	{
		bWantsToQuickThrowing = false;

		if (CurrentWeapon)
		{
			CurrentWeapon->QuickThrowTriggerEnd();
		}
	}
}

void APBCharacter::HandleStartQuickThrow()
{
	
	bool bHaveBoth = GetGrenadeWeapon() && GetSpecialWeapon();

	// If you have both weapons, then use the selected one.
	if (bHaveBoth)
	{
		if (CurrentSelectedQuickWeapSlot == EWeaponSlot::Grenade)
		{
			HandleStartQuickGrenade();
		}
		else if (CurrentSelectedQuickWeapSlot == EWeaponSlot::Special)
		{
			HandleStartQuickSpecial();
		}
	}
	else
	{
		// If you only have one of them, use the one.
		if (GetGrenadeWeapon())
		{
			HandleStartQuickGrenade();
		}
		else if(GetSpecialWeapon())
		{
			HandleStartQuickSpecial();
		}
	}


}

void APBCharacter::HandleStartQuickGrenade()
{
	OnStartQuickThrowTrigger(EWeaponType::GRENADE);
}

void APBCharacter::HandleStartQuickSpecial()
{
	OnStartQuickThrowTrigger(EWeaponType::SPECIAL);

}

void APBCharacter::OnPressPickup()
{
	if (PickupAction)
	{
		PickupAction->NotifyPickupButtonPressed();
	}
}

void APBCharacter::OnReleasePickup()
{
	if (PickupAction)
	{
		PickupAction->NotifyPickupButtonReleased();
	}
}

void APBCharacter::WakeFireTrigger()
{
	if (bWantsToMeleeTrigger)
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->MeleeAttackTriggerBegin();
		}
	}

	if (bWantsToFireTrigger[0])
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->FireTriggerBegin(EFireTrigger::Fire1);
		}
	}
	if (bWantsToFireTrigger[1])
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->FireTriggerBegin(EFireTrigger::Fire2);
		}
	}
}

void APBCharacter::StartWeaponFire(EFireTrigger eFireTrigger)
{
	int32 idx = (eFireTrigger == EFireTrigger::Fire1) ? 0 : 1;
	
	if (!bWantsToFireTrigger[idx])
	{
		bWantsToFireTrigger[idx] = true;
		if (CurrentWeapon)
		{
			CurrentWeapon->FireTriggerBegin(eFireTrigger);
		}
	}
}

void APBCharacter::StopWeaponFire(EFireTrigger eFireTrigger)
{
	int32 idx = eFireTrigger == EFireTrigger::Fire1 ? 0 : 1;

	if (bWantsToFireTrigger[idx])
	{
		bWantsToFireTrigger[idx] = false;
		if (CurrentWeapon)
		{
			CurrentWeapon->FireTriggerEnd(eFireTrigger);
		}
	}
}

void APBCharacter::OnZoomIn()
{
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed() && !IsPlantingBomb() && !IsDefusingBomb())
	{
		bWantsToZooming = true;

		APBWeapon* Weapon = GetCurrentWeapon();
		if (Weapon && !Weapon->CanZooming())
		{
			return;
		}

		if (IsSprinting())
		{
			SetSprinting(false);
		}
		
		SetZooming(true);
	}
}

void APBCharacter::OnZoomOut()
{
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed() && !IsPlantingBomb() && !IsDefusingBomb())
	{
		bWantsToZooming = false;
		SetZooming(false);
	}
}

void APBCharacter::OnRep_ZoomState()
{
	SetZooming(bIsZooming);
}

bool APBCharacter::IsZooming() const
{
	return bIsZooming;
}

bool APBCharacter::IsWantsToZooming() const
{
	return bWantsToZooming;
}

bool APBCharacter::GetViewerIsZooming() const
{
	auto PlayerCharacter = Cast<APBCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	bool bResultIsViewingOwnself = false;
	bool bResultIsZooming = false;

	if (nullptr != PlayerCharacter)
	{
		bResultIsViewingOwnself = PlayerCharacter->IsViewingOwnself();
		if (false == bResultIsViewingOwnself)
		{
			bResultIsZooming = PlayerCharacter->IsZooming();
		}
	}

	if (bResultIsViewingOwnself)
	{
		bResultIsZooming = IsZooming();
	}

	return bResultIsZooming;
}

bool APBCharacter::CanZooming() const
{
	return IsAlive() && !IsPendingKill() && !IsSpectator();
}

void APBCharacter::SetZooming(bool bZoom)
{
	bIsZooming = bZoom;

	APBWeapon* Weapon = GetCurrentWeapon();
	if (Weapon)
	{
		Weapon->SetZooming(bZoom);
	}

	if (IsLocallyControlled())
	{
		APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
		if (MyPC)
		{
			MyPC->WidgetEventDispatcher->OnZoom.Broadcast(bZoom);
		}

		/*auto GS = UGameplayStatics::GetGameState(this);
		for (auto CurItr : GS->PlayerArray)
		{
			auto CurPS = Cast<APBPlayerState>(CurItr);
			auto CurOC = CurPS->GetOwnerCharacter();
			if (nullptr == CurOC)
				continue;

			auto TP = CurOC->GetTargetViewPlayer();
			if (nullptr == TP)
				continue;

			auto TC = Cast<APBCharacter>(TP->GetOwnerCharacter());
			if (TC && TC == this)
			{
				TC->bIsZooming = bZoom;
				break;
			}
		}*/

		if (Role < ROLE_Authority)
		{
			ServerSetZooming(bZoom);
		}
	}
}

bool APBCharacter::BroadcastWidgetEventZooming_Validate(bool bZoom)
{
	return true;
}
void APBCharacter::BroadcastWidgetEventZooming_Implementation(bool bZoom)
{
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC)
	{
		MyPC->WidgetEventDispatcher->OnZoom.Broadcast(bZoom);
	}
}

bool APBCharacter::ServerSetZooming_Validate(bool bZoom)
{
	return true;
}

void APBCharacter::ServerSetZooming_Implementation(bool bZoom)
{
	SetZooming(bZoom);
}


void APBCharacter::InitVoiceSound()
{
	int32 NumSound = Voices.Num();
	for (int32 i = 0; i < NumSound; i++)
	{
		const FCharacterVoice& Voice = Voices[i];
		if (Voice.Sound)
		{
			VoiceMap.Add(Voice.VoiceType, Voice.Sound);
		}
	}
}

UAudioComponent* APBCharacter::PlayVoice(EPBVoiceType VoiceType)
{
	if (GetNetMode() == NM_DedicatedServer)
		return nullptr;

	USoundCue** SoundPtr = VoiceMap.Find(VoiceType);
	USoundCue* Sound = SoundPtr ? *SoundPtr : nullptr;
	if (Sound)
	{
		return UPBGameplayStatics::SpawnSoundAttached(Sound, GetRootComponent());
	}

	return nullptr;
}

void APBCharacter::OnNextWeapon()
{
	if (IsCameraStyle(EPBCameraStyle::DebugFreeCam) || IsCameraStyle(EPBCameraStyle::DebugSphericalCam))
		return;

	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed() && !IsPlantingBomb() && !IsDefusingBomb())
	{
		//if (Inventory.Num() >= 2 && (CurrentWeapon == NULL || CurrentWeapon->GetCurrentState() != EWeaponState::Equipping))
		if (Inventory.Num() >= 2 && CurrentWeapon)
		{
			const int32 CurrWeapSlot = int32(GetWeaponSlot(CurrentWeapon));
			int32 NextWeapSlot = CurrWeapSlot;
			int32 NextWeapSlotIdx = 0;

			do 
			{
				NextWeapSlot = (NextWeapSlot + 1) % Inventory.Num();
				if (NextWeapSlot == int32(EWeaponSlot::Primary) || NextWeapSlot == int32(EWeaponSlot::Secondary))
				{
					if (SwitchWeapon(EWeaponSlot(NextWeapSlot), NextWeapSlotIdx, EWeaponMode::Mode1))
					{
						break;
					}
				}			

			} while (CurrWeapSlot != NextWeapSlot);

		}
	}
}

void APBCharacter::OnPrevWeapon()
{
	if (IsCameraStyle(EPBCameraStyle::DebugFreeCam) || IsCameraStyle(EPBCameraStyle::DebugSphericalCam))
		return;

	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed() && !IsPlantingBomb() && !IsDefusingBomb())
	{
		//if (Inventory.Num() >= 2 && (CurrentWeapon == NULL || CurrentWeapon->GetCurrentState() != EWeaponState::Equipping))
		if (Inventory.Num() >= 2 && CurrentWeapon)
		{
			const int32 CurrWeapSlot = int32(GetWeaponSlot(CurrentWeapon));
			int32 NextWeapSlot = CurrWeapSlot;
			int32 NextWeapSlotIdx = 0;

			do
			{
				NextWeapSlot = (NextWeapSlot - 1 + Inventory.Num()) % Inventory.Num();
				if (NextWeapSlot == int32(EWeaponSlot::Primary) || NextWeapSlot == int32(EWeaponSlot::Secondary))
				{
					if (SwitchWeapon(EWeaponSlot(NextWeapSlot), NextWeapSlotIdx, EWeaponMode::Mode1))
					{
						break;
					}
				}

			} while (CurrWeapSlot != NextWeapSlot);
		}
	}
}

void APBCharacter::OnSwitchWeapon()
{
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC == nullptr || !MyPC->IsGameInputAllowed() && !IsPlantingBomb() && !IsDefusingBomb())
		return;

	EWeaponSlot eWeapKey = EWeaponSlot::Primary;

	if (MyPC->IsInputKeyDown(EKeys::One))
	{
		eWeapKey = EWeaponSlot::Primary;
	}
	else if (MyPC->IsInputKeyDown(EKeys::Two))
	{
		eWeapKey = EWeaponSlot::Secondary;
	}
	else if (MyPC->IsInputKeyDown(EKeys::Three))
	{
		eWeapKey = EWeaponSlot::Melee;
	}
	else if (MyPC->IsInputKeyDown(EKeys::Four))
	{
		eWeapKey = EWeaponSlot::Grenade;
	}
	else if (MyPC->IsInputKeyDown(EKeys::Five))
	{
		eWeapKey = EWeaponSlot::Special;
	}

	SwitchWeapon(eWeapKey, 0, EWeaponMode::Mode1);
}

void APBCharacter::OnSwitchQuickWeapon()
{
	// Now we have only two case.
	// Grenade slot or Special slot.

	if (CurrentSelectedQuickWeapSlot == EWeaponSlot::Grenade)
	{
		CurrentSelectedQuickWeapSlot = EWeaponSlot::Special;
	}
	else if (CurrentSelectedQuickWeapSlot == EWeaponSlot::Special)
	{
		CurrentSelectedQuickWeapSlot = EWeaponSlot::Grenade;
	}

	//print("Current selected quick weapon slot: " + EnumToString(EWeaponSlot, CurrentSelectedQuickWeapSlot));
}

bool APBCharacter::CanReload() const
{
	return IsAlive() && !IsSprinting() && !IsSpectator();
}

void APBCharacter::OnReload()
{
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed() && !IsPlantingBomb() && !IsDefusingBomb())
	{
		if (CurrentWeapon && !CurrentWeapon->IsReloading())
		{
			CurrentWeapon->StartReload();
		}
	}
}

void APBCharacter::OnReloadEnded()
{
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC)
	{
		MyPC->WidgetEventDispatcher->OnReloadEnded.Broadcast();
	}

}

void APBCharacter::OnEquipFinished()
{
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC)
	{
		MyPC->WidgetEventDispatcher->OnEquipFinished.Broadcast();
	}
}

void APBCharacter::PlayFootstep(EPBFootstepType StepType, bool bLeft)
{
	if (GetWorld() == nullptr)
		return;

	static FName NAME_FootstepTrace(TEXT("FootstepTrace"));
	FCollisionQueryParams QueryParams(NAME_FootstepTrace, false, this);
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.bTraceAsyncScene = true;

	FHitResult HitResult(1.f);

	if (StepType == EPBFootstepType::Step)
	{
		FVector LineTraceStart = GetBoneLocation(bLeft ? BoneName_LFoot : BoneName_RFoot);
		FVector LineTraceEnd = LineTraceStart - 40.f;
		GetWorld()->LineTraceSingleByChannel(HitResult, LineTraceStart, LineTraceEnd, GetCapsuleComponent()->GetCollisionObjectType(), QueryParams);
	}
	else
	{
		float PawnRadius, PawnHalfHeight;
		GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);

		const FVector LineTraceStart = GetCapsuleComponent()->GetComponentLocation(); //capsule center
		FVector LineTraceEnd = LineTraceStart - (40.0f + PawnHalfHeight);
		GetWorld()->LineTraceSingleByChannel(HitResult, LineTraceStart, LineTraceEnd, GetCapsuleComponent()->GetCollisionObjectType(), QueryParams);
	}

	if (HitResult.bBlockingHit)
	{
		if (HitResult.PhysMaterial.IsValid())
		{
			FTransform const SpawnTransform(HitResult.ImpactNormal.Rotation(), HitResult.ImpactPoint);

			APBImpactEffect* EffectActor = nullptr;

			switch (StepType)
			{
			case EPBFootstepType::Step:
				EffectActor = GetWorld()->SpawnActorDeferred<APBImpactEffect>(GetViewerIsFirstPerson() ? FootstepEffect1P : FootstepEffect3P, SpawnTransform);
				break;
			case EPBFootstepType::Jump:
				EffectActor = GetWorld()->SpawnActorDeferred<APBImpactEffect>(GetViewerIsFirstPerson() ? JumpEffect1P : JumpEffect3P, SpawnTransform);
				break;
			case EPBFootstepType::Land:
				EffectActor = GetWorld()->SpawnActorDeferred<APBImpactEffect>(GetViewerIsFirstPerson() ? LandEffect1P : LandEffect3P, SpawnTransform);
				break;
			}

			if (EffectActor)
			{
				EffectActor->SurfaceHit = HitResult;
				EffectActor->SetOwner(this);
				EffectActor->Instigator = this;
				UPBGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
			}
		}
	}
}

void APBCharacter::SetLobbyAnimClass()
{
	if (GetMesh3P())
	{
		GetMesh3P()->SetAnimInstanceClass(LobbyAnimClass);
	}
}

void APBCharacter::Tick(float DeltaSeconds)
{	
	Super::Tick(DeltaSeconds);

	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC && MyPC->HasHealthRegen())
	{
		if (this->Health < this->GetMaxHealth())
		{
			this->Health +=  5 * DeltaSeconds;
			if (Health > this->GetMaxHealth())
			{
				Health = this->GetMaxHealth();
			}
		}
	}
	
	if (LowHealthSound && GEngine->UseSound())
	{
		if ((this->Health > 0 && this->Health < this->GetMaxHealth() * LowHealthPercentage) && (!LowHealthWarningPlayer || !LowHealthWarningPlayer->IsPlaying()))
		{
			LowHealthWarningPlayer = UPBGameplayStatics::SpawnSoundAttached(LowHealthSound, GetRootComponent(),
				NAME_None, FVector(ForceInit), EAttachLocation::KeepRelativeOffset, true);
			LowHealthWarningPlayer->SetVolumeMultiplier(0.0f);
		} 
		else if ((this->Health > this->GetMaxHealth() * LowHealthPercentage || this->Health < 0) && LowHealthWarningPlayer && LowHealthWarningPlayer->IsPlaying())
		{
			LowHealthWarningPlayer->Stop();
		}
		if (LowHealthWarningPlayer && LowHealthWarningPlayer->IsPlaying())
		{
			const float MinVolume = 0.3f;
			const float VolumeMultiplier = (1.0f - (this->Health / (this->GetMaxHealth() * LowHealthPercentage)));
			LowHealthWarningPlayer->SetVolumeMultiplier(MinVolume + (1.0f - MinVolume) * VolumeMultiplier);
		}
	}

	InterpRecoilCorr(DeltaSeconds);
	
	
	UpdateSpectatorCameraRotationSync();

	const bool bLocallyControlled = IsLocallyControlled();
	if (bLocallyControlled && !IsBot())
	{
		TakeAimingTrace();
	}
	if (GetNameTagWidget())
	{
		GetNameTagWidget()->Update();
	}

	UpdateFollowTargetOnSpectatorMode();

	OnFalling();
}


void APBCharacter::OnStartJump()
{
	APBPlayerController* MyPC = Cast<APBPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed() && !IsPlantingBomb() && !IsDefusingBomb())
	{
		Jump();
	}
}

void APBCharacter::OnStopJump()
{
	StopJumping();
}

//////////////////////////////////////////////////////////////////////////
// Replication

APBWeapon* APBCharacter::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

class APBWeapon* APBCharacter::GetCurrentQuickWeapon()
{
	return CurrentQuickWeapon;
}

EWeaponType APBCharacter::GetCurrentWeaponType() const
{
	if (CurrentWeapon)
	{
		return CurrentWeapon->GetWeaponType();
	}

	return EWeaponType::AR;
}

APBWeapon* APBCharacter::FindWeaponByType(TSubclassOf<APBWeapon> WeaponClass)
{
	for (FWeaponSlotSet& SlotWeapSet : Inventory)
	{
		for (FWeaponModeSet& WeapModeSet : SlotWeapSet.SlotWeapons)
		{
			for (APBWeapon* Weapon : WeapModeSet.Weapons)
			{
				if (Weapon && !Weapon->IsPendingKill() && Weapon->GetClass() == WeaponClass)
				{
					return Weapon;
				}
			}
		}
	}

	return nullptr;
}

class APBWeapon* APBCharacter::GetWeapon(EWeaponSlot eSlot, int32 SlotIdx, EWeaponMode eMode)
{
	int32 Slot = int32(eSlot);

	if (Inventory.IsValidIndex(Slot))
	{
		const TArray<FWeaponModeSet>& SlotWeapons = Inventory[Slot].SlotWeapons;
		if (SlotWeapons.IsValidIndex(SlotIdx))
		{
			const FWeaponModeSet& WeapModeSet = SlotWeapons[SlotIdx];
			if (WeapModeSet.Weapons.IsValidIndex(int32(eMode)))
			{
				return WeapModeSet.Weapons[int32(eMode)];
			}				
		}
	}
	return nullptr;
}

TArray<FWeaponModeSet>& APBCharacter::GetSlotWeapons(EWeaponSlot eSlot)
{
	if (Inventory.IsValidIndex(int32(eSlot)))
	{
		return Inventory[int32(eSlot)].SlotWeapons;
	}

	EmptySlotWeapons.Empty();
	return EmptySlotWeapons;
}

FWeaponModeSet& APBCharacter::GetSlotIdxWeapons(EWeaponSlot eSlot, int32 SlotIdx)
{
	int32 Slot = int32(eSlot);

	if (Inventory.IsValidIndex(Slot))
	{
		TArray<FWeaponModeSet>& SlotWeapons = Inventory[Slot].SlotWeapons;
		if (SlotWeapons.IsValidIndex(SlotIdx))
		{
			return SlotWeapons[SlotIdx];
		}
	}

	EmptyWeapModeSet.Weapons.Empty();
	return EmptyWeapModeSet;
}

APBWeapon* APBCharacter::GetGrenadeWeapon()
{
	const FWeaponModeSet& WeapModeSet = GetSlotIdxWeapons(EWeaponSlot::Grenade, CurrentGrenadeSlotIdx);

	for (APBWeapon* Weapon : WeapModeSet.Weapons)
	{
		if (Weapon && Weapon->GetWeaponType() == EWeaponType::GRENADE && Weapon->HasAmmoInClip())
		{
			return Weapon;
		}
	}

	return nullptr;
}



APBWeapon* APBCharacter::GetSpecialWeapon()
{
	FWeaponModeSet& WeapModeSet = GetSlotIdxWeapons(EWeaponSlot::Special, CurrentGrenadeSlotIdx);
	
	for (APBWeapon* Weapon : WeapModeSet.Weapons)
	{
		if (Weapon && Weapon->HasAmmoInClip())
		{
			auto WeapType = Weapon->GetWeaponType();
			if (WeapType == EWeaponType::SPECIAL ||
				WeapType == EWeaponType::GRENADE // For now, there's still some special weapons that has weapon type as a GRENADE.(In the future, This line should be removed.)
				)	
			{
				return Weapon;
			}

		}
	}

	return nullptr;
}

bool APBCharacter::GetWeaponSlotWithIdxMode(APBWeapon* EquippedWeapon, EWeaponSlot& outSlot, int32& outSlotIdx, EWeaponMode& outMode)
{
	for (int32 Slot = 0; Slot < Inventory.Num(); Slot++)
	{
		const TArray<FWeaponModeSet>& SlotWeapons = Inventory[Slot].SlotWeapons;

		for (int32 SlotIdx = 0 ; SlotIdx < SlotWeapons.Num(); SlotIdx++)
		{
			for (int32 Mode = 0; Mode < SlotWeapons[SlotIdx].Weapons.Num(); Mode++)
			{
				if (SlotWeapons[SlotIdx].Weapons[Mode] == EquippedWeapon)
				{
					outSlot = EWeaponSlot(Slot);
					outSlotIdx = SlotIdx;
					outMode = EWeaponMode(Mode);
					return true;
				}
			}
		}		
	}

	outSlot = EWeaponSlot::Max;
	outSlotIdx = -1;
	outMode = EWeaponMode::Mode1;
	return false;
}

EWeaponSlot APBCharacter::GetWeaponSlot(APBWeapon* EquippedWeapon)
{
	EWeaponSlot outSlot;
	int32 outSlotIdx;
	EWeaponMode outMode;
	GetWeaponSlotWithIdxMode(EquippedWeapon, outSlot, outSlotIdx, outMode);
	return outSlot;
}

int32 APBCharacter::GetWeaponSlotIdx(APBWeapon* EquippedWeapon)
{
	EWeaponSlot outSlot;
	int32 outSlotIdx;
	EWeaponMode outMode;
	GetWeaponSlotWithIdxMode(EquippedWeapon, outSlot, outSlotIdx, outMode);
	return outSlotIdx;
}

EWeaponMode APBCharacter::GetWeaponMode(APBWeapon* EquippedWeapon)
{
	EWeaponSlot outSlot;
	int32 outSlotIdx;
	EWeaponMode outMode;
	GetWeaponSlotWithIdxMode(EquippedWeapon, outSlot, outSlotIdx, outMode);
	return outMode;
}

EWeaponSlot APBCharacter::GetCurrentWeaponSlot()
{
	return CurrentWeaponSlot;
}

int32 APBCharacter::GetCurrentWeaponSlotIdx()
{
	return CurrentWeaponSlotIdx;
}

EWeaponMode APBCharacter::GetCurrentWeaponMode()
{
	return CurrentWeaponMode;
}

int32 APBCharacter::GetInventoryCount() const
{
	return Inventory.Num();
}

int32 APBCharacter::GetSlotWeaponsCount(EWeaponSlot eSlot)
{
	return GetSlotWeapons(eSlot).Num();
}

USkeletalMeshComponent* APBCharacter::GetPawnMesh() const
{
	return GetViewerIsFirstPerson() ? GetMesh1P() : GetMesh3P();
}

USkeletalMeshComponent* APBCharacter::GetSpecificPawnMesh( bool WantFirstPerson ) const
{
	return WantFirstPerson == true  ? GetMesh1P() : GetMesh3P();
}

FName APBCharacter::GetWeaponAttachPointName(EWeaponPoint ePoint) const
{
	switch (ePoint)
	{
	case EWeaponPoint::Point_R:	return FName("WeaponPointR");
	case EWeaponPoint::Point_L:	return FName("WeaponPointL");
	}
	return FName("WeaponPointR");
}

float APBCharacter::GetZoomingSpeedModifier() const
{
	return CharacterData.ZoomingSpeedModifier;
}

bool APBCharacter::IsFirstPersonView() const
{
	bool bIsFirstPersonView = IsCameraStyle(EPBCameraStyle::FPS) || IsCameraStyle(EPBCameraStyle::RagDoll);

	return bIsFirstPersonView;
}

bool APBCharacter::IsViewingOwnself() const
{
	return bIsViewOwnself;
}

bool APBCharacter::GetViewerIsFirstPerson() const
{
	auto PlayerCharacter = Cast<APBCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	bool bIsViewingOwnself = false;
	bool bIsFirstPerson = false;

	if (nullptr != PlayerCharacter)
	{
		bIsViewingOwnself = PlayerCharacter->IsViewingOwnself();
		if (false == bIsViewingOwnself)
		{
			bIsFirstPerson = PlayerCharacter->IsFirstPersonView();
		}
	}

	if (bIsViewingOwnself)
	{
		bIsFirstPerson = IsFirstPersonView();
	}

	return bIsFirstPerson;
}

EPBCameraStyle APBCharacter::GetCameraStyle() const
{
	return CurrentCameraStyle;
}

void APBCharacter::SetCameraStyle(EPBCameraStyle CameraStyle)
{
	if (CurrentCameraStyle != CameraStyle)
		CurrentCameraStyle = CameraStyle;
}

bool APBCharacter::IsCameraStyle(EPBCameraStyle CameraStyle) const
{
	return GetCameraStyle() == CameraStyle;
}

int32 APBCharacter::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<APBCharacter>()->Health;
}

float APBCharacter::GetHealth() const
{
	return Health;
}

float APBCharacter::GetHealthPercentage() const
{
	float MaxHp = (float)GetMaxHealth();
	float Hp = (float)GetHealth();
	float HealthPercent = (MaxHp <= 0.f) ? 0.f : FMath::Clamp(Hp / MaxHp, 0.0f, 1.0f);

	return HealthPercent;
}

bool APBCharacter::IsAlive() const
{
	return Health > 0;
}

bool APBCharacter::IsSpectator() const
{
	return PlayerState ? PlayerState->bIsSpectator : false;
}

void APBCharacter::SetSpectator(bool bNewSpectator)
{
	if (ensure(PlayerState))
	{
		PlayerState->bIsSpectator = bNewSpectator;
	}
}

float APBCharacter::ApplyMovementSpeedModifier(float Speed) const
{
	// 차후 movement type 로 변경해야겠다. 지금은 이렇게(IsWalking(), IsRunning(), IsSprinting())

	// start walk, crouched walk speed

	if (IsWalking())
	{
		Speed += Speed * GetWalkingSpeedModifier(); 
	}
	else if (IsSprinting())
	{
		Speed += Speed * GetSprintingSpeedModifier();
	}
	else /* if (IsRunning()) */
	{
		Speed += Speed * GetRunningSpeedModifier();
	}

	// character state 
	if (IsZooming())
	{
		Speed += Speed * GetZoomingSpeedModifier();
	}

	// moving direction(기획서 공식 적용)
	const FVector MoveDirection = GetVelocity().GetSafeNormal();
	const FVector ViewDirection = FRotationMatrix(GetActorRotation()).GetUnitAxis(EAxis::X);
	const float Cos_Theta = FVector::DotProduct(MoveDirection, ViewDirection);
	const float DirectionModifier = FMath::Clamp((Cos_Theta - 1.f), -1.f, 0.f);
	float DirectionSpeedModifier = DirectionModifier * CharacterData.DirectionSpeedModifier;
	Speed += Speed * DirectionSpeedModifier;

	// weapon
	if (GetCurrentWeapon())
	{
		Speed = GetCurrentWeapon()->ApplyMovementSpeedModifier(Speed);
	}	

	// damage
	Speed = ApplyDamagedSpeed(Speed);

	return Speed;
}

void APBCharacter::LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride)
{
	Super::LaunchCharacter(LaunchVelocity, bXYOverride, bZOverride);

	FVector L = GetActorLocation();
	if (FallDamage.AccumulatedFallDeathTime <= 0)
	{
		FallDamage.FallingStartedPosition = L;
	}
}

void APBCharacter::Falling()
{
	bIsFalling = true;

	Super::Falling();
}

void APBCharacter::OnFalling()
{
	if (false == bIsFalling)
		return;

	FVector L = GetActorLocation();
	if (FallDamage.AccumulatedFallDeathTime <= 0)
	{
		FallDamage.FallingStartedPosition = L;
	}

	if (FallDamage.FallingStartedPosition.Z < L.Z)
	{
		FallDamage.AccumulatedFallDeathTime = 0.0f;
		FallDamage.FallingStartedPosition = L;
	}

	FallDamage.AccumulatedFallDeathTime += FApp::GetDeltaTime();

	//@todo: FallDeathTime이상 떨어지면 사망하게한다.
	if (FallDamage.AccumulatedFallDeathTime > FallOffDeathTime)
	{
		FDamageEvent Dmg;
		if (FallOffDamageTypeClass)
		{
			Dmg.DamageTypeClass = FallOffDamageTypeClass;
		}
		else
		{
			Dmg.DamageTypeClass = UPBFallOffDamageType::StaticClass();
		}

		//@todo: Die, 다른 방식도 생각해보아야
		float Damage = 1000.0f;
		TakeDamage(Damage, Dmg, Controller, this);
	}

	FVector CurVelocity = GetVelocity();
	//float CurVelocityLength = CurVelocity.Size();
	float CurVelocityLength = FVector(0, 0, CurVelocity.Z).Size();

	if (UPBCheatManager::EnabledDebugFallOffStatus)
	{
		FColor MsgColor = (FLinearColor(FColor::Cyan) * 0.5f).ToFColor(true);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, MsgColor
			, FString::Printf(TEXT("[DebugFallOffDamage] Velocity: %0.2fm/s")
			, CurVelocityLength * 0.01f));
	}
}

void APBCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	FVector CurVelocity = GetVelocity();
	//float CurVelocityLength = CurVelocity.Size();
	float CurVelocityLength = FVector(0, 0, CurVelocity.Z).Size();

	if (UPBCheatManager::EnabledDebugFallOffStatus)
	{
		FColor MsgColor = FColor::Cyan;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan
			, FString::Printf(TEXT("[DebugFallOffDamage] Velocity: %0.2fm/s")
				, CurVelocityLength * 0.01f));
	}

	if (CurVelocityLength > FallOffDamageSpeed)
	{
		//@todo:
		float FallenHeight = FallDamage.FallingStartedPosition.Z - Hit.Location.Z;
		int CurFallDamage = FMath::CeilToInt((CurVelocityLength - FallOffDamageSpeed) * FallOffDamageSlope + FallOffDamageMin);

		if (UPBCheatManager::EnabledDebugFallOffDamage)
		{
			//@todo: take damamge
			FColor MsgColor = (FLinearColor(FColor::Red) * (1.0f - CurFallDamage / GetMaxHealth())).ToFColor(true);
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, MsgColor
				, FString::Printf(TEXT("[DebugFallOffDamage] Velocity: %0.2fm/s, Height: %dcm, Damage: %d")
					, CurVelocityLength * 0.01f, FMath::CeilToInt(FallenHeight), CurFallDamage));
		}

		FDamageEvent Dmg;
		if (FallOffDamageTypeClass)
		{
			Dmg.DamageTypeClass = FallOffDamageTypeClass;
		}
		else
		{
			Dmg.DamageTypeClass = UPBFallOffDamageType::StaticClass();
		}

		TakeDamage(CurFallDamage, Dmg, Controller, this);
	}

	// Reset
	FallDamage.AccumulatedFallDeathTime = 0;
	FallDamage.FallingStartedPosition = FVector::ZeroVector;

	bIsFalling = false;
}

void APBCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (PrevMovementMode == MOVE_Falling)
		bIsFalling = false;

	if (nullptr == GEngine)
		return;

	if (UPBCheatManager::EnabledDebugFallOffStatus)
	{
		FColor MsgColor = (bIsFalling ? FLinearColor(FColor::Cyan) : FLinearColor(FColor::White)).ToFColor(true);
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, MsgColor
			, FString::Printf(TEXT("[DebugFallOffDamage] PrevMovementMode: %s"), *EnumToString(EMovementMode, PrevMovementMode)));
	}
}

float APBCharacter::GetLowHealthPercentage() const
{
	return LowHealthPercentage;
}

void APBCharacter::UpdateTeamColorsAllMIDs()
{
	for (int32 i = 0; i < MeshMIDs.Num(); ++i)
	{
		UpdateTeamColors(MeshMIDs[i]);
	}
}

EPBTeamType APBCharacter::GetTeamType() const
{
	EPBTeamType TeamType = EPBTeamType::Alpha;

	APBPlayerState* PS = Cast<APBPlayerState>(PlayerState);
	if (PS)
	{
		TeamType = PS->GetTeamType();
	}

	return TeamType;
}


bool APBCharacter::IsSameTeam(EPBTeamType eOtherTeam)
{
	return GetTeamType() == eOtherTeam;
}


void APBCharacter::SetAimingTargeted(bool bSet)
{
	bIsAimingTargeted = bSet;	
}

APBCharacter *APBCharacter::GetAmingTargetCache() const
{
	return AimingTargetCache;
}

float APBCharacter::ApplyDamagedSpeed(float InSpeed) const
{
	return InSpeed;
}

// only LocalPlayer
void APBCharacter::TakeAimingTrace()
{
	if (GetWorld() == nullptr)
		return;

	// 모든 플레이어 타겟팅 상태 초기화
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* ItPawn = It->Get();
		const APBCharacter* SkipPawn = this;
		if (IsValid(ItPawn) && SkipPawn != ItPawn)
		{
			APBCharacter* TargetPawn = Cast<APBCharacter>(ItPawn);
			if (TargetPawn)
			{
				TargetPawn->SetAimingTargeted(false);
			}
		}
	}

	// 타겟팅 검사
	if (CurrentWeapon)
	{
		CurrentWeapon->AimingTrace(AimingTraceResultCache);

		AimingTargetCache = Cast<APBCharacter>(AimingTraceResultCache.Actor.Get());

		if (AimingTraceResultCache.bBlockingHit)
		{
			APBCharacter* TargetActer = Cast<APBCharacter>(AimingTraceResultCache.GetActor());
			if (IsValid(TargetActer))
			{
				TargetActer->SetAimingTargeted(true);
			}
		}
	}
}

FHitResult& APBCharacter::GetAimingTraceResult()
{
	return AimingTraceResultCache;
}
