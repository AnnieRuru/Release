//===== Hercules Plugin ======================================
//= OnPCPickUpEvent
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2015-12-19
//===== Description: =========================================
//= invoke OnPCPickUpEvent whenever a player picks up an item
//===== Topic ================================================
//= http://herc.ws/board/topic/11509-count-total-number-of-items/
//===== Additional Comments: =================================  
//= quite useless, because items can be drop and pick up again
//= unless that item has set nodrop flag in item_db.conf
//============================================================

#include "common/hercules.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
	"0.1",
	HPM_VERSION,
};

enum pickup_type {
	PICKUP_ITEMID = 1,
	PICKUP_AMOUNT,
	PICKUP_IDENTIFY,
	PICKUP_REFINE,
	PICKUP_BROKEN,
	PICKUP_CARD1,
	PICKUP_CARD2,
	PICKUP_CARD3,
	PICKUP_CARD4,
	PICKUP_EXPIRE,
	PICKUP_BOUND,
	PICKUP_UNIQUEID,
	PICKUP_POSX,
	PICKUP_POSY,
};

/* STUPID !!! map->clearflooritem(&fitem->bl); clear the item data !!
int pc_takeitem_post( int retVal, struct map_session_data *sd, struct flooritem_data *fitem ) {
	if ( retVal == 0 ) return 0;
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_ITEMID ), (int)fitem->item_data.nameid );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_AMOUNT ), (int)fitem->item_data.amount );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_IDENTIFY ), (int)fitem->item_data.identify );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_REFINE ), (int)fitem->item_data.refine );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_BROKEN ), (int)fitem->item_data.attribute );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_CARD1 ), (int)fitem->item_data.card[0] );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_CARD2 ), (int)fitem->item_data.card[1] );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_CARD3 ), (int)fitem->item_data.card[2] );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_CARD4 ), (int)fitem->item_data.card[3] );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_EXPIRE ), (int)fitem->item_data.expire_time );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_BOUND ), (int)fitem->item_data.bound );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_UNIQUEID ), (int)fitem->item_data.unique_id );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_POSX ), (int)fitem->bl.x );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_POSY ), (int)fitem->bl.y );
	npc->event_doall_id( "OnPCPickUpEvent", sd->bl.id );
	return 1;
}
*/

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

	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_ITEMID ), (int)fitem->item_data.nameid );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_AMOUNT ), (int)fitem->item_data.amount );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_IDENTIFY ), (int)fitem->item_data.identify );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_REFINE ), (int)fitem->item_data.refine );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_BROKEN ), (int)fitem->item_data.attribute );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_CARD1 ), (int)fitem->item_data.card[0] );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_CARD2 ), (int)fitem->item_data.card[1] );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_CARD3 ), (int)fitem->item_data.card[2] );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_CARD4 ), (int)fitem->item_data.card[3] );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_EXPIRE ), (int)fitem->item_data.expire_time );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_BOUND ), (int)fitem->item_data.bound );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_UNIQUEID ), (int)fitem->item_data.unique_id );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_POSX ), (int)fitem->bl.x );
	pc->setreg( sd, reference_uid( script->add_str("@pickup"), PICKUP_POSY ), (int)fitem->bl.y );
	npc->event_doall_id( "OnPCPickUpEvent", sd->bl.id );

	map->clearflooritem(&fitem->bl);
	return 1;
}

HPExport void plugin_init (void) {
//	addHookPost( "pc->takeitem", pc_takeitem_post );
	pc->takeitem = &pc_takeitem_overload;

	script->set_constant("PICKUP_ITEMID",PICKUP_ITEMID,false);
	script->set_constant("PICKUP_AMOUNT",PICKUP_AMOUNT,false);
	script->set_constant("PICKUP_IDENTIFY",PICKUP_IDENTIFY,false);
	script->set_constant("PICKUP_REFINE",PICKUP_REFINE,false);
	script->set_constant("PICKUP_BROKEN",PICKUP_BROKEN,false);
	script->set_constant("PICKUP_CARD1",PICKUP_CARD1,false);
	script->set_constant("PICKUP_CARD2",PICKUP_CARD2,false);
	script->set_constant("PICKUP_CARD3",PICKUP_CARD3,false);
	script->set_constant("PICKUP_CARD4",PICKUP_CARD4,false);
	script->set_constant("PICKUP_EXPIRE",PICKUP_EXPIRE,false);
	script->set_constant("PICKUP_BOUND",PICKUP_BOUND,false);
	script->set_constant("PICKUP_UNIQUEID",PICKUP_UNIQUEID,false);
	script->set_constant("PICKUP_POSX",PICKUP_POSX,false);
	script->set_constant("PICKUP_POSY",PICKUP_POSY,false);
}