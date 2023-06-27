#include "Items/BaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Particles/ParticleSystem.h"
#include "Attributes/CharacterAttribute.h"
#include "Particles/ParticleSystemComponent.h"
#include "Attributes/WeaponAttributes.h"
#include "PlayerCharacter.h"
#include "Components/Image.h"
#include "Enemy/EnemyCharacter.h"
#include "Engine/DamageEvents.h"
#include "HUD/WeaponInfoWidget.h"

void ABaseWeapon::IPickUpItem(ABaseWeapon* Weapon)
{
	IPickupInterface::IPickUpItem(Weapon);
	UE_LOG(LogTemp, Warning, TEXT("그거받음ㅋ"));
}

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	FirePoint = CreateDefaultSubobject<USceneComponent>(TEXT("FirePoint"));
	FirePoint->SetupAttachment(GetRootComponent());
	WeaponAttributes = CreateDefaultSubobject<UWeaponAttributes>(TEXT("WeaponAttributes"));
	
	SetCollisionWhenItemDivestiture();
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	// 아이템 이름 초기화
	if(WeaponInfo && WeaponAttributes)
	{
		WeaponInfo->SetItemName(WeaponAttributes->GetItemName());
	}
	DisableCustomDepth();
}

// 아이템을 버림
void ABaseWeapon::ItemDivestiture()
{
	SetCollisionWhenItemDivestiture();
	const FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, false);
	this->DetachFromActor(DetachmentTransformRules);

	// 형광 효과 다시 활성화
	// EnableCustomDepth();
}

// 아이템을 플레이어가 주울 때, 콜리전과 시뮬레이트 비활성화, 소켓에 붙임, 아이템 상태 변경 ★ NewWeapon ★
void ABaseWeapon::ItemAcquisition(USceneComponent* InParent, AActor* NewOwner)
{

	// 바닥에서 습득한 무기라면 캐릭터 소켓에 부착해야함
	if(this->Owner == nullptr)
	{
		// 캐릭터 무기 소켓에 부착하기
		const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		this->AttachToComponent(InParent, AttachmentRules, FName("RightHandWeaponSocket"));
	}
	
	// 아이템 피직스와 콜라이더 설정 변경
	this->Owner = NewOwner;
	SetCollisionWhenItemChangeOFF();
	/*Skm->SetSimulatePhysics(false);
	Skm->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NoticeSphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);*/

	Skm->SetVisibility(true);
	SetItemState(EItemState::EIS_Equipped);
}

// 변경될 무기 (Old Weapon) : 슬롯에 들어갈 무기를 말함 ★ Old Weapon ★
void ABaseWeapon::SetCollisionWhenItemChangeOFF()
{
	// 물리 효과, OverlapSphere들 전부 끄고
	Skm->SetSimulatePhysics(false);
	Skm->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Skm->SetVisibility(false);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NoticeSphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 아이템 상태 변경하기 Free -> Equipped
	SetItemState(EItemState::EIS_UnEquipped);
	
}

// 아이템을 다시 떨어트릴 때 -> 콜리전, 시뮬레이션을 모아놓은 함수
void ABaseWeapon::SetCollisionWhenItemDivestiture()
{
	if(Skm && SphereCollision && NoticeSphereCollision)
	{
		this->Owner = nullptr;
		Skm->SetSimulatePhysics(true);
		Skm->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		SphereCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
		SphereCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		NoticeSphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SetItemState(EItemState::EIS_Free);
	}
}

// 변경된 무기 (New Weapon) : 이제 바꾸는 무기를 말함
void ABaseWeapon::SetCollisionWhenItemChangeON()
{
	// 물리, OverlapSphere들 다 켜고
	//SetCollisionWhenItemDivestiture();

	// 매쉬까지 다시 보이게 하기
	Skm->SetVisibility(true);
	
}

void ABaseWeapon::NoticeRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::NoticeRangeBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	// 이게 맞는지 잘 모르겠음... 부모 변수 그대로 끌어다 쓰는건데 이게 맞음?
	if(WeaponInfo && WeaponAttributes)
	{
		WeaponInfo->SetAmmo(WeaponAttributes->GetAmmoCount());
	}
}

void ABaseWeapon::PickUpItem(APlayerCharacter* Player){

	if(Player->GetCharAttribute()->GetEquippedWeapon()) return;
	Super::PickUpItem(Player);
	
	Player->GetCharAttribute()->SetEquippedWeapon(this);
	ItemAcquisition(Player->GetMesh(), Player);
	
}

void ABaseWeapon::PlayFireSound()
{
	if(FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, FirePointVector);
	}
}

void ABaseWeapon::PlayFireMuzzleEffect()
{
	if(FireFlashMuzzle)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FireFlashMuzzle, FirePointVector);
	}
}

void ABaseWeapon::FireEffectPlay(const FHitResult& HitResult)
{
	if(ShootParticle && HitResult.bBlockingHit)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ShootParticle, FireEndPoint);
	}
	if(BeamParticle && HitResult.bBlockingHit)
	{
		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticle, FirePointVector);
		if(Beam)
		{
			Beam->SetVectorParameter(FName("Target"), FireEndPoint);
		}
	}
}

// Play FireSound and FireFlash 
void ABaseWeapon::GunFire(const FHitResult &HitResult, AActor* Player)
{
	// 총구 위치 구하기
	FirePointVector = FirePoint->GetComponentLocation();

	// 총구와 사격 지점 사이에 물체가 있는지 확인하는 LineTrace
	FHitResult WeaponHitResult;
	FireStartPoint = FirePointVector;
	FireEndPoint = HitResult.Location;
	GetWorld()->LineTraceSingleByChannel(WeaponHitResult, FireStartPoint, FireEndPoint, ECC_Visibility);
	// 총구와 사격 지점사이에 물체가 있다면, TargetPoint를 바꿔주고 그렇지 않다면 원래 위치에 사격 효과를 준다.
	if(WeaponHitResult.bBlockingHit)
	{
		FireEndPoint = WeaponHitResult.Location;
	}

	// HitResult? WeaponHitResult? 뭘 써야 하지?
	// HitResult

	if(HitResult.GetActor()->ActorHasTag(FName("Enemy")))
	{
		if(AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(HitResult.GetActor()))
		{
			const FDamageEvent DamageEvent;
			// UGameplayStatics::ApplyDamage(Enemy, GetWeaponDamage(), Player->GetInstigatorController(), Player, nullptr);
			Enemy->GetHit(HitResult.ImpactPoint, Player, WeaponDamage);
			// Enemy->TakeDamage(GetWeaponDamage(), DamageEvent, Player->GetInstigatorController(), Player);
			UE_LOG(LogTemp, Warning, TEXT("데미지 함수 실행"));
		}
	}
	
	// 총 소리 플레이, 총구 화염, 총탄과 비행운 생성
	PlayFireSound();
	PlayFireMuzzleEffect();
	FireEffectPlay(HitResult);

	// 총알 개수 감소
	DecreaseBulletCount();
	
}

void ABaseWeapon::DecreaseBulletCount()
{
	if(WeaponAttributes)
	{
		// 총알 한 개 감소
		WeaponAttributes->DecreaseAmmoCount();
	}
}