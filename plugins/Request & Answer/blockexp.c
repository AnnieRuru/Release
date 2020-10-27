//===== Hercules Plugin ======================================
//= blockexp
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2019-02-19
//===== Description: =========================================
//= players can freely stop leveling at will with a simple atcommand
//===== Topic ================================================
//= http://herc.ws/board/topic/16570-blockexp/
//===== Additional Comments: =================================
//= the reason of using a script variable instead of creating a new SQL table,
//= so server owner can also block exp gain for players from a script
//= so just comment out the addAtcommand line ...
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "common/nullpo.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"blockexp",
	SERVER_TYPE_MAP,
	"0.1",
	HPM_VERSION,
};

bool pc_authok_post( bool retVal, struct map_session_data *sd, int login_id2, time_t expiration_time, int group_id, const struct mmo_charstatus *st, bool changing_mapservers ) {
	if ( retVal == false )
		return false;
	if ( pc_readglobalreg( sd, script->add_variable("blockexp") ) == true )
		clif->message( sd->fd, "@blockexp turn ON." );
	return true;
}

bool pc_gainexp_pre( struct map_session_data **sd, struct block_list **src, uint64 *base_exp, uint64 *job_exp, bool *is_quest ) {
	nullpo_ret(*sd);
	if ( *src == NULL && pc_readglobalreg( *sd, script->add_variable("blockexp") ) == true )
		*base_exp = 0;
	return true;
}

void pc_calcexp_post( struct map_session_data *sd, uint64 *base_exp, uint64 *job_exp, struct block_list *src ) {
	if ( pc_readglobalreg( sd, script->add_variable("blockexp") ) == true )
		*base_exp = 0;
	return;
}

ACMD(blockexp) {
	if ( pc_readglobalreg( sd, script->add_variable("blockexp") ) ) {
		clif->message( sd->fd, "@blockexp turn OFF." );
		pc_setglobalreg( sd, script->add_variable("blockexp"), false );
	}
	else {
		clif->message( sd->fd, "@blockexp turn ON." );
		pc_setglobalreg( sd, script->add_variable("blockexp"), true );
	}
	return true;
}

HPExport void plugin_init(void) {
	addHookPre( pc, gainexp, pc_gainexp_pre );
	addHookPost( pc, calcexp, pc_calcexp_post );
	addAtcommand( "blockexp", blockexp );
}