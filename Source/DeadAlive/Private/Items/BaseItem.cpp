#include "Items/BaseItem.h"
#include "PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundBase.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HUD/WeaponInfoWidget.h"
#include "Components/StaticMeshComponent.h"

ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = true;
	// 루트 - 스켈레탈 메시 - 중력 활성화, 시뮬레이트 활성화, 모든 채널 블록, 카메라는 무시
	SKM = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SetRootComponent(SKM);
	SKM->SetCollisionResponseToAllChannels(ECR_Block);
	SKM->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	SKM->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	SKM->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// (스켈레탈 메시 - 스피어 콜리전) - 오버랩 이벤트 생성, 콜리전 프리셋 OverlapAllDynamic
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(SKM);
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetCollisionObjectType(ECC_WorldDynamic);
	SphereCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	NoticeSphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("NoticeRangeSphere"));
	NoticeSphereCollision->SetupAttachment(GetRootComponent());

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());
	PickupWidget->SetRelativeTransform(FTransform(FVector3d(0.f, 0.f, 90.f)));
}

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnSphereOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnSphereOverlapEnd);
	NoticeSphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::NoticeRangeBeginOverlap);
	NoticeSphereCollision->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::NoticeRangeOverlapEnd);

	// 니 전화 바로 픽업
	// 픽업 위젯 숨기기
	if(PickupWidget)
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(177, 1.f, FColor::Black, TEXT("BaseItem"));
		}
		PickupWidget->SetVisibility(false);
	}
	
	WeaponInfo = Cast<UWeaponInfoWidget>(PickupWidget->GetUserWidgetObject());

	SetActiveStarts();
	
}

// 여기 수정
void ABaseItem::SetActiveStarts()
{
	if(WeaponInfo == nullptr) return;
	// 0번은 사용 안 함
	for(int32 i = 0; i <= 5; i++)
	{
		ActiveStarts.Add(false);
	}

	switch(ItemRarity)
	{
	case EItemRarity::EIR_Damaged:
		ActiveStarts[1] = true;
		WeaponInfo->SetStarIcon(1);
		break;
	case EItemRarity::EIR_Common:
		ActiveStarts[1] = true;
		ActiveStarts[2] = true;
		WeaponInfo->SetStarIcon(2);
		break;
	case EItemRarity::EIR_Uncommon:
		ActiveStarts[1] = true;
		ActiveStarts[2] = true;
		ActiveStarts[3] = true;
		WeaponInfo->SetStarIcon(3);
		break;
	case EItemRarity::EIR_Rare:
		ActiveStarts[1] = true;
		ActiveStarts[2] = true;
		ActiveStarts[3] = true;
		ActiveStarts[4] = true;
		WeaponInfo->SetStarIcon(4);
		break;
	case EItemRarity::EIR_Legendary:
		ActiveStarts[1] = true;
		ActiveStarts[2] = true;
		ActiveStarts[3] = true;
		ActiveStarts[4] = true;
		ActiveStarts[5] = true;
		WeaponInfo->SetStarIcon(5);
		break;
	}
}

void ABaseItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	IPickupInterface* Interface = Cast<IPickupInterface>(OtherActor);

	if(Interface)
	{
		Interface->OverlappingItem(this);
	}
}

// 주변에 플레이어가 들어왔을 때 위젯 갱신
void ABaseItem::NoticeRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//★☆★☆★★☆BaseWeapon 클래스에서 overloading 필요함☆★☆★☆★☆
	if(OtherActor)
	{
		/*WeaponInfo = Cast<UWeaponInfoWidget>(PickupWidget->GetUserWidgetObject());
		if(WeaponInfo)
		{
			ABaseWeapon* Weapon = Cast<ABaseWeapon>(this);
			/*if(Weapon->GetWeaponAttributes()->GetAmmoCount())
			{
				// 총알 개수를 갱신해준다.
				WeaponInfo->SetAmmo(Weapon->GetWeaponAttributes()->GetAmmoCount());
			}
		}*/
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
		if(Player)
		{
			Player->IncrementOverlappedItemCount(1);
		}
	}
}

void ABaseItem::NoticeRangeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
		if(Player)
		{
			Player->IncrementOverlappedItemCount(-1);
		}
	}
}

void ABaseItem::ThrowTheWeapon()
{
	const FVector MeshForward { SKM->GetForwardVector() };
	const FVector ThrowDirection = MeshForward.RotateAngleAxis(-50.f, MeshForward);
	SKM->AddImpulse(ThrowDirection*1500.f);
}

void ABaseItem::PlayWeaponPickupSound()
{
	UGameplayStatics::PlaySoundAtLocation(this, EquipSound, this->GetActorLocation(), this->GetActorRotation());
}

// 오버라이드해서 사용할거임
/*void ABaseItem::PickUpItem(APlayerCharacter* Player)
{
	PlayWeaponPickupSound();
}*/

void ABaseItem::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IPickupInterface* Interface = Cast<IPickupInterface>(OtherActor);
	if(Interface)
	{
		Interface->OverlappingItem(nullptr);
	}
}

void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseItem::IPickUpItem(ABaseItem* Item, APlayerCharacter* Player)
{
	IPickupInterface::IPickUpItem(Item, Player);
	PlayWeaponPickupSound();
}

