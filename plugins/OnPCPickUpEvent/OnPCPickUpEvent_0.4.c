//===== Hercules Plugin ======================================
//= OnPCPickUpEvent
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.4
//===== Compatible With: ===================================== 
//= Hercules 2019-03-27
//===== Description: =========================================
//= invoke OnPCPickUpEvent whenever a player add/remove an item
//===== Topic ================================================
//= http://herc.ws/board/topic/16660-trigger-onmobdrop-event/?do=findComment&comment=91041
//===== Additional Comments: =================================  
//= remember to use make the bound check, like @pickup_bound or *getitembound to prevent players drop the item again
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/npc.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"OnPCPickUpEvent",
	SERVER_TYPE_MAP,
	"0.4",
	HPM_VERSION,
};

int pc_additem_post( int retVal, struct map_session_data *sd, const struct item *item_data, int amount, e_log_pick_type log_type ) {
	if ( sd == NULL || item_data == NULL || retVal != 0 )
		return retVal;

	pc->setreg( sd, reference_uid( script->add_str("@pickup_itemid"), 0 ), item_data->nameid );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_amount"), 0 ), amount );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_identify"), 0 ), item_data->identify );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_refine"), 0 ), item_data->refine );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_attribute"), 0 ), item_data->attribute );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_expire"), 0 ), item_data->expire_time );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_bound"), 0 ), item_data->bound );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_uniqueid1"), 0 ), item_data->unique_id >> 32 );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_uniqueid2"), 0 ), item_data->unique_id & 0xffffffff );
	pc->setreg( sd, reference_uid( script->add_str("@pickup_action"), 0 ), log_type );
	
	int i;
	char var[SCRIPT_VARNAME_LENGTH];
	for ( i = 0; i < MAX_SLOTS; ++i ) {
		safesnprintf( var, SCRIPT_VARNAME_LENGTH, "@pickup_card%d", i +1 );
		pc->setreg( sd, reference_uid( script->add_str(var), 0 ), item_data->card[i] );
	}
	for ( i = 0; i < MAX_SLOTS; ++i ) {
		safesnprintf( var, SCRIPT_VARNAME_LENGTH, "@pickup_opt_id%d", i +1 );
		pc->setreg( sd, reference_uid( script->add_str(var), 0 ), item_data->option[i].index );
		safesnprintf( var, SCRIPT_VARNAME_LENGTH, "@pickup_opt_val%d", i +1 );
		pc->setreg( sd, reference_uid( script->add_str(var), 0 ), item_data->option[i].value );
		safesnprintf( var, SCRIPT_VARNAME_LENGTH, "@pickup_opt_param%d", i +1 );
		pc->setreg( sd, reference_uid( script->add_str(var), 0 ), item_data->option[i].param );
	}
	npc->event_doall_id( "OnPCPickUpEvent", sd->bl.id );
	return 0;
}

int pc_delitem_pre( struct map_session_data **sd, int *n, int *amount, int *type, short *reason, e_log_pick_type *log_type ) {
	if ( *sd == NULL || *n < 0 || *n >= (*sd)->status.inventorySize )
		return 0;
	if ( (*sd)->status.inventory[*n].nameid == 0 ||
	*amount <= 0 ||
	(*sd)->status.inventory[*n].amount < *amount ||
	(*sd)->inventory_data[*n] == NULL ) {
		hookStop();
		return 1;
	}

	pc->setreg( *sd, reference_uid( script->add_str("@pickup_itemid"), 0 ), (*sd)->status.inventory[*n].nameid );
	pc->setreg( *sd, reference_uid( script->add_str("@pickup_amount"), 0 ), -*amount );
	pc->setreg( *sd, reference_uid( script->add_str("@pickup_identify"), 0 ), (*sd)->status.inventory[*n].identify );
	pc->setreg( *sd, reference_uid( script->add_str("@pickup_refine"), 0 ), (*sd)->status.inventory[*n].refine );
	pc->setreg( *sd, reference_uid( script->add_str("@pickup_attribute"), 0 ), (*sd)->status.inventory[*n].attribute );
	pc->setreg( *sd, reference_uid( script->add_str("@pickup_expire"), 0 ), (*sd)->status.inventory[*n].expire_time );
	pc->setreg( *sd, reference_uid( script->add_str("@pickup_bound"), 0 ), (*sd)->status.inventory[*n].bound );
	pc->setreg( *sd, reference_uid( script->add_str("@pickup_uniqueid1"), 0 ), (*sd)->status.inventory[*n].unique_id >> 32 );
	pc->setreg( *sd, reference_uid( script->add_str("@pickup_uniqueid2"), 0 ), (*sd)->status.inventory[*n].unique_id & 0xffffffff );
	pc->setreg( *sd, reference_uid( script->add_str("@pickup_action"), 0 ), *log_type );
	
	int i;
	char var[SCRIPT_VARNAME_LENGTH];
	for ( i = 0; i < MAX_SLOTS; ++i ) {
		safesnprintf( var, SCRIPT_VARNAME_LENGTH, "@pickup_card%d", i +1 );
		pc->setreg( *sd, reference_uid( script->add_str(var), 0 ), (*sd)->status.inventory[*n].card[i] );
	}
	for ( i = 0; i < MAX_SLOTS; ++i ) {
		safesnprintf( var, SCRIPT_VARNAME_LENGTH, "@pickup_opt_id%d", i +1 );
		pc->setreg( *sd, reference_uid( script->add_str(var), 0 ), (*sd)->status.inventory[*n].option[i].index );
		safesnprintf( var, SCRIPT_VARNAME_LENGTH, "@pickup_opt_val%d", i +1 );
		pc->setreg( *sd, reference_uid( script->add_str(var), 0 ), (*sd)->status.inventory[*n].option[i].value );
		safesnprintf( var, SCRIPT_VARNAME_LENGTH, "@pickup_opt_param%d", i +1 );
		pc->setreg( *sd, reference_uid( script->add_str(var), 0 ), (*sd)->status.inventory[*n].option[i].param );
	}
	npc->event_doall_id( "OnPCPickUpEvent", (*sd)->bl.id );
	return 0;
}

HPExport void plugin_init (void) {
	addHookPost( pc, additem, pc_additem_post );
	addHookPre( pc, delitem, pc_delitem_pre );

	script->set_constant( "LOG_TYPE_TRADE", LOG_TYPE_TRADE, false, false );
	script->set_constant( "LOG_TYPE_VENDING", LOG_TYPE_VENDING, false, false );
	script->set_constant( "LOG_TYPE_PICKDROP_PLAYER", LOG_TYPE_PICKDROP_PLAYER, false, false );
	script->set_constant( "LOG_TYPE_PICKDROP_MONSTER", LOG_TYPE_PICKDROP_MONSTER, false, false );
	script->set_constant( "LOG_TYPE_NPC", LOG_TYPE_NPC, false, false );
	script->set_constant( "LOG_TYPE_SCRIPT", LOG_TYPE_SCRIPT, false, false );
	script->set_constant( "LOG_TYPE_STEAL", LOG_TYPE_STEAL, false, false );
	script->set_constant( "LOG_TYPE_CONSUME", LOG_TYPE_CONSUME, false, false );
	script->set_constant( "LOG_TYPE_PRODUCE", LOG_TYPE_PRODUCE, false, false );
	script->set_constant( "LOG_TYPE_MVP", LOG_TYPE_MVP, false, false );
	script->set_constant( "LOG_TYPE_COMMAND", LOG_TYPE_COMMAND, false, false );
	script->set_constant( "LOG_TYPE_STORAGE", LOG_TYPE_STORAGE, false, false );
	script->set_constant( "LOG_TYPE_GSTORAGE", LOG_TYPE_GSTORAGE, false, false );
	script->set_constant( "LOG_TYPE_MAIL", LOG_TYPE_MAIL, false, false );
	script->set_constant( "LOG_TYPE_AUCTION", LOG_TYPE_AUCTION, false, false );
	script->set_constant( "LOG_TYPE_BUYING_STORE", LOG_TYPE_BUYING_STORE, false, false );
	script->set_constant( "LOG_TYPE_OTHER", LOG_TYPE_OTHER, false, false );
	script->set_constant( "LOG_TYPE_BANK", LOG_TYPE_BANK, false, false );
	script->set_constant( "LOG_TYPE_DIVORCE", LOG_TYPE_DIVORCE, false, false );
	script->set_constant( "LOG_TYPE_ROULETTE", LOG_TYPE_ROULETTE, false, false );
	script->set_constant( "LOG_TYPE_RENTAL", LOG_TYPE_RENTAL, false, false );
	script->set_constant( "LOG_TYPE_CARD", LOG_TYPE_CARD, false, false );
	script->set_constant( "LOG_TYPE_INV_INVALID", LOG_TYPE_INV_INVALID, false, false );
	script->set_constant( "LOG_TYPE_CART_INVALID", LOG_TYPE_CART_INVALID, false, false );
	script->set_constant( "LOG_TYPE_EGG", LOG_TYPE_EGG, false, false );
	script->set_constant( "LOG_TYPE_QUEST", LOG_TYPE_QUEST, false, false );
	script->set_constant( "LOG_TYPE_SKILL", LOG_TYPE_SKILL, false, false );
	script->set_constant( "LOG_TYPE_REFINE", LOG_TYPE_REFINE, false, false );
}
