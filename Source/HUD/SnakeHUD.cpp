// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SnakeHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "GameMode/GameState/SnakeGameState.h"
#include "SnakeCharacter/SnakeLink.h"
#include "SnakeCharacter/SnakePlayer.h"
//#include "HUD/UI/Widgets/ScoreBoardWidget.h"

#define LOCTEXT_NAMESPACE "Snake_Project.HUD.Menu"

const float ASnakeHUD::MinUIScale = 0.5f;

ASnakeHUD::ASnakeHUD(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	static ConstructorHelpers::FObjectFinder<UFont> BigFontOb(TEXT("/Game/UI/Fonts/TechnoHideo50"));
	static ConstructorHelpers::FObjectFinder<UFont> SmallFontOb(TEXT("/Game/UI/Fonts/TechnoHideo25"));

	static ConstructorHelpers::FObjectFinder<UTexture2D> HealthFillOb(TEXT("/Game/Textures/UI/HealthFill"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> HealthColorOb(TEXT("/Game/Textures/UI/HealthColor"));

	BigFont = BigFontOb.Object;
	SmallFont = SmallFontOb.Object;

	HealthFill = HealthFillOb.Object;
	HealthColor = HealthColorOb.Object;

	DarkFontColor = FColor(50, 50, 50);
	LightFontColor = FColor(100, 100, 100);

	ShadowedText.bEnableShadow = true;
}

void ASnakeHUD::BeginPlay()
{
	Super::BeginPlay();
}

void ASnakeHUD::DrawHUD()
{
	Super::DrawHUD();

	UIScale = Canvas->SizeY / 1080.0f;
	//Ensure the Scale doesn't drop below the min value
	UIScale = FMath::Max(UIScale, MinUIScale);

	DrawMatchTime();
	//DrawPlayerHealth();
	//DrawPowerIcons();
}

void ASnakeHUD::DrawMatchTime()
{
	ASnakeGameState* const MyGameState = Cast<ASnakeGameState>(GetWorld()->GetGameState());
	if (MyGameState && MyGameState->RemainingTime > 0.0f)
	{
		FCanvasTextItem TextItem(FVector2D::ZeroVector, FText(), BigFont, DarkFontColor);
		TextItem.EnableShadow(FLinearColor::Black);
		FString Text;
		float SizeX;
		float SizeY;

		TextItem.FontRenderInfo = ShadowedText;
		TextItem.Scale = FVector2D(1.0f, 1.0f);
		if (MyGameState->GetMatchState() == MatchState::WaitingToStart)
		{
			Text = LOCTEXT("MatchStartsIn", "Match Starts In ").ToString() + FString::FromInt(MyGameState->RemainingTime);
			Canvas->StrLen(BigFont, Text, SizeX, SizeY);
			TextItem.Position = FVector2D((Canvas->SizeX / 2) - (SizeX / 2), 100 * UIScale);
			TextItem.SetColor(LightFontColor);
			TextItem.Text = FText::FromString(Text);
			Canvas->DrawItem(TextItem);
		}
		else if(MyGameState->GetMatchState() == MatchState::InProgress)
		{
			TextItem.Scale = FVector2D(0.75f, 0.75f);
			TextItem.Font = SmallFont;
			Text = LOCTEXT("TimeRemaining", "Time Remaining ").ToString() + GetMatchTime(MyGameState->RemainingTime);
			Canvas->StrLen(SmallFont, Text, SizeX, SizeY);
			TextItem.Position = FVector2D((Canvas->SizeX - (SizeX * 0.75f)) - (20 * UIScale), (Canvas->SizeY - (SizeY * 0.75f)) - (10 * UIScale));
			TextItem.SetColor(DarkFontColor);
			TextItem.Text = FText::FromString(Text);
			Canvas->DrawItem(TextItem);
		}
	}
}

void ASnakeHUD::DrawPlayerHealth()
{
	ASnakeGameState* const MyGameState = Cast<ASnakeGameState>(GetWorld()->GetGameState());
	if (MyGameState && MyGameState->GetMatchState() == MatchState::InProgress)
	{
		ASnakeLink* Link = Cast<ASnakeLink>(GetOwningPawn());
		if (Link && !Link->IsDead())
		{
			float Width = 400.0f;
			float Height = 60.0f;
			float CurrentHealth = Link->GetHealth();
			float MaxHealth = Link->GetTotalHealth();
			float HealthPre = CurrentHealth / MaxHealth;

			float X = Canvas->SizeX / 2 - ((Width * UIScale) / 2);
			float Y = Canvas->SizeY - (20 * UIScale) - (Height * UIScale) ;

			FCanvasTileItem TileItem(FVector2D(0.0f, 0.0f), HealthColor->Resource, FVector2D(0.0f, 0.0f), FVector2D(1.0f, 1.0f), FColor::White);
			TileItem.Texture = HealthColor->Resource;
			TileItem.Position = FVector2D(X, Y);
			TileItem.Size = FVector2D((Width * UIScale) * HealthPre, Height * UIScale);
			Canvas->DrawItem(TileItem);

			TileItem.Texture = HealthFill->Resource;
			X = Canvas->SizeX / 2 - ((1.0f - HealthPre) * (Width * UIScale) - ((Width * UIScale) / 2));
			TileItem.Position = FVector2D(X, Y);
			TileItem.Size = FVector2D((Width * UIScale) * (1.0f - HealthPre), Height * UIScale);
			Canvas->DrawItem(TileItem);

			FCanvasTextItem TextItem(FVector2D::ZeroVector, FText(), BigFont, DarkFontColor);
			FString Text;
			float SizeX;
			float SizeY;

			TextItem.Scale = FVector2D(0.75f, 0.75f);
			TextItem.Font = SmallFont;
			Text = FString::FromInt(Link->GetHealth());
			Canvas->StrLen(SmallFont, Text, SizeX, SizeY);
			X = (Canvas->SizeX / 2) - ((Width * UIScale) / 2) + ((SizeX * UIScale) / 2);
			TextItem.Position = FVector2D(X, Y);
			TextItem.SetColor(DarkFontColor);
			TextItem.Text = FText::FromString(Text);
			Canvas->DrawItem(TextItem);

			Text = FString::FromInt(Link->GetTotalHealth());
			Canvas->StrLen(SmallFont, Text, SizeX, SizeY);
			X = (Canvas->SizeX / 2) + ((Width * UIScale) / 2) - (SizeX * UIScale) - (20 * UIScale);
			TextItem.Position = FVector2D(X, Y);
			TextItem.Text = FText::FromString(Text);
			Canvas->DrawItem(TextItem);
		}
	}
}

//void ASnakeHUD::DrawPowerIcons()
//{
//	ASnakeGameState* const MyGameState = Cast<ASnakeGameState>(GetWorld()->GetGameState());
//	if (MyGameState && MyGameState->GetMatchState() == MatchState::InProgress)
//	{
//		ASnakeLink* Link = Cast<ASnakeLink>(GetOwningPawn());
//		//if (Link && Link->SnakeController && !Link->SnakeController->IsDead())
//		//{
//		//	uint8 Index = (uint8)Link->SnakeController->GetPowerType();
//		//	if (!PowerIcons.IsValidIndex(Index))
//		//	{
//		//		return;
//		//	}
//
//		//	float Width = 200 * UIScale;
//		//	float Height = 200 * UIScale;
//
//		//	float X = Canvas->SizeX - (Width * UIScale) - (75 * UIScale);
//		//	float Y = Canvas->SizeY - (Height * UIScale) - (100 * UIScale);
//
//		//	FCanvasTileItem TileItem(FVector2D(0.0f, 0.0f), HealthColor->Resource, FVector2D(0.0f, 0.0f), FVector2D(1.0f, 1.0f), FColor::White);
//		//	TileItem.Texture = PowerIcons[Index]->Resource;
//		//	TileItem.Position = FVector2D(X, Y);
//		//	TileItem.Size = FVector2D(Width, Height);
//		//	Canvas->DrawItem(TileItem);
//		//}
//	}
//}

FString ASnakeHUD::GetMatchTime(float MatchTime)
{
	const int32 MatchSeconds = FMath::Max(0, FMath::TruncToInt(MatchTime) % 3600);
	const int32 Minutes = MatchSeconds / 60;
	const int32 Seconds = MatchSeconds % 60;
	const FString Time = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

	return Time;
}

//void ASnakeHUD::ShowScoreboard(bool bEnabled)
//{
	//if (bScoreboardVisible == bEnabled)
	//{
	//	if (bEnabled)
	//	{
	//		ToggleScoreboard();
	//	}
	//	else
	//	{
	//		return;
	//	}
	//}

	//if (bEnabled)
	//{
	//	ASnakePlayer* Controller = Cast<ASnakePlayer>(GetOwningPlayerController());
	//	if (!Controller || Controller->IsInGameMenuUp())
	//	{
	//		return;
	//	}
	//}

	//bScoreboardVisible = bEnabled;
	//if (bScoreboardVisible)
	//{
	//	SAssignNew(ScoreboardWidgetOverlay, SOverlay)
	//		+ SOverlay::Slot()
	//		.HAlign(HAlign_Center)
	//		.VAlign(VAlign_Center)
	//		.Padding(FMargin(50))
	//		[
	//			SAssignNew(ScoreboardWidget, SScoreboardWidget)
	//			.PlayerOwner(TWeakObjectPtr<APlayerController>(PlayerOwner))
	//			.MatchState(MatchState)
	//		];

	//	GEngine->GameViewport->AddViewportWidgetContent(SAssignNew(ScoreboardWidgetContainer, SWeakWidget).PossiblyNullContent(ScoreboardWidgetOverlay));
	//}
	//else
	//{
	//	if (ScoreboardWidgetContainer.IsValid())
	//	{
	//		if (GEngine && GEngine->GameViewport)
	//		{
	//			GEngine->GameViewport->RemoveViewportWidgetContent(ScoreboardWidgetContainer.ToSharedRef());
	//		}
	//	}
	//}
//}

//void ASnakeHUD::ToggleScoreboard()
//{
//	ShowScoreboard(!bScoreboardVisible);
//}
//
//void ASnakeHUD::ForceCloseScoreboard()
//{
//	if (bScoreboardVisible)
//	{
//		ShowScoreboard(false);
//	}
//}

bool ASnakeHUD::IsMatchOver() const
{
	return MatchState == ESnakeMatchState::Lost || MatchState == ESnakeMatchState::Won;
}

void ASnakeHUD::SetMatchState(ESnakeMatchState::Type State)
{
	MatchState = State;
}

ESnakeMatchState::Type ASnakeHUD::GetMatchState() const
{
	return MatchState;
}

//bool ASnakeHUD::IsScoreboardVisible()
//{
//	return bScoreboardVisible;
//}

#undef LOCTEXT_NAMESPACE