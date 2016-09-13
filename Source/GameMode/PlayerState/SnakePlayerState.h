// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "GameFramework/PlayerState.h"
#include "SnakePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ASnakePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ASnakePlayerState(const class FObjectInitializer& PCIP);
	
	/** Clear Scores */
	virtual void Reset() override;

	/** Set the Team and initialize the player state */
	virtual void ClientInitialize(class AController* InController) override;

	/** Remove the player from the session */
	virtual void UnregisterPlayerWithSession() override;

	/** Set the Player Team Number. This will also update the players team color */
	void SetTeamNumber(int32 NetTeamMember);
	
	/** Set the Team Color Data */
	void SetTeamColor(FTeamColorData InData);

	/** Set If this player is a quitter */
	void SetQuitter(bool bIsQuitter);

	/** Set the number of lives */
	void SetPlayerLives(int32 InLives);

	/** Reduce Player Lives by Value */
	void ReducePlayerLives(int32 Value = 1);

	/** Increase Player Lives by Value */
	void IncreasePlayerLives(int32 Value = 1);

	/** Player Killed another Player */
	void ScoreSnakeKill(int32 Points);

	/** Player Killed a Link */
	void ScoreLinkKill(int32 Points);

	/** Player has beed killed */
	void ScoreDeath(int32 Points);

	/** Player has killed a bot */
	void ScoreBotKill(int32 Points);

	/** Player has picked up a pickup */
	void ScorePickup(int32 Points);

	/** Player has Scored Misc points (i.e Destroyed a wall) */
	void ScoreMisc(int32 Points);

	/** Get the Number of lives */
	UFUNCTION(BlueprintCallable, Category = "State")
	int32 GetPlayerLives() const;

	/** Get the Team Number */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	int32 GetTeamNumber() const;

	/** Get the Team Color */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FTeamColorData GetTeamColor() const;

	/** Get Snake Kills */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	int32 GetSnakeKills() const;

	/** Get Link Kills */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	int32 GetLinkKills() const;

	/** Get Deaths */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	int32 GetDeaths() const;

	/** Get Bot Kills */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	int32 GetBotKills() const;

	/** Get Pickup Score. */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	int32 GetPickups() const;

	/** Get Misc Score */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	int32 GetMiscScore() const;

	/** Get Score */
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetScore() const;

	/** Get Quitter State */
	bool IsQuitter() const;

	virtual void CopyProperties(class APlayerState* PlayerState) override;

protected:
	/** Player Lives */
	UPROPERTY(Transient)
	int32 Lives;

	/** Team Number */
	UPROPERTY(Transient, Replicated)
	int32 TeamNumber;

	/** Team Color */
	UPROPERTY(Transient, Replicated)
	FTeamColorData TeamColor;

	/** Total Snake Kills */
	UPROPERTY(Transient, Replicated)
	int32 SnakeKillsCount;

	/** Total Links Killed */
	UPROPERTY(Transient, Replicated)
	int32 LinkKillsCount;

	/** Total Deaths */
	UPROPERTY(Transient, Replicated)
	int32 DeathCount;

	/** Bots Killed */
	UPROPERTY(Transient, Replicated)
	int32 BotKillsCount;

	UPROPERTY(Transient, Replicated)
	int32 PickupCount;

	UPROPERTY(Transient)
	int32 MiscCount;

	/** Has this player quit the match? */
	UPROPERTY()
	uint8 bQuitter : 1;

	/** Add to the Score */
	void AddScore(int32 Points);

};
