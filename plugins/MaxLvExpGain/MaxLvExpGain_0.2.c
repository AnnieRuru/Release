//===== Hercules Plugin ======================================
//= Maximum Level Exp Gain
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.2
//===== Compatible With: ===================================== 
//= Hercules 2015-12-19
//===== Description: =========================================
//= stop players from gaining exp after certain level
//===== Topic ================================================
//= http://herc.ws/board/topic/11510-set-base-exp-gain-to-0-at-certain-level
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
	"0.2",
	HPM_VERSION,
};

int max_lv_exp_gain = 99;

void battle_config_setting( const char *key, const char *val ) {
	if ( !strcmpi( key, "max_lv_exp_gain" ) )
		max_lv_exp_gain = atoi(val);
}

int return_battle_config( const char *key ) {
	if ( !strcmpi( key, "max_lv_exp_gain" ) )
		return max_lv_exp_gain;
	return 0;
}

int pc_checkbaselevelup_pre( struct map_session_data *sd ) {
	nullpo_ret(sd);
	if ( (int)sd->status.base_level >= max_lv_exp_gain ) {
		sd->status.base_exp = 0;
		hookStop();
		return false;
	}
	return true;
}

HPExport void server_preinit (void) {
	addBattleConf( "max_lv_exp_gain", battle_config_setting, return_battle_config );
}

HPExport void plugin_init (void) {
	addHookPre( "pc->checkbaselevelup", pc_checkbaselevelup_pre );
}