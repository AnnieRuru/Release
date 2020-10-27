//===== Hercules Plugin ======================================
//= PetNeverHungry
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.3
//===== Compatible With: ===================================== 
//= Hercules 2015-12-25
//===== Description: =========================================
//= Prevent the pet from getting hungry
//===== Topic ================================================
//= <none>
//===== Additional Comments: =================================  
//= feel like having a poring pet in my test server
//= but I fed up having to feed it so frequently (I knew about the battle_config.pet_hungry_delay_rate)
//= so I attach this plugin to my test server so I don't have to feed it even once
//============================================================

#include "common/hercules.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "map/pc.h"
#include "map/pet.h"
#include "common/timer.h"
#include "common/nullpo.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"PetNeverHungry",
	SERVER_TYPE_MAP,
	"0.3",
	HPM_VERSION,
};

int pet_hungry_pre( int *tid, int64 *tick, int *id, intptr_t *data ) {
	TBL_PC *sd;
	struct pet_data *pd;
	sd = map->id2sd(*id);
	nullpo_retr( true, sd );
	pd = sd->pd;
	pd->pet.intimate = 1000;
	pd->pet.hungry = 100;
	pd->pet_hungry_timer = timer->add( *tick + 100000000, pet->hungry, sd->bl.id, 0 );
	hookStop();
	return 0;
}

HPExport void plugin_init (void) {
	addHookPre( "pet->hungry", pet_hungry_pre );
}