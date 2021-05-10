// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeActor.h"
#include "Caravan.h"

ATreeActor::ATreeActor(const class FObjectInitializer& ObjInitializer)
	: Super(ObjInitializer, ECraftResourceType::Wood)
{
	ResourceDropCount = 3;
}