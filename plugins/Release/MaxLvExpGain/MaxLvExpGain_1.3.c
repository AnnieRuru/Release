//===== Hercules Plugin ======================================
//= Maximum Level Exp Gain
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.3
//===== Compatible With: ===================================== 
//= Hercules 2019-02-19
//===== Description: =========================================
//= stop players from gaining exp after certain level
//===== Topic ================================================
//= http://herc.ws/board/topic/11587-maxlvexpgain/
//===== Additional Comments: =================================
//= this plugin DOES
//= 1. stop players gain exp through killing monsters, mvp exp, or share party experience
//= 2. allows to gain exp by *getexp script command or BaseExp++ / JobExp++
//= 3. allows players to pay guild tax exp after they hit your capped level value
//= 4. if your server has heal_exp, resurrection_exp or shop_exp enable,
//=    they will stop gain exp from these action after they hit your cap level
//= 5. if the player do SA_LEVELUP ( a random skill from Hocos-Pocus ) after the cap level, they can't gain exp, or pay guild tax using this skill
//= ... it seems like SA_LEVELUP skill and guild->payexp hate each other ...
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "common/memmgr.h"
#include "common/nullpo.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"MaxLvExpGain",
	SERVER_TYPE_MAP,
	"1.3",
	HPM_VERSION,
};

int max_blv_exp_gain = 5;   // BaseLevel when player will no longer gain BaseExp
int max_jlv_exp_gain = 5;   // JobLevel when player will no longer gain JobExp

struct player_data {
	bool nobaseexpgain;
	bool nojobexpgain;
};

void battle_config_setting( const char *key, const char *val ) {
	if ( !strcmpi( key, "battle_configuration/max_blv_exp_gain" ) )
		max_blv_exp_gain = atoi(val);
	else if ( !strcmpi( key, "battle_configuration/max_jlv_exp_gain" ) )
		max_jlv_exp_gain = atoi(val);
}

int return_battle_config( const char *key ) {
	if ( !strcmpi( key, "battle_configuration/max_blv_exp_gain" ) )
		return max_blv_exp_gain;
	else if ( !strcmpi( key, "battle_configuration/max_jlv_exp_gain" ) )
		return max_jlv_exp_gain;
	return 0;
}

bool pc_gainexp_pre( struct map_session_data **sd, struct block_list **src, uint64 *base_exp, uint64 *job_exp, bool *is_quest ) {
	struct player_data *ssd;
	nullpo_ret(*sd);

	if ( *is_quest == true )
		return true;

	ssd = getFromMSD(*sd, 0);
	if ( ssd == NULL ) {
		CREATE(ssd, struct player_data, 1);
		ssd->nobaseexpgain = 0;
		ssd->nojobexpgain = 0;
		addToMSD(*sd, ssd, 0, true);
	}
	if ( (*sd)->status.base_level >= max_blv_exp_gain ) {
		if ( *src == NULL )
			*base_exp = 0;
		else
			ssd->nobaseexpgain = 1;
	}
	if ( (*sd)->status.job_level >= max_jlv_exp_gain ) {
		if ( *src == NULL )
			*job_exp = 0;
		else
			ssd->nojobexpgain = 1;
	}
	return true;
}

void pc_calcexp_post( struct map_session_data *sd, uint64 *base_exp, uint64 *job_exp, struct block_list *src ) {
	struct player_data *ssd = getFromMSD(sd, 0);
	nullpo_retv(ssd);
	if ( ssd->nobaseexpgain == 1 ) {
		*base_exp = 0;
		ssd->nobaseexpgain = 0;
	}
	if ( ssd->nojobexpgain == 1 ) {
		*job_exp = 0;
		ssd->nojobexpgain = 0;
	}
	return;
}

HPExport void server_preinit(void) {
	addBattleConf( "battle_configuration/max_blv_exp_gain", battle_config_setting, return_battle_config, false );
	addBattleConf( "battle_configuration/max_jlv_exp_gain", battle_config_setting, return_battle_config, false );
}

HPExport void plugin_init(void) {
	addHookPre( pc, gainexp, pc_gainexp_pre );
	addHookPost( pc, calcexp, pc_calcexp_post );
}