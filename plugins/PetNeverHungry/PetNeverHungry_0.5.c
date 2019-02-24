//===== Hercules Plugin ======================================
//= PetNeverHungry
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.5
//===== Compatible With: ===================================== 
//= Hercules 2018-06-18
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
#include "map/pc.h"
#include "map/pet.h"
#include "common/timer.h"
#include "common/nullpo.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"PetNeverHungry",
	SERVER_TYPE_MAP,
	"0.5",
	HPM_VERSION,
};

int pet_hungry_pre( int *tid, int64 *tick, int *id, intptr_t *data ) {
	struct map_session_data *sd = map->id2sd(*id);
	struct pet_data *pd;
	nullpo_retr( true, sd );
	pd = sd->pd;
	pet->set_intimate(pd, 1000);
	pd->pet.hungry = 100;
	pd->pet_hungry_timer = timer->add( *tick + 1000000000, pet->hungry, sd->bl.id, 0 );
	hookStop();
	return 0;
}

int pet_data_init_post( int retVal, struct map_session_data *sd, struct s_pet *petinfo ) {
	struct pet_data *pd = sd->pd;
	pet->set_intimate(pd, 1000);
	pd->pet.hungry = 100;
	return retVal;
}
 
HPExport void plugin_init (void) {
	addHookPre( pet, hungry, pet_hungry_pre );
	addHookPost( pet, data_init, pet_data_init_post );
}