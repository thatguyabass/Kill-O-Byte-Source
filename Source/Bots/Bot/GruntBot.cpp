// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "GruntBot.h"
#include "Utility/AttackTypeDataAsset.h"

AGruntBot::AGruntBot()
	:ABot()
{

}

//void AGruntBot::SetDMIColor()
//{
//	if (DMI.Num() > 0)
//	{
//		for (int32 c = 0; c < DMI.Num(); c++)
//		{
//			DMI[c]->SetVectorParameterValue(PrimaryColorName, GetAttackType().ColorData.PrimaryColor);
//			DMI[c]->SetVectorParameterValue(SecondaryColorName, GetAttackType().ColorData.SecondaryColor);
//			DMI[c]->SetVectorParameterValue(FractureColorName, GetAttackType().ColorData.SecondaryColor);
//		}
//	}
//}