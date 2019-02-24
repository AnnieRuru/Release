//===== Hercules Plugin ======================================
//= Maximum Level Exp Gain
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.1
//===== Compatible With: ===================================== 
//= Hercules 2016-1-1
//===== Description: =========================================
//= stop players from gaining exp after certain level
//===== Topic ================================================
//= http://herc.ws/board/topic/11587-maxlvexpgain/
//===== Additional Comments: =================================  
//= the 1st one I did has a battle config
//============================================================

#include "common/hercules.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "map/pc.h"
#include "common/memmgr.h"
#include "common/nullpo.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"MaxLvExpGain",
	SERVER_TYPE_MAP,
	"1.1",
	HPM_VERSION,
};

int max_blv_exp_gain = 1; // when player reach this BaseLevel, they stop gain BaseExp
int max_jlv_exp_gain = 1; // when player reach this JobLevel, they stop gain JobExp

struct player_data {
	bool nobaseexpgain;
	bool nojobexpgain;
};

void battle_config_setting( const char *key, const char *val ) {
	if ( !strcmpi( key, "max_blv_exp_gain" ) )
		max_blv_exp_gain = atoi(val);
	else if ( !strcmpi( key, "max_jlv_exp_gain" ) )
		max_jlv_exp_gain = atoi(val);
}

int return_battle_config( const char *key ) {
	if ( !strcmpi( key, "max_blv_exp_gain" ) )
		return max_blv_exp_gain;
	else if ( !strcmpi( key, "max_jlv_exp_gain" ) )
		return max_jlv_exp_gain;
	return 0;
}

bool pc_gainexp_pre( struct map_session_data *sd, struct block_list *src, unsigned int *base_exp, unsigned int *job_exp, bool *is_quest ) {
	nullpo_ret(sd);
	if ( *is_quest == true ) // exp gain through quest are exempted
		return true;
	else {
		struct player_data *ssd = getFromMSD( sd, 0 );
		if ( !ssd ) {
			CREATE( ssd, struct player_data, 1 );
			ssd->nobaseexpgain = 0;
			ssd->nojobexpgain = 0;
			addToMSD( sd, ssd, 0, true );
		}
		if ( (int)sd->status.base_level >= max_blv_exp_gain )
			ssd->nobaseexpgain = 1;
		if ( (int)sd->status.job_level >= max_jlv_exp_gain )
			ssd->nojobexpgain = 1;
	}
	return true;
}

void pc_calcexp_post( struct map_session_data *sd, unsigned int *base_exp, unsigned int *job_exp, struct block_list *src ) {
	struct player_data *ssd = getFromMSD( sd, 0 );
	nullpo_retv(ssd);
	if ( ssd->nobaseexpgain ) {
		*base_exp = 0;
		ssd->nobaseexpgain = 0;
	}
	if ( ssd->nojobexpgain ) {
		*job_exp = 0;
		ssd->nojobexpgain = 0;
	}
	return;
}

HPExport void server_preinit (void) {
	addBattleConf( "max_blv_exp_gain", battle_config_setting, return_battle_config );
	addBattleConf( "max_jlv_exp_gain", battle_config_setting, return_battle_config );
}

HPExport void plugin_init (void) {
	addHookPre( "pc->gainexp", pc_gainexp_pre );
	addHookPost( "pc->calcexp", pc_calcexp_post );
}