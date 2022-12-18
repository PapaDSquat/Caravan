// 2021
#pragma once

/*
*	AI
*/

static TAutoConsoleVariable CVarAIDebug_Profile(
	TEXT("AI.Robot.Profile"),
	false,
	TEXT("Toggle AI Profile HUD Overlay"),
	ECVF_Cheat);

static TAutoConsoleVariable CVarAIDebug_Behaviour(
	TEXT("AI.Robot.Behaviour"),
	false,
	TEXT("Toggle AI Behaviour HUD Overlay"),
	ECVF_Cheat);

/*
*	Interaction
*/

static TAutoConsoleVariable CVarPlayerDebug_ShowInteractionTarget(
	TEXT("Interaction.Target"),
	false,
	TEXT("Toggle target name for Player interaction"),

	ECVF_Cheat);
static TAutoConsoleVariable CVarPlayerDebug_ShowInteractionOverlay(
	TEXT("Interaction.Overlay"),
	false,
	TEXT("Toggle debug overlay for Player interaction"),
	ECVF_Cheat);


/*
*	Caravan
*/

static TAutoConsoleVariable CVarCaravanDebug_Generation(
	TEXT("Caravan.Generation"),
	false,
	TEXT("Toggle debug overlay for Caravan camp generation on open"),
	ECVF_Cheat);

/*
*	RPG
*/
static TAutoConsoleVariable CVarRPGDebug_AlwaysUsePlayerInventory(
	TEXT("RPG.AlwaysUsePlayerInventory"),
	true,
	TEXT("Toggle AI Should store items in Player's inventory rather than personal"),
	ECVF_Cheat);