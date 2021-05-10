// Fill out your copyright notice in the Description page of Project Settings.

#include "RockActor.h"
#include "Caravan.h"

ARockActor::ARockActor(const class FObjectInitializer& ObjInitializer)
	: Super(ObjInitializer, ECraftResourceType::Stone)
{
	ResourceDropCount = 5;
}