//===== Hercules Plugin ======================================
//= maxbaselv
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= x_O
//===== Compatible With: ===================================== 
//= Hercules 2019-01-31
//===== Description: =========================================
//= return the value of maximum base/job level of that character's current job
//===== Topic ================================================
//= http://herc.ws/board/topic/16536-help-1-read-maxlevel-from-exp_group_dbconf-and-2-getstorageinventorylist/?do=findComment&comment=90326
//===== Additional Comments: =================================  
//= 
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"maxbaselv",
	SERVER_TYPE_MAP,
	"x_O",
	HPM_VERSION,
};

BUILDIN(maxbaselv) {
	struct map_session_data *sd = script->rid2sd(st);

	if ( sd == NULL )
		return true;

	script_pushint( st, pc->maxbaselv(sd) );
	return true;
}

BUILDIN(maxjoblv) {
	struct map_session_data *sd = script->rid2sd(st);

	if ( sd == NULL )
		return true;

	script_pushint( st, pc->maxjoblv(sd) );
	return true;
}

HPExport void plugin_init (void) {
	addScriptCommand( "maxbaselv", "", maxbaselv );
	addScriptCommand( "maxjoblv", "", maxjoblv );
}