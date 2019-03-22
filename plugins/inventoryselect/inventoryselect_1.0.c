//===== Hercules Plugin ======================================
//= inventoryselect
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.0a
//===== Compatible With: ===================================== 
//= Hercules 2019-03-22
//===== Description: =========================================
//= just like a menu command, but shows your inventory items instead
//===== Topic ================================================
//= http://herc.ws/board/topic/16650-inventoryselect/
//===== Additional Comments: =================================  
//= kinda useless, I was tricked into thinking can manipulate the item menu, but it actually can only list inventory
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/npc.h"
#include "map/clif.h"
#include "map/script.h"
#include "common/memmgr.h"
#include "common/socket.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"inventoryselect",
	SERVER_TYPE_MAP,
	"1.0a",
	HPM_VERSION,
};

struct player_data {
	bool parse_inventory_select;
	int inventory_select;
};

BUILDIN(inventoryselect) {
	struct map_session_data *sd = script->rid2sd(st);
	if ( sd == NULL )
		return true;

	struct player_data *ssd = getFromMSD( sd, 0 );
	if ( ssd == NULL ) {
		CREATE( ssd, struct player_data, 1 );
		ssd->parse_inventory_select = false;
		ssd->inventory_select = -1;
		addToMSD( sd, ssd, 0, true );
	}	

	if ( sd->state.menu_or_input == false ) {
		int i, c;
		WFIFOHEAD( sd->fd, sd->status.inventorySize *2 +4 );
		WFIFOW( sd->fd, 0 ) = 0x0177;
		for ( i = c = 0; i < sd->status.inventorySize; ++i ) {
			if ( sd->status.inventory[i].nameid > 0 ) {
				WFIFOW( sd->fd, c *2 +4 ) = i +2;
				++c;
			}
		}
		if ( c == 0 )
			c = 1; // hacky ... have to always show the item window somehow
		
		WFIFOW( sd->fd, 2 ) = c *2 +4;
		WFIFOSET( sd->fd, WFIFOW( sd->fd, 2 ) );

		st->state = RERUNLINE;
		sd->state.menu_or_input = true;
		ssd->parse_inventory_select = true;
	}
	else if ( ssd->inventory_select < 0 ) {
		sd->state.menu_or_input = false;
		ssd->parse_inventory_select = false;
		st->state = RUN;
		script_pushint( st, -1 );
	}
	else {
		int i = ssd->inventory_select, j;
		char var[SCRIPT_VARNAME_LENGTH];

		script->setd_sub( st, sd, ".@inventoryselect_id", 0, (void *)h64BPTRSIZE( sd->status.inventory[i].nameid ), NULL );
		script->setd_sub( st, sd, ".@inventoryselect_amount", 0, (void *)h64BPTRSIZE( sd->status.inventory[i].amount ), NULL );
		script->setd_sub( st, sd, ".@inventoryselect_refine", 0, (void *)h64BPTRSIZE( sd->status.inventory[i].refine ), NULL );
		script->setd_sub( st, sd, ".@inventoryselect_identify", 0, (void *)h64BPTRSIZE( sd->status.inventory[i].identify ), NULL );
		script->setd_sub( st, sd, ".@inventoryselect_attribute", 0, (void *)h64BPTRSIZE( sd->status.inventory[i].attribute ), NULL );
		script->setd_sub( st, sd, ".@inventoryselect_expire", 0, (void *)h64BPTRSIZE( sd->status.inventory[i].expire_time ), NULL );
		script->setd_sub( st, sd, ".@inventoryselect_bound", 0, (void *)h64BPTRSIZE( sd->status.inventory[i].bound ), NULL );
		for ( j = 0; j < MAX_SLOTS; ++j ) {
			safesnprintf( var, SCRIPT_VARNAME_LENGTH, ".@inventoryselect_card%d", j +1 );
			script->setd_sub( st, sd, var, 0, (void *)h64BPTRSIZE( sd->status.inventory[i].card[j] ), NULL );
		}
		for ( j = 0; j < MAX_ITEM_OPTIONS; ++j ) {
			safesnprintf( var, SCRIPT_VARNAME_LENGTH, ".@inventoryselect_opt_id%d", j +1 );
			script->setd_sub( st, sd, var, 0, (void *)h64BPTRSIZE( sd->status.inventory[i].option[j].index ), NULL );
			safesnprintf( var, SCRIPT_VARNAME_LENGTH, ".@inventoryselect_opt_val%d", j +1 );
			script->setd_sub( st, sd, var, 0, (void *)h64BPTRSIZE( sd->status.inventory[i].option[j].value ), NULL );
			safesnprintf( var, SCRIPT_VARNAME_LENGTH, ".@inventoryselect_opt_param%d", j +1 );
			script->setd_sub( st, sd, var, 0, (void *)h64BPTRSIZE( sd->status.inventory[i].option[j].param ), NULL );
		}
		sd->state.menu_or_input = false;
		ssd->parse_inventory_select = false;
		st->state = RUN;
		script_pushint( st, i );
	}
	return true;
}
			
void clif_pItemIdentify_pre( int *fd, struct map_session_data **sd ) {
	if ( *sd == NULL )
		return;

	struct player_data *ssd = getFromMSD( *sd, 0 );
	if ( ssd->parse_inventory_select == false )
		return;

	short idx = RFIFOW( *fd, 2 ) -2;
	ssd->inventory_select = idx;

	clif_menuskill_clear(*sd);
	npc->scriptcont( *sd, (*sd)->npc_id, false );
	hookStop();
	return;
}
	
HPExport void plugin_init (void) {
	addScriptCommand( "inventoryselect", "", inventoryselect );
	addHookPre( clif, pItemIdentify, clif_pItemIdentify_pre );
}
