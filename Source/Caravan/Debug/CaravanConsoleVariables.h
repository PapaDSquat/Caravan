// 2021
#pragma once

/*
*	AI
*/

static TAutoConsoleVariable<bool> CVarAIDebug_Profile(
	TEXT("AI.Robot.Profile"),
	false,
	TEXT("Toggle AI Profile HUD Overlay"),
	ECVF_Cheat);

static TAutoConsoleVariable<bool> CVarAIDebug_Behaviour(
	TEXT("AI.Robot.Behaviour"),
	false,
	TEXT("Toggle AI Behaviour HUD Overlay"),
	ECVF_Cheat);

/*
*	Player
*/

static TAutoConsoleVariable<bool> CVarPlayerDebug_ShowInteractionDebug(
	TEXT("Player.ShowInteractionDebug"),
	false,
	TEXT("Toggle debug overlay for Player interaction"),
	ECVF_Cheat);

/*
*	RPG
*/
static TAutoConsoleVariable<bool> CVarRPGDebug_AlwaysUsePlayerInventory(
	TEXT("RPG.AlwaysUsePlayerInventory"),
	true,
	TEXT("Toggle AI Should store items in Player's inventory rather than personal"),
	ECVF_Cheat);