// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SnakePlayer.h"
#include "Online.h"
#include "Online/SnakeInstance.h"
#include "SnakeLink.h"
#include "SnakeLinkHead.h"
#include "GameMode/PlayerState/SnakePlayerState.h"
#include "UserData/UserData.h"
#include "UserData/SnakeLocalPlayer.h"
#include "GameMode/GameMode_Arcade.h"

ASnakePlayer::ASnakePlayer(const class FObjectInitializer& PCIP)
	: AMasterController(PCIP)
{
	bAutoManageActiveCameraTarget = true;

	RespawnTime = 2.0f;

	bPawnInvulnerable = false;
	InvulnerableProgress = 0.0f;
	InvulnerableDuration = 10.0f;

	XSensitivity = 1.0f;
	YSensitivity = 1.0f;

	bLevelCompleted = false;
	bRepeatTutorial = false;

	ScoreProgressDuration = 3.0f;

	CameraRotation = FRotator::ZeroRotator;

	bPlaySpawnEffect = false;

	PrimaryActorTick.bCanEverTick = true;

	CameraBoomHeight = 3500.0f;
}

void ASnakePlayer::BeginPlay()
{
	Super::BeginPlay();

	AddMiscInfoWidget();
}

void ASnakePlayer::SetupInputComponent()
{
	Super::SetupInputComponent();

	FInputActionBinding& InGameAction = InputComponent->BindAction("InGameMenu", IE_Pressed, this, &ASnakePlayer::ToggleInGameMenu);
	InGameAction.bExecuteWhenPaused = true;

	//InputComponent->BindAction("ToggleScoreboard", IE_Pressed, this, &ASnakePlayer::ToggleScoreboard);
	FInputActionBinding& CancelAction = InputComponent->BindAction("Cancel", IE_Pressed, this, &ASnakePlayer::HideInGameMenu);
	CancelAction.bExecuteWhenPaused = true;
	CancelAction.bConsumeInput = false;
}

void ASnakePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPawnInvulnerable)
	{
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, "SnakePlayer.cpp(50): God Mode Activated");
		InvulnerableProgress += DeltaTime;
		if (InvulnerableProgress > InvulnerableDuration)
		{
			EndInvulnerablePhase();
		}
	}
}

void ASnakePlayer::Possess(class APawn* InPawn)
{
	Super::Possess(InPawn);

	//Spawn HUD
	ShowHUD();

	SetViewTarget(InPawn);
}

void ASnakePlayer::UnPossess()
{
	Super::UnPossess();
}

void ASnakePlayer::PawnPendingDestroy(APawn* Pawn)
{
	AGameMode* Mode = GetWorld()->GetAuthGameMode();
	if (Mode && Mode->GetMatchState() == MatchState::InProgress)
	{
		GetWorld()->GetTimerManager().SetTimer(Respawn_TimerHandle, this, &ASnakePlayer::Respawn, RespawnTime, false);
	}

	HideHUD();

	EndInvulnerablePhase();

	FVector DeathCameraLocation = FVector::ZeroVector;
	
	ASnakeLinkHead* Link = Cast<ASnakeLinkHead>(Pawn);
	if (Link)
	{
		DeathCameraLocation = Link->CameraComp->GetComponentLocation();
		CameraRotation.Pitch = Link->CameraComp->GetComponentRotation().Pitch;
	}

	Super::PawnPendingDestroy(Pawn);
	ClientSetSpectatorCamera(DeathCameraLocation, CameraRotation);
}

void ASnakePlayer::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);
}

void ASnakePlayer::Respawn()
{
	ServerRestartPlayer();
	StartInvulnerablePhase();
}

bool ASnakePlayer::IsPawnInvulnerable() const
{
	return bPawnInvulnerable;
}

void ASnakePlayer::SetGodMode()
{
	bPawnInvulnerable = bPawnInvulnerable ? false : true;
}

void ASnakePlayer::StartSpawnEffect(bool InDirection)
{
	ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(GetPawn());
	if (!Head)
	{
		return;
	}

	bPlaySpawnEffect = true;

	Head->StartSpawnEffect(InDirection);
}

void ASnakePlayer::StartInvulnerablePhase()
{
	//Do not start again
	if (bPawnInvulnerable)
	{
		return;
	}

	bPawnInvulnerable = true;
	InvulnerableProgress = 0.0f;

	ASnakeLinkHead* Link = Cast<ASnakeLinkHead>(GetPawn());
	if (Link)
	{
		Link->StartMaterialFlicker();
	}
}

void ASnakePlayer::EndInvulnerablePhase()
{
	bPawnInvulnerable = false;
	InvulnerableProgress = 0.0f;

	ASnakeLinkHead* Link = Cast<ASnakeLinkHead>(GetPawn());
	if (Link)
	{
		Link->EndMaterialFlicker();
	}
}

void ASnakePlayer::HideHUD()
{
	if (HUDWidget && HUDWidget->IsInViewport())
	{
		RemoveHUD();
	}
}

void ASnakePlayer::ShowHUD()
{
	if (!HUDWidget && GetPawn())
	{
		AddHUD();
	}
}

void ASnakePlayer::RemoveHUD()
{
	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
		HUDWidget = nullptr;
	}
	if (AdditionalHUDWidget)
	{
		AdditionalHUDWidget->RemoveFromParent();
		AdditionalHUDWidget = nullptr;
	}
}

void ASnakePlayer::ToggleInGameMenu()
{
	HideScoreboard();
	
	if (InGameMenu && InGameMenu->IsInViewport())
	{
		bPlayerInMenu = false;
		RemoveInGameMenu();
	}
	else
	{
		bPlayerInMenu = true;
		AddInGameMenu();
	}
}

void ASnakePlayer::RemoveInGameMenu()
{
	if (InGameMenu)
	{
		UGameplayStatics::SetGamePaused(this, false);

		InGameMenu->RemoveFromParent();
		InGameMenu = nullptr;
	}
}

void ASnakePlayer::HideInGameMenu()
{
	if (InGameMenu && InGameMenu->IsInViewport())
	{
		RemoveInGameMenu();
	}
}

void ASnakePlayer::RemoveEndGameScoreboard()
{
	if (EndgameScoreboard)
	{
		EndgameScoreboard->RemoveFromParent();
		EndgameScoreboard = nullptr;
	}
}

void ASnakePlayer::ReturnToMainMenu(bool ShowCredits)
{
	if (InGameMenu && InGameMenu->IsInViewport())
	{
		RemoveInGameMenu();
	}

	if (EndgameScoreboard && EndgameScoreboard->IsInViewport())
	{
		RemoveEndGameScoreboard();
	}

	bShowCredits = ShowCredits;

	GetWorld()->ForceGarbageCollection(true);

	USnakeInstance* const Instance = Cast<USnakeInstance>(GetGameInstance());
	if (Instance)
	{
		Instance->GoToState(SGameInstanceState::MainMenu);
	}
}

void ASnakePlayer::ToggleScoreboard()
{
	if (Scoreboard && Scoreboard->IsInViewport())
	{
		RemoveScoreboard();
	}
	else
	{
		AddScoreboard();
	}
}

void ASnakePlayer::HideScoreboard()
{
	if (Scoreboard && Scoreboard->IsInViewport())
	{
		RemoveScoreboard();
	}
}
void ASnakePlayer::ShowScoreboard()
{
	if (!Scoreboard)
	{
		AddScoreboard();
	}
}

void ASnakePlayer::RemoveNullStateWidget()
{
	if (NullStateWidget)
	{
		NullStateWidget->RemoveFromParent();
		NullStateWidget = nullptr;
	}
}

void ASnakePlayer::RemoveBossBarWidget()
{
	if (BossBarWidget)
	{
		BossBarWidget->RemoveFromParent();
		BossBarWidget = nullptr;
	}
}

void ASnakePlayer::HandleReturnToMainMenu()
{
	HideScoreboard();
	Super::HandleReturnToMainMenu();
}

void ASnakePlayer::HandleNextLevelTransition()
{
	HandleReturnToMainMenu();
}

void ASnakePlayer::ClientStartGame_Implementation()
{
	Super::ClientStartGame_Implementation();
}

void ASnakePlayer::ClientGameEnded_Implementation(AActor* EndGameFocus, bool bIsWinner)
{
	Super::ClientGameEnded_Implementation(EndGameFocus, bIsWinner);

	AGameMode_Arcade* GameMode = Cast<AGameMode_Arcade>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		FVector Loc = DeathCamOffset;
		Loc.Z += CameraBoomHeight;
		ClientSetSpectatorCamera(Loc, CameraRotation);
	}
	
	GetWorldTimerManager().ClearTimer(Respawn_TimerHandle);

	SetViewTarget(GetPawn());
}

ASnakeHUD* ASnakePlayer::GetSnakeHUD() const
{
	return Cast<ASnakeHUD>(GetHUD());
}

void ASnakePlayer::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);

	//Loading Screen
	USnakeInstance* Instance = Cast<USnakeInstance>(GetGameInstance());
	if (ensure(Instance))
	{
		Instance->ShowLoadingScreen();
	}

	RemoveAllWidgets();

	if (Scoreboard)
	{
		Scoreboard->RemoveFromParent();
		Scoreboard = nullptr;
	}
}

void ASnakePlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetSensitivitySettings();
}

void ASnakePlayer::SetTutorialRepeat(bool First, bool Repeat)
{
	bLevelCompleted = First;
	bRepeatTutorial = Repeat;
}

bool ASnakePlayer::GetCurrentLevelCompleted() const
{
	return bLevelCompleted;
}

int32 ASnakePlayer::GetDeathCount()
{
	ensure(PlayerState);

	ASnakePlayerState* State = Cast<ASnakePlayerState>(PlayerState);
	return State->GetDeaths();
}

void ASnakePlayer::AddScoreProgressWidget(UUserWidget* InScoreProgress)
{
	if (InScoreProgress)
	{
		ScoreProgressWidget = InScoreProgress;

		GetWorldTimerManager().SetTimer(ScoreProgress_TimerHandle, this, &ASnakePlayer::RemoveScoreProgressWidget, ScoreProgressDuration, false);
	}
}

void ASnakePlayer::RemoveScoreProgressWidget()
{
	if (ScoreProgressWidget)
	{
		ScoreProgressWidget->RemoveFromParent();
		ScoreProgressWidget = nullptr;
	}
}

void ASnakePlayer::RemoveMiscInfoWidget()
{
	if (MiscInfoWidget)
	{
		MiscInfoWidget->RemoveFromParent();
		MiscInfoWidget = nullptr;
	}
}

void ASnakePlayer::SuppressWidgets()
{
	RemoveHUD();
	RemoveMiscInfoWidget();
}

void ASnakePlayer::RestoreWidgets()
{
	ShowHUD();
	AddMiscInfoWidget();
}

void ASnakePlayer::BlockPlayerInput(bool InInput)
{
	ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(GetPawn());
	if (Head)
	{
		Head->bSuppressInput = InInput;
	}
}

void ASnakePlayer::RemoveIntroWidget()
{
	if (IntroWidget)
	{
		IntroWidget->RemoveFromParent();
		IntroWidget = nullptr;
	}
}

void ASnakePlayer::RemoveAllWidgets()
{
	RemoveInGameMenu();
	RemoveEndGameScoreboard();
	RemoveHUD();
	RemoveNullStateWidget();
	RemoveScoreProgressWidget();
	RemoveMiscInfoWidget();
	RemoveBossBarWidget();
	RemoveIntroWidget();
	RemoveAllOnScreenEffectWidgets();
}

void ASnakePlayer::GetSensitivitySettings()
{
	if (!GConfig)
	{
		return;
	}

	float X = 0.0f;
	float Y = 0.0f;

	GConfig->GetFloat(TEXT("/Script/Snake_Project.GeneralSettings"), TEXT("MouseSensitivityX"), X, GGameIni);
	GConfig->GetFloat(TEXT("/Script/Snake_Project.GeneralSettings"), TEXT("MouseSensitivityY"), Y, GGameIni);

	ClientMessage(FString::SanitizeFloat(X));
	ClientMessage(FString::SanitizeFloat(Y));

	XSensitivity = X;
	YSensitivity = Y;
}

void ASnakePlayer::SetInputSettings(bool Value)
{
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		SavedData->SetInputSettings(Value);
		SavedData->SaveIfDirty();

		InputSettingsChanged();

		ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(GetPawn());
		if (Head)
		{
			Head->RetrieveControllerSettings();
		}
	}
}

void ASnakePlayer::SetTwinStickMode(bool Value)
{
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		SavedData->SetTwinStickMode(Value);
		SavedData->SaveIfDirty();

		ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(GetPawn());
		if (Head)
		{
			Head->RetrieveControllerSettings();
		}
	}
}

bool ASnakePlayer::GetTwinStickMode() const
{
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		return SavedData->GetTwinStickMode();
	}

	return false;
}

bool ASnakePlayer::GetAttackType02Unlocked() const
{
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		return SavedData->GetAttack02Unlocked();
	}

	return false;
}

void ASnakePlayer::SetAttackType02Unlocked(bool Value, bool Save)
{
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		SavedData->SetAttack02Unlocked(Value);
		if (Save)
		{
			SavedData->SaveIfDirty();
		}
	}
}

bool ASnakePlayer::GetAttackType03Unlocked() const
{
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		return SavedData->GetAttack03Unlocked();
	}

	return false;
}

void ASnakePlayer::SetAttackType03Unlocked(bool Value, bool Save)
{
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		SavedData->SetAttack03Unlocked(Value);
		if (Save)
		{
			SavedData->SaveIfDirty();
		}
	}
}

bool ASnakePlayer::GetAttackType04Unlocked() const
{
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		return SavedData->GetAttack04Unlocked();
	}

	return false;
}

void ASnakePlayer::SetAttackType04Unlocked(bool Value, bool Save)
{
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		SavedData->SetAttack04Unlocked(Value);
		if (Save)
		{
			SavedData->SaveIfDirty();
		}
	}
}

bool ASnakePlayer::GetFocusShapeUnlocked() const
{
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		return SavedData->GetFocusShapeUnlocked();
	}

	return false;
}

void ASnakePlayer::SetFocusShapeUnlocked(bool Value, bool Save)
{
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		SavedData->SetFocusShapeUnlocked(Value);
		if (Save)
		{
			SavedData->SaveIfDirty();
		}
	}
}

bool ASnakePlayer::GetLazerShapeUnlocked() const
{
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		return SavedData->GetLazerShapeUnlocked();
	}

	return false;
}

void ASnakePlayer::SetLazerShapeUnlocked(bool Value, bool Save)
{
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		SavedData->SetLazerShapeUnlocked(Value);
		if (Save)
		{
			SavedData->SaveIfDirty();
		}
	}
}

bool ASnakePlayer::GetSpeedShapeUnlocked() const
{
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		return SavedData->GetSpeedShapeUnlocked();
	}

	return false;
}

void ASnakePlayer::SetSpeedShapeUnlocked(bool Value, bool Save)
{
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		SavedData->SetSpeedShapeUnlocked(Value);
		if (Save)
		{
			SavedData->SaveIfDirty();
		}
	}
}

bool ASnakePlayer::GetShieldShapeUnlocked() const
{
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		return SavedData->GetShieldShapeUnlocked();
	}

	return false;
}

void ASnakePlayer::SetShieldShapeUnlocked(bool Value, bool Save)
{
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		SavedData->SetShieldShapeUnlocked(Value);
		if (Save)
		{
			SavedData->SaveIfDirty();
		}
	}
}

void ASnakePlayer::HardResetAbilities()
{
	SetAttackType02Unlocked(false);
	SetAttackType03Unlocked(false);
	SetAttackType04Unlocked(false);

	SetFocusShapeUnlocked(false);
	SetLazerShapeUnlocked(false);
	SetSpeedShapeUnlocked(false);
	SetShieldShapeUnlocked(false);
}

void ASnakePlayer::UnlockAllAbilities()
{
	SetAttackType02Unlocked(true);
	SetAttackType03Unlocked(true);
	SetAttackType04Unlocked(true);

	SetFocusShapeUnlocked(true);
	SetLazerShapeUnlocked(true);
	SetSpeedShapeUnlocked(true);
	SetShieldShapeUnlocked(true);
}

void ASnakePlayer::UnlockAllAbilitiesWithoutSave()
{
	SetAttackType02Unlocked(true, false);
	SetAttackType03Unlocked(true, false);
	SetAttackType04Unlocked(true, false);

	SetFocusShapeUnlocked(true, false);
	SetLazerShapeUnlocked(true, false);
	SetSpeedShapeUnlocked(true, false);
	SetShieldShapeUnlocked(true, false);
}