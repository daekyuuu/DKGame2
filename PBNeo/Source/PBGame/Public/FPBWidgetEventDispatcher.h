// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "PBTypes.h"
#include "PBKillMarkTypes.h"

/**
 * This class is used for local UMG Widgets
 */


struct FPBWidgetEventDispatcher : public TSharedFromThis<FPBWidgetEventDispatcher>
{
	FPBWidgetEventDispatcher();
	~FPBWidgetEventDispatcher();

	/* ------------------------------------------------------------------------------- */
	// Basic Events
	/* ------------------------------------------------------------------------------- */

	DECLARE_MULTICAST_DELEGATE_OneParam(FMatchStartedEvent, int32 /* Round Number */);
	FMatchStartedEvent OnMatchStarted;

	DECLARE_MULTICAST_DELEGATE(FMatchEndedEvent);
	FMatchEndedEvent OnWarmupStarted;

	DECLARE_MULTICAST_DELEGATE_OneParam(FConnectionTypeChangedEvent, bool /* Is Server? */);
	FConnectionTypeChangedEvent OnConnectionTypeChanged;

	DECLARE_MULTICAST_DELEGATE_FourParams(FDeathEvent, class APBPlayerState*, class APBPlayerState*, const UDamageType*, const FTakeDamageInfo&);
	FDeathEvent OnDeath;

	DECLARE_MULTICAST_DELEGATE_OneParam(FSpawnEvent, class APBPlayerState*);
	FSpawnEvent OnSpawn;

	DECLARE_MULTICAST_DELEGATE_OneParam(FZoomEvent, bool);
	FZoomEvent OnZoom;

	DECLARE_MULTICAST_DELEGATE(FWeaponChangeEvent);
	FWeaponChangeEvent OnWeaponChange;

	// got hit by someone.
	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnHitEvent, float, const FVector&, bool /*bIsDamagedByOwnself*/);
	FOnHitEvent OnHit;

	// Success to hit someone
	DECLARE_MULTICAST_DELEGATE_OneParam(FHitSuccessEvent, bool);
	FHitSuccessEvent OnHitSuccess;

	DECLARE_MULTICAST_DELEGATE(FReloadEndedEvent);
	FReloadEndedEvent OnReloadEnded;

	DECLARE_MULTICAST_DELEGATE(FEquipFinishedEvent);
	FEquipFinishedEvent OnEquipFinished;

	DECLARE_MULTICAST_DELEGATE(FOnMissionRenewedEvent);
	FOnMissionRenewedEvent OnMissionRenewed;

	/* ------------------------------------------------------------------------------- */
	// Kill Mark
	/* ------------------------------------------------------------------------------- */

	DECLARE_MULTICAST_DELEGATE_TwoParams(FShowKillMarkEvent, const TArray<int32>&, EWeaponType);
	FShowKillMarkEvent OnShowKillMark;

	/* ------------------------------------------------------------------------------- */
	// Situation Board
	/* ------------------------------------------------------------------------------- */

	DECLARE_MULTICAST_DELEGATE(FToggleSituationEvent);
	FToggleSituationEvent OnToggleSituation;

	DECLARE_MULTICAST_DELEGATE_OneParam(FShowSituationEvent, bool);
	FShowSituationEvent OnShowSituation;

	/* ------------------------------------------------------------------------------- */
	// Bombs
	/* ------------------------------------------------------------------------------- */

	DECLARE_MULTICAST_DELEGATE(FBombPlantedEvent);
	FBombPlantedEvent OnBombPlanted;

	DECLARE_MULTICAST_DELEGATE_OneParam(FSyncBombRemainingTimeEvent, float /* Time to explosion */);
	FSyncBombRemainingTimeEvent OnSyncBombRemainingTime;

	DECLARE_MULTICAST_DELEGATE(FBombDefusedEvent);
	FBombDefusedEvent OnBombDefused;

	DECLARE_MULTICAST_DELEGATE(FBombExplodedEvent);
	FBombExplodedEvent OnBombExploded;

	DECLARE_MULTICAST_DELEGATE(FBombFrozenEvent);
	FBombFrozenEvent OnBombFrozen;

	DECLARE_MULTICAST_DELEGATE_OneParam(FCanPlantBombEvent, bool /* Can Plant */);
	FCanPlantBombEvent OnCanPlantBomb;

	DECLARE_MULTICAST_DELEGATE_OneParam(FPlantingBombEvent, float /* Time needed to plant bomb*/);
	FPlantingBombEvent OnPlantingBomb;

	DECLARE_MULTICAST_DELEGATE(FBombPlantingEndEvent);
	FBombPlantingEndEvent OnBombPlantingEnd;

	DECLARE_MULTICAST_DELEGATE(FBombPlantingDoneEvent);
	FBombPlantingDoneEvent OnBombPlantingDone;

	DECLARE_MULTICAST_DELEGATE_OneParam(FCanDefuseBombEvent, bool /* Can Plant */);
	FCanDefuseBombEvent OnCanDefuseBomb;

	DECLARE_MULTICAST_DELEGATE_OneParam(FDefusingBombEvent, float /* Time needed to defuse bomb*/);
	FDefusingBombEvent OnDefusingBomb;

	DECLARE_MULTICAST_DELEGATE(FBombDefusingEndEvent);
	FBombDefusingEndEvent OnBombDefusingEnd;

	DECLARE_MULTICAST_DELEGATE(FBombDefusingDoneEvent);
	FBombDefusingDoneEvent OnBombDefusingDone;

	/* ------------------------------------------------------------------------------- */
	// Spectator
	/* ------------------------------------------------------------------------------- */
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FBecomeFollowerEvent, APBPlayerState *);
	FBecomeFollowerEvent OnBecomeFollower;

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FEnterFollowCameraEvent, APBPlayerState *, bool /* Previous(false) or Next(true) */, bool /* Play Sound */);
	FEnterFollowCameraEvent OnUpdateFollowCamera;

	/* ------------------------------------------------------------------------------- */
	// Pickup
	/* ------------------------------------------------------------------------------- */
	DECLARE_MULTICAST_DELEGATE_OneParam(FEnterOuterPickupVolumeEvent, class APBPickup* /*Pickup*/);
	FEnterOuterPickupVolumeEvent OnEnterOuterPickup;

	DECLARE_MULTICAST_DELEGATE_OneParam(FLeaveOuterPickupVolumeEvent, class APBPickup* /*Pickup*/);
	FLeaveOuterPickupVolumeEvent OnLeaveOuterPickup;

	DECLARE_MULTICAST_DELEGATE_OneParam(FReadyPickupEvent, class APBPickup* /*Pickup*/);
	FReadyPickupEvent OnReadyPickup;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FStartPickupEvent, class APBPickup* /*Pickup*/, float /*KeyDuration*/);
	FStartPickupEvent OnStartPickup;

	DECLARE_MULTICAST_DELEGATE_OneParam(FCancelPickupEvent, class APBPickup* /*Pickup*/);
	FCancelPickupEvent OnCancelPickup;

	DECLARE_MULTICAST_DELEGATE_OneParam(FFinishPickupEvent, class APBPickup* /*Pickup*/);
	FFinishPickupEvent OnFinishPickup;

	/* ------------------------------------------------------------------------------- */
	// End
	/* ------------------------------------------------------------------------------- */
};
