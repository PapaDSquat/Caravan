// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#ifndef __CARAVAN_H__
#define __CARAVAN_H__

#include "EngineMinimal.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

// TODO: Move to editor config
#define CARAVAN_DEBUG 0

// Allows to load an object dynamically at runtime (outside constructor)
template <typename ObjClass>
static FORCEINLINE ObjClass* DynamicLoadObjFromPath(const FName& Path)
{
	if (Path == NAME_None) return NULL;

	return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, *Path.ToString()));
}

#endif
