//===== Hercules Plugin ======================================
//= PetNoLootItemID
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2015-12-25
//===== Description: =========================================
//= Prevent the pet from looting certain items
//===== Topic ================================================
//= http://herc.ws/board/topic/11356-disabling-items-in-loot-petsdisable-specific-pets-on-map/
//===== Additional Comments: =================================  
//= use in certain event when the event ask you to pick up as many stuffs as possible
//= and disable pet from looting these item IDs
//============================================================

#include "common/hercules.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "map/pet.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"PetNoLootItemID",
	SERVER_TYPE_MAP,
	"0.1",
	HPM_VERSION,
};

int pet_ai_sub_hard_lootsearch_pre( struct block_list *bl, va_list *ap ) {
	struct flooritem_data *fitem = (struct flooritem_data *)bl;
		// pet will no loot these items
	if ( fitem->item_data.nameid == 501 || fitem->item_data.nameid == 502 || fitem->item_data.nameid == 503 || fitem->item_data.nameid == 504 || fitem->item_data.nameid == 505 )
		hookStop();
	return 0;
}

HPExport void plugin_init (void) {
	addHookPre( "pet->ai_sub_hard_lootsearch", pet_ai_sub_hard_lootsearch_pre );
}
