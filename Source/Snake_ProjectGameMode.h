// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "Spawner/TimeSpawner.h"
#include "Snake_ProjectGameMode.generated.h"

/**
 * 
 */
UENUM()
enum class EGameState : uint8
{
	PreGame,
	Playing,
	GameOver,
	Reset
};

UCLASS()
class SNAKE_PROJECT_API ASnake_ProjectGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ASnake_ProjectGameMode(const class FObjectInitializer& PCIP);

	UPROPERTY(EditAnywhere, Category = GameMode)
	int32 InitialLives;

	UPROPERTY(EditAnywhere, Category = GameMode)
	AActor* PreGameCamera;

	void SetCurrentState(EGameState State);
	EGameState GetCurrentState();

	UPROPERTY()
	bool bGameOver;

	UFUNCTION()
	void PlayerDead(ACharacter* Character);

	UFUNCTION(BlueprintCallable, Category = GameMode_Lives)
	int32 GetLives();

	UFUNCTION(BlueprintCallable, Category = GameMode_Lives)
	void AddLives(int32 NewLives);

	UFUNCTION(BlueprintCallable, Category = GameMode_Lives)
	void RemoveLife();

	UFUNCTION(BlueprintCallable, Category = GameMode_Score)
	int32 GetScore();

	UFUNCTION(BlueprintCallable, Category = GameMode_Score)
	void AddScore(int32 NewScore);

	UFUNCTION(BlueprintCallable, Category = GameMode_Score)
	void ResetScore();

	UFUNCTION(BlueprintCallable, Category = GameMode)
	void PlayAgain();

	UFUNCTION(BlueprintCallable, Category = GameMode)
	void Quit();

	UFUNCTION(BlueprintCallable, Category = GameMode)
	void Respawn();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;	

private:
	int32 Lives;
	int32 Score;
	//const int32 INITIAL_SCORE;

	TArray<ATimeSpawner*> TimeSpawners;

	EGameState CurrentState;

	void HandleNewState();

	//Enable or Disable the Mouse Cursor, Click Events and Mouse Over Events
	void SetMouseState(bool State);

	bool bPlayerDead;
	float RespawnProgress;

	UInputComponent* InputComponent;

};
