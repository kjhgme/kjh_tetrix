// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockSpawner.h"
#include "TetrixPlayerController.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
ABlockSpawner::ABlockSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABlockSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlockSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlockSpawner::SpawnBlock()
{
    CheckAndClearLines();

    if (Blocks.Num() == 0) return;

    int32 RandomIndex = FMath::RandRange(0, Blocks.Num() - 1);
    TSubclassOf<AActor> SelectedBlockClass = Blocks[RandomIndex];

    if (!SelectedBlockClass) return; 

    float StartY;
    if (GridSizeX % 2 == 0)
        StartY = -25.0f;
    else
        StartY = 0.0f;

    FVector SpawnLocation = { -100.0f, StartY, 25.0f + 50.0f * (GridSizeY - 1) };
    FRotator SpawnRotation = FRotator::ZeroRotator;

    SpawnedBlock = GetWorld()->SpawnActor<AActor>(SelectedBlockClass, SpawnLocation, SpawnRotation);

    if (SpawnedBlock)
    {
        UE_LOG(LogTemp, Warning, TEXT("Block Spawned: %s"), *SpawnedBlock->GetName());
    }
}

void ABlockSpawner::MoveLeft()
{
    TArray<UActorComponent*> Components;
    SpawnedBlock->GetComponents(Components);

    for (UActorComponent* Component : Components)
    {
        USceneComponent* SceneComponent = Cast<USceneComponent>(Component);
        if (SceneComponent)
        {
            FVector ComponentLocation = SceneComponent->GetComponentLocation();
            const float NewY = ComponentLocation.Y - 50.0f;

            if (ComponentLocation.Y < -(GridSizeX - 2) * 25.0f)
            {
                return;
            }

            for (AActor* PlacedBlock : PlacedBlocks)
            {
                if (!PlacedBlock) continue;

                TArray<UActorComponent*> PlacedComponents;
                PlacedBlock->GetComponents(PlacedComponents);

                for (UActorComponent* PlacedComponent : PlacedComponents)
                {
                    USceneComponent* PlacedSceneComponent = Cast<USceneComponent>(PlacedComponent);
                    if (!PlacedSceneComponent) continue;

                    FVector PlacedLocation = PlacedSceneComponent->GetComponentLocation();

                    if (FMath::IsNearlyEqual(ComponentLocation.Z, PlacedLocation.Z, 0.1f) &&
                        FMath::IsNearlyEqual(NewY, PlacedLocation.Y, 0.1f))
                    {
                        return;
                    }
                }
            }
        }
    }

    FTransform TransformOffset(FRotator::ZeroRotator, FVector(0.0f, -50.0f, 0.0f));
    SpawnedBlock->AddActorWorldTransform(TransformOffset);
}

void ABlockSpawner::MoveRight()
{
    TArray<UActorComponent*> Components;
    SpawnedBlock->GetComponents(Components);

    for (UActorComponent* Component : Components)
    {
        USceneComponent* SceneComponent = Cast<USceneComponent>(Component);
        if (SceneComponent)
        {
            FVector ComponentLocation = SceneComponent->GetComponentLocation();
            const float NewY = ComponentLocation.Y + 50.0f;

            if (ComponentLocation.Y > (GridSizeX - 2) * 25.0f)
            {
                return;
            }

            for (AActor* PlacedBlock : PlacedBlocks)
            {
                if (!PlacedBlock) continue;

                TArray<UActorComponent*> PlacedComponents;
                PlacedBlock->GetComponents(PlacedComponents);

                for (UActorComponent* PlacedComponent : PlacedComponents)
                {
                    USceneComponent* PlacedSceneComponent = Cast<USceneComponent>(PlacedComponent);
                    if (!PlacedSceneComponent) continue;

                    FVector PlacedLocation = PlacedSceneComponent->GetComponentLocation();

                    if (FMath::IsNearlyEqual(ComponentLocation.Z, PlacedLocation.Z, 0.1f) &&
                        FMath::IsNearlyEqual(NewY, PlacedLocation.Y, 0.1f))
                    {
                        return;
                    }
                }
            }
        }
    }

    FTransform TransformOffset(FRotator::ZeroRotator, FVector(0.0f, 50.0f, 0.0f));
    SpawnedBlock->AddActorWorldTransform(TransformOffset);
}

void ABlockSpawner::MoveDown()
{
    if (!SpawnedBlock) return;

    FVector MoveOffset(0.0f, 0.0f, -50.0f);
    FVector NewLocation = SpawnedBlock->GetActorLocation() + MoveOffset;

    TArray<UActorComponent*> Components;
    SpawnedBlock->GetComponents(Components);

    for (UActorComponent* Component : Components)
    {
        // 바닥에 닿을 때
        USceneComponent* SceneComponent = Cast<USceneComponent>(Component);
        if (SceneComponent)
        {
            FVector ComponentLocation = SceneComponent->GetComponentLocation();
            float NewZ = ComponentLocation.Z - 50.0f;

            if (ComponentLocation.Z <= 30.0f)   // 25.0f
            {
                SpawnedBlock->SetActorEnableCollision(true);
                PlacedBlocks.Add(SpawnedBlock);
                SpawnedBlock = nullptr;

                SpawnBlock();

                return;
            }

            // 아래에 블록이 있을 때
            for (AActor* PlacedBlock : PlacedBlocks)
            {
                if (!PlacedBlock) continue;

                TArray<UActorComponent*> PlacedComponents;
                PlacedBlock->GetComponents(PlacedComponents);

                for (UActorComponent* PlacedComponent : PlacedComponents)
                {
                    USceneComponent* PlacedSceneComponent = Cast<USceneComponent>(PlacedComponent);
                    if (!PlacedSceneComponent) continue;

                    FVector PlacedComponentLocation = PlacedSceneComponent->GetComponentLocation();
                    
                    bool bSameX = FMath::IsNearlyEqual(ComponentLocation.X, PlacedComponentLocation.X, 0.1f);
                    bool bSameY = FMath::IsNearlyEqual(ComponentLocation.Y, PlacedComponentLocation.Y, 0.1f);

                    float SpawnedMinZ = NewZ - 25.0f;
                    float SpawnedMaxZ = NewZ + 25.0f;
                    float PlacedMinZ = PlacedComponentLocation.Z - 25.0f;
                    float PlacedMaxZ = PlacedComponentLocation.Z + 25.0f;

                    if (bSameX && bSameY && (SpawnedMaxZ > PlacedMinZ) && (SpawnedMinZ < PlacedMaxZ))
                    {
                        SpawnedBlock->SetActorEnableCollision(true);
                        PlacedBlocks.Add(SpawnedBlock);
                        SpawnedBlock = nullptr;

                        SpawnBlock();
                        return;
                    }
                }
            }
        }        
    }

    SpawnedBlock->AddActorWorldOffset(MoveOffset);
}

void ABlockSpawner::MoveToBottom()
{
}

void ABlockSpawner::Rotate()
{
    SpawnedBlock->AddActorWorldRotation(FRotator(0.0f, 0.0f, 90.0f));

    TArray<UActorComponent*> Components;
    SpawnedBlock->GetComponents(Components);

    for (UActorComponent* Component : Components)
    {
        USceneComponent* SceneComponent = Cast<USceneComponent>(Component);
        if (SceneComponent)
        {
            FVector ComponentLocation = SceneComponent->GetComponentLocation();

            if (ComponentLocation.Y < -(GridSizeX - 1) * 25.0f)
            {
                FTransform TransformOffset(FRotator::ZeroRotator, FVector(0.0f, 50.0f, 0.0f));
                SpawnedBlock->AddActorWorldTransform(TransformOffset);
            }
            else if (ComponentLocation.Y > (GridSizeX) * 25.0f)
            {
                FTransform TransformOffset(FRotator::ZeroRotator, FVector(0.0f, -50.0f, 0.0f));
                SpawnedBlock->AddActorWorldTransform(TransformOffset);
            }
        }
    }
}

void ABlockSpawner::CheckAndClearLines()
{
    // Collect all valid blocks and components
    for (AActor* Block : PlacedBlocks)
    {
        if (!Block) continue;

        ValidBlocks.Add(Block);
        TArray<UActorComponent*> Components;
        Block->GetComponents(Components);

        for (UActorComponent* Comp : Components)
        {
            if (USceneComponent* SceneComp = Cast<USceneComponent>(Comp))
            {
                AllComponents.Add(SceneComp);
            }
        }
    }
    PlacedBlocks = ValidBlocks;

    // Group components by Z coordinate
    TMap<float, TSet<int32>> GridMap;
    for (USceneComponent* Comp : AllComponents)
    {
        FVector Location = Comp->GetComponentLocation();
        int32 XIndex = FMath::RoundToInt(Location.X / 50.0f);
        float Z = Location.Z;

        if (XIndex >= 0 && XIndex < GridSizeX)
        {
            GridMap.FindOrAdd(Z).Add(XIndex);
        }
    }

    // Find complete lines
    TArray<float> FullLines;
    for (const auto& Elem : GridMap)
    {
        if (Elem.Value.Num() == GridSizeX)
        {
            FullLines.Add(Elem.Key);
        }
    }

    if (FullLines.Num() == 0) return;

    // Sort lines from bottom to top
    FullLines.Sort();

    // Destroy components in complete lines
    TArray<USceneComponent*> ToDestroy;
    for (USceneComponent* Comp : AllComponents)
    {
        if (FullLines.Contains(Comp->GetComponentLocation().Z))
        {
            ToDestroy.Add(Comp);
        }
    }

    // Move components above cleared lines down
    const float MoveDistance = FullLines.Num() * 50.0f;
    for (USceneComponent* Comp : AllComponents)
    {
        if (!ToDestroy.Contains(Comp))
        {
            FVector Location = Comp->GetComponentLocation();
            bool bShouldMove = false;

            for (float ClearedZ : FullLines)
            {
                if (Location.Z > ClearedZ)
                {
                    bShouldMove = true;
                    break;
                }
            }

            if (bShouldMove)
            {
                Comp->SetWorldLocation(Location - FVector(0, 0, MoveDistance));
            }
        }
    }

    // Destroy marked components
    for (USceneComponent* Comp : ToDestroy)
    {
        Comp->DestroyComponent();
    }

    //// Cleanup empty actors (NEW CODE)
    //TArray<AActor*> ToRemove;
    //for (AActor* Block : PlacedBlocks)
    //{
    //    if (!Block) continue;

    //    TArray<UActorComponent*> Comps;
    //    Block->GetComponents(Comps);

    //    bool bHasComponents = false;
    //    for (UActorComponent* Comp : Comps)
    //    {
    //        if (Comp)// && !Comp->IsPendingKill())
    //        {
    //            bHasComponents = true;
    //            break;
    //        }
    //    }

    //    if (!bHasComponents)
    //    {
    //        ToRemove.Add(Block);
    //    }
    //}

    //for (AActor* Block : ToRemove)
    //{
    //    PlacedBlocks.Remove(Block);
    //    Block->Destroy();
    //}
}