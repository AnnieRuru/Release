//===== Hercules Plugin ======================================
//= inventoryselect
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.2
//===== Compatible With: ===================================== 
//= Hercules 2020-10-11
//===== Description: =========================================
//= just like a menu command, but shows your inventory items instead
//===== Topic ================================================
//= http://herc.ws/board/topic/16650-inventoryselect/
//===== Additional Comments: =================================  
//= now allow to list inventory items, by list out only that item type
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
	"1.2",
	HPM_VERSION,
};

// maybe some members wants to show IT_WEAPON and IT_ARMOR at the same time ??
enum item_type_flag { 
	ITF_HEALING      = 0x1,
	ITF_USABLE       = 0x4,
	ITF_ETC          = 0x8,
	ITF_WEAPON       = 0x10,
	ITF_ARMOR        = 0x20,
	ITF_CARD         = 0x40,
	ITF_PETEGG       = 0x80,
	ITF_PETARMOR     = 0x100,
	ITF_AMMO         = 0x400,
	ITF_DELAYCONSUME = 0x800,
	ITF_CASH         = 0x4000,
};

struct player_data {
	unsigned parse_inventory_select : 1;
	int inventory_select;
};

BUILDIN(inventoryselect) {
	struct map_session_data *sd = script->rid2sd(st);
	if (sd == NULL)
		return true;

	struct player_data *ssd = getFromMSD(sd, 0);
	if (ssd == NULL) {
		CREATE(ssd, struct player_data, 1);
		ssd->parse_inventory_select = 0;
		ssd->inventory_select = -1;
		addToMSD(sd, ssd, 0, true);
	}	

	int item_type_flag = -1;
	if (script_hasdata(st,2)) {
		if (script_isstringtype(st,2)) {
			ShowError("buildin_inventoryselect: string input is not supported!\n");
			st->state = END;
			return false;
		}
		int num = script_getnum(st,2);
		if (num == 0) {
			ShowError("buildin_inventoryselect: Item type flag cannot be 0.\n", num);
			st->state = END;
			return false;
		}
		item_type_flag = num;
	}

	if (sd->state.menu_or_input == 0) {
		int i, c;
		WFIFOHEAD(sd->fd, sd->status.inventorySize *2 +4);
		WFIFOW(sd->fd, 0) = 0x0177;
		if (item_type_flag == -1) {
			for (i = c = 0; i < sd->status.inventorySize; ++i) {
				if (sd->status.inventory[i].nameid > 0) {
					WFIFOW(sd->fd, c *2 +4) = i +2;
					++c;
				}
			}
		}
		else {
			for (i = c = 0; i < sd->status.inventorySize; ++i) {
				if (sd->status.inventory[i].nameid > 0) {
					struct item_data *it = itemdb->exists(sd->status.inventory[i].nameid);
					if (((1 << it->type) & item_type_flag) != 0) {
						WFIFOW(sd->fd, c *2 +4) = i +2;
						++c;
					}
				}
			}
		}
		if (c == 0)
			c = 1; // hacky ... have to always show the item window somehow
		
		WFIFOW(sd->fd, 2) = c *2 +4;
		WFIFOSET(sd->fd, WFIFOW(sd->fd, 2));

		st->state = RERUNLINE;
		sd->state.menu_or_input = 1;
		ssd->parse_inventory_select = 1;
	}
	else if (ssd->inventory_select < 0) {
		sd->state.menu_or_input = 0;
		ssd->parse_inventory_select = 0;
		st->state = RUN;
		script_pushint(st, -1);
	}
	else {
		int i = ssd->inventory_select, j;
		char var[SCRIPT_VARNAME_LENGTH];

		script->setd_sub(st, sd, ".@inventoryselect_id", 0, (void *)h64BPTRSIZE(sd->status.inventory[i].nameid), NULL);
		script->setd_sub(st, sd, ".@inventoryselect_amount", 0, (void *)h64BPTRSIZE(sd->status.inventory[i].amount), NULL);
		script->setd_sub(st, sd, ".@inventoryselect_refine", 0, (void *)h64BPTRSIZE(sd->status.inventory[i].refine), NULL);
		script->setd_sub(st, sd, ".@inventoryselect_identify", 0, (void *)h64BPTRSIZE(sd->status.inventory[i].identify), NULL);
		script->setd_sub(st, sd, ".@inventoryselect_attribute", 0, (void *)h64BPTRSIZE(sd->status.inventory[i].attribute), NULL);
		script->setd_sub(st, sd, ".@inventoryselect_expire", 0, (void *)h64BPTRSIZE(sd->status.inventory[i].expire_time), NULL);
		script->setd_sub(st, sd, ".@inventoryselect_bound", 0, (void *)h64BPTRSIZE(sd->status.inventory[i].bound), NULL);
		for (j = 0; j < MAX_SLOTS; ++j) {
			safesnprintf(var, SCRIPT_VARNAME_LENGTH, ".@inventoryselect_card%d", j +1);
			script->setd_sub( st, sd, var, 0, (void *)h64BPTRSIZE( sd->status.inventory[i].card[j]), NULL);
		}
		for (j = 0; j < MAX_ITEM_OPTIONS; ++j) {
			safesnprintf(var, SCRIPT_VARNAME_LENGTH, ".@inventoryselect_opt_id%d", j +1);
			script->setd_sub(st, sd, var, 0, (void *)h64BPTRSIZE(sd->status.inventory[i].option[j].index), NULL);
			safesnprintf(var, SCRIPT_VARNAME_LENGTH, ".@inventoryselect_opt_val%d", j +1);
			script->setd_sub(st, sd, var, 0, (void *)h64BPTRSIZE(sd->status.inventory[i].option[j].value), NULL);
			safesnprintf(var, SCRIPT_VARNAME_LENGTH, ".@inventoryselect_opt_param%d", j +1);
			script->setd_sub(st, sd, var, 0, (void *)h64BPTRSIZE(sd->status.inventory[i].option[j].param), NULL);
		}
		sd->state.menu_or_input = 0;
		ssd->parse_inventory_select = 0;
		st->state = RUN;
		script_pushint(st, i);
	}
	return true;
}
			
void clif_pItemIdentify_pre(int *fd, struct map_session_data **sd) {
	if (*sd == NULL)
		return;

	struct player_data *ssd = getFromMSD(*sd, 0);
	if (ssd == NULL)
		return;
	if (ssd->parse_inventory_select == 0)
		return;

	short idx = RFIFOW(*fd, 2) -2;
	ssd->inventory_select = idx;

	clif_menuskill_clear(*sd);
	npc->scriptcont(*sd, (*sd)->npc_id, 0);
	hookStop();
	return;
}
	
HPExport void plugin_init (void) {
	addScriptCommand("inventoryselect", "?", inventoryselect);
	addHookPre(clif, pItemIdentify, clif_pItemIdentify_pre);

	script->set_constant("ITF_HEALING", ITF_HEALING, false, false);
	script->set_constant("ITF_USABLE", ITF_USABLE, false, false);
	script->set_constant("ITF_ETC", ITF_ETC, false, false);
	script->set_constant("ITF_WEAPON", ITF_WEAPON, false, false);
	script->set_constant("ITF_ARMOR", ITF_ARMOR, false, false);
	script->set_constant("ITF_CARD", ITF_CARD, false, false);
	script->set_constant("ITF_PETEGG", ITF_PETEGG, false, false);
	script->set_constant("ITF_PETARMOR", ITF_PETARMOR, false, false);
	script->set_constant("ITF_AMMO", ITF_AMMO, false, false);
	script->set_constant("ITF_DELAYCONSUME", ITF_DELAYCONSUME, false, false);
	script->set_constant("ITF_CASH", ITF_CASH, false, false);
}
