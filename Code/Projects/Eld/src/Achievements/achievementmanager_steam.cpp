#include "core.h"
#include "achievementmanager_steam.h"
#include "simplestring.h"

#include <stdio.h>

#ifdef __vita__
#include <vitasdk.h>
#include <vitaGL.h>

extern "C" {
static char comm_id[12] = {0};
static char signature[160] = {0xb9,0xdd,0xe1,0x3b,0x01,0x00};

static int trp_ctx;
static int plat_id = -1;

typedef struct {
	int sdkVersion;
	SceCommonDialogParam commonParam;
	int context;
	int options;
	uint8_t reserved[128];
} SceNpTrophySetupDialogParam;

typedef struct {
	uint32_t unk[4];
} SceNpTrophyUnlockState;
SceNpTrophyUnlockState trophies_unlocks;

int sceNpTrophyInit(void *unk);
int sceNpTrophyCreateContext(int *context, char *commId, char *commSign, uint64_t options);
int sceNpTrophySetupDialogInit(SceNpTrophySetupDialogParam *param);
SceCommonDialogStatus sceNpTrophySetupDialogGetStatus();
int sceNpTrophySetupDialogTerm();
int sceNpTrophyCreateHandle(int *handle);
int sceNpTrophyDestroyHandle(int handle);
int sceNpTrophyUnlockTrophy(int ctx, int handle, int id, int *plat_id);
int sceNpTrophyGetTrophyUnlockState(int ctx, int handle, SceNpTrophyUnlockState *state, uint32_t *count);

int trophies_available = 0;

volatile int trp_id;
SceUID trp_request_mutex, trp_delivered_mutex;
int trophies_unlocker(SceSize args, void *argp) {
	for (;;) {
		sceKernelWaitSema(trp_request_mutex, 1, NULL);
		int local_trp_id = trp_id;
		int trp_handle;
		sceNpTrophyCreateHandle(&trp_handle);
		sceNpTrophyUnlockTrophy(trp_ctx, trp_handle, local_trp_id, &plat_id);
		sceKernelSignalSema(trp_delivered_mutex, 1);
		sceNpTrophyDestroyHandle(trp_handle);
	}
}

int trophies_init() {
	// Starting sceNpTrophy
	strcpy(comm_id, "ELDR00001");
	sceSysmoduleLoadModule(SCE_SYSMODULE_NP_TROPHY);
	sceNpTrophyInit(NULL);
	int res = sceNpTrophyCreateContext(&trp_ctx, comm_id, signature, 0);
	if (res < 0) {
#ifdef DEBUG
		printf("sceNpTrophyCreateContext returned 0x%08X\n", res);
#endif	
		return res;
	}
	SceNpTrophySetupDialogParam setupParam;
	sceClibMemset(&setupParam, 0, sizeof(SceNpTrophySetupDialogParam));
	_sceCommonDialogSetMagicNumber(&setupParam.commonParam);
	setupParam.sdkVersion = PSP2_SDK_VERSION;
	setupParam.options = 0;
	setupParam.context = trp_ctx;
	sceNpTrophySetupDialogInit(&setupParam);
	static int trophy_setup = SCE_COMMON_DIALOG_STATUS_RUNNING;
	while (trophy_setup == SCE_COMMON_DIALOG_STATUS_RUNNING) {
		trophy_setup = sceNpTrophySetupDialogGetStatus();
		vglSwapBuffers(GL_TRUE);
	}
	sceNpTrophySetupDialogTerm();
	
	// Starting trophy unlocker thread
	trp_delivered_mutex = sceKernelCreateSema("trps delivery", 0, 1, 1, NULL);
	trp_request_mutex = sceKernelCreateSema("trps request", 0, 0, 1, NULL);
	SceUID tropies_unlocker_thd = sceKernelCreateThread("trophies unlocker", &trophies_unlocker, 0x10000100, 0x10000, 0, 0, NULL);
	sceKernelStartThread(tropies_unlocker_thd, 0, NULL);
	
	// Getting current trophy unlocks state
	int trp_handle;
	uint32_t dummy;
	sceNpTrophyCreateHandle(&trp_handle);
	sceNpTrophyGetTrophyUnlockState(trp_ctx, trp_handle, &trophies_unlocks, &dummy);
	sceNpTrophyDestroyHandle(trp_handle);
	
	trophies_available = 1;
	return res;
}

uint8_t trophies_is_unlocked(uint32_t id) {
	if (trophies_available) {
		return (trophies_unlocks.unk[id >> 5] & (1 << (id & 31))) > 0;
	}
	return 0;
}

void trophies_unlock(uint32_t id) {
	if (trophies_available && !trophies_is_unlocked(id)) {
		trophies_unlocks.unk[id >> 5] |= (1 << (id & 31));
		sceKernelWaitSema(trp_delivered_mutex, 1, NULL);
		trp_id = id;
		sceKernelSignalSema(trp_request_mutex, 1);
	}
}
};
#endif

AchievementManager_Steam::AchievementManager_Steam()
:	m_UserStats( NULL )
#if BUILD_STEAM
#endif // BUILD_STEAM
{
#if BUILD_STEAM
#endif // BUILD_STEAM
	RequestServerUpdate();
}

/*virtual*/ AchievementManager_Steam::~AchievementManager_Steam()
{
}

/*virtual*/ void AchievementManager_Steam::RequestServerUpdate()
{
#if BUILD_STEAM
#endif // BUILD_STEAM
}

/* virtual*/ void AchievementManager_Steam::AwardAchievement( const SimpleString& AchievementTag )
{
#ifdef __vita__
	if (!strcmp(AchievementTag.CStr(), "ACH_Soul_Water")) { // Gross, It's All Wet (ID 1)
		trophies_unlock(1);
	} else if (!strcmp(AchievementTag.CStr(), "ACH_Lore")) { // Well Read (ID 2)
		trophies_unlock(2);
	} else if (!strcmp(AchievementTag.CStr(), "ACH_Soul_Sand")) { // Gross, It's All Sandy (ID 4)
		trophies_unlock(4);
	} else if (!strcmp(AchievementTag.CStr(), "ACH_Soul_Swamp")) { // Gross, It's All Slimy (ID 5)
		trophies_unlock(5);
	} else if (!strcmp(AchievementTag.CStr(), "ACH_End")) { // These Timeless Words (ID 6)
		trophies_unlock(6);
	} else if (!strcmp(AchievementTag.CStr(), "ACH_End_True")) { // Free at Last (ID 8)
		trophies_unlock(8);
	} else if (!strcmp(AchievementTag.CStr(), "ACH_End_MoM")) { // What Final Horror (ID 7)
		trophies_unlock(7);
	} else if (!strcmp(AchievementTag.CStr(), "ACH_End_NGP")) { // A Marvel I Cannot Fathom (ID 11)
		trophies_unlock(11);
	} else if (!strcmp(AchievementTag.CStr(), "ACH_End_Speed")) { // Never Enough Time (ID 12)
		trophies_unlock(12);
	} else if (!strcmp(AchievementTag.CStr(), "ACH_End_Unhurt")) { // Not a Scratch (ID 14)
		trophies_unlock(14);
	} else if (!strcmp(AchievementTag.CStr(), "ACH_Bank")) { // All That Junk (ID 10)
		trophies_unlock(10);
	} else if (!strcmp(AchievementTag.CStr(), "ACH_Friends")) { // R'lyeh Nice to Meet You (ID 13)
		trophies_unlock(13);
	} else if (!strcmp(AchievementTag.CStr(), "ACH_Watson")) { // Sweet Revenge (ID 3)
		trophies_unlock(3);
	} else if (!strcmp(AchievementTag.CStr(), "ACH_Souls")) { // Soul Purpose (ID 15)
		trophies_unlock(15);
	} else if (!strcmp(AchievementTag.CStr(), "ACH_Azathoth")) { // As Fast Azathoth (ID 9)
		trophies_unlock(9);
	}
#else
	Unused( AchievementTag );
#endif
#if BUILD_STEAM
#endif // BUILD_STEAM
}

/*virtual*/ void AchievementManager_Steam::IncrementStat( const SimpleString& StatTag, const int Amount )
{
	Unused( StatTag );
	Unused( Amount );

#if BUILD_STEAM
#endif // BUILD_STEAM
}

/*virtual*/ void AchievementManager_Steam::SetStat( const SimpleString& StatTag, const int Value )
{
	Unused( StatTag );
	Unused( Value );

#if BUILD_STEAM
#endif // BUILD_STEAM
}

/*virtual*/ void AchievementManager_Steam::Store()
{
#if BUILD_STEAM
#endif // BUILD_STEAM
}

#if BUILD_DEV
/*virtual*/ void AchievementManager_Steam::ResetAllStats( const bool ResetAchievements )
{
	Unused( ResetAchievements );

#if BUILD_STEAM
#endif
}
#endif

/*virtual*/ void AchievementManager_Steam::ReportGlobalStat( const SimpleString& StatTag )
{
	Unused( StatTag );

#if BUILD_STEAM
#endif // BUILD_STEAM
}

/*virtual*/ void AchievementManager_Steam::ReportGlobalAchievementRate( const SimpleString& AchievementTag )
{
	Unused( AchievementTag );

#if BUILD_STEAM
#endif // BUILD_STEAM
}

void AchievementManager_Steam::RequestUserStats()
{
#if BUILD_STEAM
#endif // BUILD_STEAM
}

void AchievementManager_Steam::RequestGlobalStats()
{
#if BUILD_STEAM
#endif // BUILD_STEAM
}

void AchievementManager_Steam::RequestGlobalAchievementRates()
{
#if BUILD_STEAM
#endif // BUILD_STEAM
}

#if BUILD_STEAM
#endif // BUILD_STEAM

#if BUILD_STEAM
#endif // BUILD_STEAM

#if BUILD_STEAM
#endif // BUILD_STEAM