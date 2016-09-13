// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "GameMode_Arcade.h"
#include "GameMode/GameState/SnakeGameState.h"
#include "Bots/BotManager.h"
#include "SnakeCharacter/SnakePlayer.h"
#include "HUD/Controllers/ArcadeScore.h"
#include "HUD/Controllers/LevelListDataAsset.h"

AGameMode_Arcade::AGameMode_Arcade(const FObjectInitializer& PCIP)
	: ABaseGameMode(PCIP)
{
	StartingLives = 3;
	RoundTime = 0.0f;
	PostGameTime = 0.0f;
	bDelayedStart = false;

	RoundDuration = 30.0f;
	MaxPowerLevel = 10;

	GameModeID = 1;

	GruntKills = 0;
	DroneKills = 0;
	TankKills = 0;
	SuperGruntKills = 0;
	SeekerKills = 0;
	ShieldKills = 0;
	MortarKills = 0;
	BeamKills = 0;

	static ConstructorHelpers::FObjectFinder<UArcadeScore> ArcadeScoreDataAssetObject(TEXT("/Game/Blueprints/UI/ArcadeScore"));
	if (ArcadeScoreDataAssetObject.Object)
	{
		ArcadeScoreDataAsset = ArcadeScoreDataAssetObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<ULevelListDataAsset> LevelListDataAssetObject(TEXT("/Game/Blueprints/UI/ArcadeLevelList"));
	if (LevelListDataAssetObject.Object)
	{
		LevelListDataAsset = LevelListDataAssetObject.Object;
	}
}

void AGameMode_Arcade::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	GetWorldTimerManager().SetTimer(WaveTick_TimerHandle, this, &AGameMode_Arcade::WaveTick, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}

void AGameMode_Arcade::WaveTick()
{
	ASnakeGameState* MyGameState = Cast<ASnakeGameState>(GameState);
	int32 PlayersAlive = 0;
	for (int32 c = 0; c < MyGameState->PlayerArray.Num(); c++)
	{
		ASnakePlayerState* PlayerState = Cast<ASnakePlayerState>(MyGameState->PlayerArray[c]);
		if (PlayerState)
		{
			//Check if the players have enough remaining lives
			if (PlayerState->GetPlayerLives() > 0)
			{
				PlayersAlive++;
			}
		}
	}

	if (PlayersAlive == 0 || GetWaveIndex() >= GetMaxWaveIndex())
	{
		FinishMatch();
	}
}

void AGameMode_Arcade::HandleMatchHasStarted()
{
	ABaseGameMode::HandleMatchHasStarted();

	// Move this to a better location later
	FindBotManager();

	ASnakeGameState* MyGameState = Cast<ASnakeGameState>(GameState);
	for (int32 c = 0; c < MyGameState->PlayerArray.Num(); c++)
	{
		//Initialize the players lives
		ASnakePlayerState* Player = Cast<ASnakePlayerState>(MyGameState->PlayerArray[c]);
		Player->SetPlayerLives(StartingLives);
	}

	for (FConstPlayerControllerIterator Controller = GetWorld()->GetPlayerControllerIterator(); Controller; Controller++)
	{
		ASnakePlayer* Player = Cast<ASnakePlayer>(*Controller);
		if (Player)
		{
			//Unlock all the abilities, but dont save
			Player->UnlockAllAbilitiesWithoutSave();
		}
	}
}

void AGameMode_Arcade::FindBotManager()
{
	for (TActorIterator<ABotManager> Iter(GetWorld()); Iter; ++Iter)
	{
		if (*Iter)
		{
			BotManager = (*Iter);
			return;
		}
	}
}

void AGameMode_Arcade::IncreasePowerLevel()
{
	if (GetMatchState() == MatchState::InProgress)
	{
		PowerLevel++;
		if (PowerLevel > MaxPowerLevel)
		{
			PowerLevel = MaxPowerLevel;
		}
	}
}

void AGameMode_Arcade::FinishMatch()
{
	if (BotManager)
	{
		BotManager->EndMatch();
	}

	GetWorldTimerManager().ClearTimer(WaveTick_TimerHandle);

	USnakeLocalPlayer* const LocalPlayer = Cast<USnakeLocalPlayer>(GetWorld()->GetGameInstance()->GetFirstGamePlayer());
	if (LocalPlayer)
	{
		//Reload the Old User Data 
		LocalPlayer->ReloadUserData();

		UUserData* Data = LocalPlayer->GetUserData();
		if (Data)
		{
			int32 WaveIndex = GetWaveIndex();
			int32 LevelIndex = LevelListDataAsset->GetLevelIndex();
			bool bDiff = Data->GetDifficulty();

			int32 Score = ArcadeScoreDataAsset->GetScore(WaveIndex);
			if (bDiff)
			{
				Data->SetWaveHard(LevelIndex, WaveIndex);
				Data->SetArcadeHardScores(LevelIndex, Score);
			}

			Data->SetWaveNormal(LevelIndex, WaveIndex);
			Data->SetArcadeNormalScores(LevelIndex, Score);
		
			Data->SaveIfDirty();

			TriggerEndgameScorescreen(Score);
		}
	}

	Super::FinishMatch();
}

int32 AGameMode_Arcade::GetLevelIndex() const
{
	return LevelListDataAsset->GetLevelIndex();
}

int32 AGameMode_Arcade::GetWaveIndex() const
{
	return BotManager ? BotManager->WaveIndex : -1;
}

int32 AGameMode_Arcade::GetMaxWaveIndex() const
{
	return BotManager ? BotManager->MaxWaveIndex : -1;
}

int32 AGameMode_Arcade::GetGruntKills() const
{
	return GruntKills;
}

int32 AGameMode_Arcade::GetDroneKills() const
{
	return DroneKills;
}

int32 AGameMode_Arcade::GetTankKills() const
{
	return TankKills;
}

int32 AGameMode_Arcade::GetSuperGruntKills() const
{
	return SuperGruntKills;
}

int32 AGameMode_Arcade::GetSeekerKills() const
{
	return SeekerKills;
}

int32 AGameMode_Arcade::GetShieldKills() const
{
	return ShieldKills;
}

int32 AGameMode_Arcade::GetMortarKills() const
{
	return MortarKills;
}

int32 AGameMode_Arcade::GetBeamKills() const
{
	return BeamKills;
}

void AGameMode_Arcade::AddBotKill(uint8 BotClassType)
{
	EBotClass BotClass = TEnumAsByte<EBotClass>(BotClassType);

	switch (BotClass)
	{
	case EBotClass::Grunt: GruntKills++; break;
	case EBotClass::Drone: DroneKills++; break;
	case EBotClass::Tank: TankKills++; break;
	case EBotClass::Medium: SuperGruntKills++; break;
	case EBotClass::Seeker: SeekerKills++; break;
	case EBotClass::Shield: ShieldKills++; break;
	case EBotClass::Mortar: MortarKills++; break;
	case EBotClass::Lazer: BeamKills++; break;
	}
}