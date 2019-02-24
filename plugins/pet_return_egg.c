//===== Hercules Plugin ======================================
//= pet_return_egg
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2019-02-21
//===== Description: =========================================
//= return pet into egg
//===== Topic ================================================
//= http://herc.ws/board/topic/16567-hpet-egg-renewal-fixing/?do=findComment&comment=90564
//===== Additional Comments: =================================
//= 
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/pet.h"
#include "map/script.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"pet_return_egg",
	SERVER_TYPE_MAP,
	"0.1",
	HPM_VERSION,
};

BUILDIN(pet_return_egg) {
	struct map_session_data *sd;
	if ( script_hasdata(st,2) ) {
		if ( script_isstringtype(st,2) )
			sd = script->nick2sd( st, script_getstr(st,2) );
		else
			sd = script->id2sd( st, script_getnum(st,2) );
	}
	else
		sd = script->rid2sd(st);
	if ( !sd )
		return true;
	if ( !sd->pd ) {
		ShowWarning( "buildin_pet_return_egg: Pet not found !\n" );
		return false;
	}
	pet->return_egg(sd, sd->pd);
	return true;
}

HPExport void plugin_init(void) {
	addScriptCommand( "pet_return_egg", "?", pet_return_egg );
}