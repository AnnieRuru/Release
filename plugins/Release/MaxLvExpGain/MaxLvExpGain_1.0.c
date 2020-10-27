//===== Hercules Plugin ======================================
//= Maximum Level Exp Gain
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.0
//===== Compatible With: ===================================== 
//= Hercules 2015-12-19
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
#include "common/nullpo.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"MaxLvExpGain",
	SERVER_TYPE_MAP,
	"1.0",
	HPM_VERSION,
};

int max_blv_exp_gain = 90; // when player reach this BaseLevel, they stop gain BaseExp
int max_jlv_exp_gain = 45; // when player reach this JobLevel, they stop gain JobExp

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
	if ( (int)sd->status.base_level >= max_blv_exp_gain )
		*base_exp = 0;
	if ( (int)sd->status.job_level >= max_jlv_exp_gain )
		*job_exp = 0;
	return true;
}

HPExport void server_preinit (void) {
	addBattleConf( "max_blv_exp_gain", battle_config_setting, return_battle_config );
	addBattleConf( "max_jlv_exp_gain", battle_config_setting, return_battle_config );
}

HPExport void plugin_init (void) {
	addHookPre( "pc->gainexp", pc_gainexp_pre );
}