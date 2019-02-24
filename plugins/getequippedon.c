//===== Hercules Plugin ======================================
//= getequippedon
//===== By: ==================================================
//= Emistry
//===== Current Version: =====================================
//= x_O
//===== Compatible With: ===================================== 
//= Hercules 2019-01-31
//===== Description: =========================================
//= disallow the word GM at the beginning of the character creation
//===== Topic ================================================
//= http://herc.ws/board/topic/11732-usable-enchant-item/
//===== Additional Comments: =================================  
//= this is not mine, Emistry made this in patch
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"getequippedon",
	SERVER_TYPE_MAP,
	"x_O",
	HPM_VERSION,
};

BUILDIN(getequippedon) {
	int i = status->current_equip_item_index;
	struct map_session_data *sd = script->rid2sd(st);

	if ( sd == NULL )
		return true;

	if ( i < 0 || sd->inventory_data[i] == NULL )
		script_pushint( st, 0 );
	else if ( i == EQI_HAND_L )
		script_pushint( st, sd->inventory_data[i]->type == IT_ARMOR ? EQP_SHIELD : EQP_WEAPON );
	else
		script_pushint( st, sd->status.inventory[i].equip );

	return true;
}

HPExport void plugin_init (void) {
	addScriptCommand( "getequippedon", "", getequippedon );
}