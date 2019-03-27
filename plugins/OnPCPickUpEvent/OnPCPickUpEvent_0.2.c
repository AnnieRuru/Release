//===== Hercules Plugin ======================================
//= OnPCPickUpEvent
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.2a
//===== Compatible With: ===================================== 
//= Hercules 2019-03-27
//===== Description: =========================================
//= invoke OnPCPickUpEvent whenever a player picks up an item
//===== Topic ================================================
//= http://herc.ws/board/topic/11509-count-total-number-of-items/
//===== Additional Comments: =================================  
//= remember to use make the bound check, like @pickup_bound or *getitembound to prevent players drop the item again
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/npc.h"
#include "map/party.h"
#include "map/itemdb.h"
#include "map/script.h"
#include "common/timer.h"
#include "common/nullpo.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"OnPCPickUpEvent",
	SERVER_TYPE_MAP,
	"0.2a",
	HPM_VERSION,
};

int pc_takeitem_overload( struct map_session_data *sd, struct flooritem_data *fitem ) {
	int flag=0;
	int64 tick = timer->gettick();
	struct party_data *p=NULL;

	nullpo_ret(sd);
	nullpo_ret(fitem);

	if(!check_distance_bl(&fitem->bl, &sd->bl, 2) && sd->ud.skill_id!=BS_GREED)
		return 0; // Distance is too far

	if( pc_has_permission(sd,PC_PERM_DISABLE_PICK_UP) )
		return 0;

	if (sd->status.party_id)
		p = party->search(sd->status.party_id);

	if (fitem->first_get_charid > 0 && fitem->first_get_charid != sd->status.char_id) {
		struct map_session_data *first_sd = map->charid2sd(fitem->first_get_charid);
		if (DIFF_TICK(tick,fitem->first_get_tick) < 0) {
			if (!(p && p->party.item&1 &&
				first_sd && first_sd->status.party_id == sd->status.party_id
			))
				return 0;
		} else if (fitem->second_get_charid > 0 && fitem->second_get_charid != sd->status.char_id) {
			struct map_session_data *second_sd = map->charid2sd(fitem->second_get_charid);
			if (DIFF_TICK(tick, fitem->second_get_tick) < 0) {
				if (!(p && p->party.item&1 &&
					((first_sd && first_sd->status.party_id == sd->status.party_id) ||
					(second_sd && second_sd->status.party_id == sd->status.party_id))
				))
					return 0;
			} else if (fitem->third_get_charid > 0 && fitem->third_get_charid != sd->status.char_id) {
				struct map_session_data *third_sd = map->charid2sd(fitem->third_get_charid);
				if (DIFF_TICK(tick,fitem->third_get_tick) < 0) {
					if (!(p && p->party.item&1 &&
						((first_sd && first_sd->status.party_id == sd->status.party_id) ||
						(second_sd && second_sd->status.party_id == sd->status.party_id) ||
						(third_sd && third_sd->status.party_id == sd->status.party_id))
					))
						return 0;
				}
			}
		}
	}

	//This function takes care of giving the item to whoever should have it, considering party-share options.
	if ((flag = party->share_loot(p,sd,&fitem->item_data, fitem->first_get_charid))) {
		clif->additem(sd,0,0,flag);
		return 1;
	}

	//Display pickup animation.
	pc_stop_attack(sd);
	clif->takeitem(&sd->bl,&fitem->bl);

	pc->setreg( sd, reference_uid( script->add_str("@pickup_itemid"), 0 ), (int)fitem->item_data.nameid );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_amount"), 0 ), (int)fitem->item_data.amount );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_identify"), 0 ), (int)fitem->item_data.identify );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_refine"), 0 ), (int)fitem->item_data.refine );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_attribute"), 0 ), (int)fitem->item_data.attribute );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_expire"), 0 ), (int)fitem->item_data.expire_time );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_bound"), 0 ), (int)fitem->item_data.bound );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_uniqueid1"), 0 ), (int)(fitem->item_data.unique_id >> 32) );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_uniqueid2"), 0 ), (int)(fitem->item_data.unique_id & 0xffffffff) );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_posx"), 0 ), (int)fitem->bl.x );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_posy"), 0 ), (int)fitem->bl.y );
	int i;
	char var[SCRIPT_VARNAME_LENGTH];
	for ( i = 0; i < MAX_SLOTS; ++i ) {
		safesnprintf( var, SCRIPT_VARNAME_LENGTH, "@pickup_card%d", i +1 );
		pc->setreg( sd, reference_uid( script->add_str(var), 0 ), (int)fitem->item_data.card[i] );
	}
	for ( i = 0; i < MAX_ITEM_OPTIONS; ++i ) {
		safesnprintf( var, SCRIPT_VARNAME_LENGTH, "@pickup_opt_id%d", i +1 );
		pc->setreg( sd, reference_uid( script->add_str(var), 0 ), (int)fitem->item_data.option[i].index );
		safesnprintf( var, SCRIPT_VARNAME_LENGTH, "@pickup_opt_val%d", i +1 );
		pc->setreg( sd, reference_uid( script->add_str(var), 0 ), (int)fitem->item_data.option[i].value );
		safesnprintf( var, SCRIPT_VARNAME_LENGTH, "@pickup_opt_param%d", i +1 );
		pc->setreg( sd, reference_uid( script->add_str(var), 0 ), (int)fitem->item_data.option[i].param );
	}
	npc->event_doall_id( "OnPCPickUpEvent", sd->bl.id );

	map->clearflooritem(&fitem->bl);
	return 1;
}

HPExport void plugin_init (void) {
	pc->takeitem = &pc_takeitem_overload;
}
