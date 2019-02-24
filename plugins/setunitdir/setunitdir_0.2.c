//===== Hercules Plugin ======================================
//= setunitdir
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.2
//===== Compatible With: ===================================== 
//= Hercules 2015-12-28
//===== Description: =========================================
//= modify and retrieve the value of unit's direction
//===== Topic ================================================
//= http://herc.ws/board/topic/11580-about-skid-trap-effect/
//===== Additional Comments: =================================  
//= it seems all others use unit->ud.dir, but npc uses nd->dir
//= it needs some clean up before this go into our repo
//============================================================

#include "common/hercules.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "map/pc.h"
#include "map/npc.h"
#include "map/unit.h"
#include "map/script.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"setunitdir",
	SERVER_TYPE_MAP,
	"0,2",
	HPM_VERSION,
};

BUILDIN(setunitdir) {
	int unit_id = script_getnum(st,2);
	struct block_list *bl = map->id2bl( unit_id );
	uint8 udir = script_getnum(st,3);
	if ( !bl ) {
		ShowWarning( "buildin_setunitdir: no such GID existed '%d'\n", unit_id );
		return false;
	}
	if ( bl->type == BL_PC ) {
		TBL_PC *sd = map->id2sd( unit_id );
		sd->ud.dir = udir % 8;
		if ( script_hasdata(st,4) )
			sd->head_dir = script_getnum(st,4) % 3;
	}
	else if ( bl->type == BL_NPC )
		((TBL_NPC*)bl)->dir = udir % 8;
	else
		unit->bl2ud(bl)->dir = udir % 8;
	clif->changed_dir(bl, AREA);
	return true;
}

BUILDIN(getunitdir) {
	int unit_id = script_getnum(st,2);
	struct block_list *bl = map->id2bl( unit_id );
	uint8 udir = 0;
	if ( !bl ) {
		ShowWarning( "buildin_getunitdir: no such GID existed '%d'\n", unit_id );
		script_pushint(st, -1);
		return false;
	}
	if ( script_hasdata(st,3) ) {
		if ( bl->type != BL_PC ) {
			ShowWarning( "buildin_getunitdir: attempt to get head direction for non PC object '%d'\n", unit_id );
			script_pushint(st, -1);
			return false;
		}
		else {
			TBL_PC *sd = map->id2sd( unit_id );
			udir = sd->head_dir;
		}
	}
	else if ( bl->type == BL_NPC )
		udir = ((TBL_NPC*)bl)->dir;
	else
		udir = unit->bl2ud(bl)->dir;
	script_pushint(st,udir);
	return true;
}

HPExport void plugin_init (void) {
	script->set_constant("DIR_HEAD_FRONT", 0, false);
	script->set_constant("DIR_HEAD_LEFT", 1, false);
	script->set_constant("DIR_HEAD_RIGHT", 2, false);
	
	addScriptCommand( "getunitdir", "i?", getunitdir );
	addScriptCommand( "setunitdir", "ii?", setunitdir );
}