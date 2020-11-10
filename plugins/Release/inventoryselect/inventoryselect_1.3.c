//===== Hercules Plugin ======================================
//= inventoryselect
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.3
//===== Compatible With: ===================================== 
//= Hercules 2020-11-11
//===== Description: =========================================
//= just like a menu command, but shows your inventory items instead
//===== Topic ================================================
//= http://herc.ws/board/topic/16650-inventoryselect/
//===== Additional Comments: =================================  
//= *inventoryselect{<item flag>{,<array>}};
//= allow to list inventory items
//=   - by list out only that item type
//=   - or by using an array of your choice with ITF_ALL flag
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
	"1.3",
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
	ITF_ALL          = 0xFFFF,
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

	if (sd->state.menu_or_input == 0) {
		int item_type_flag = ITF_ALL;
		if (script_hasdata(st,2)) {
			if (script_isstringtype(st,2)) {
				ShowError("buildin_inventoryselect: string input is not supported!\n");
				st->state = END;
				return false;
			}
			int num = script_getnum(st,2);
			if (num == 0) {
				ShowError("buildin_inventoryselect: Item type flag cannot be 0.\n");
				st->state = END;
				return false;
			}
			item_type_flag = num;
		}

		int c = 0;
		if (script_hasdata(st,3)) {
			if (script_isstringtype(st,3)) {
				ShowError("buildin_inventoryselect: string input is not supported!\n");
				st->state = END;
				return false;
			}
			struct script_data *data = script_getdata(st,3);
			if (!data_isreference(data) || !reference_tovariable(data)) {
				ShowError("buildin_inventoryselect: Target argument is not a variable!\n");
				st->state = END;
				return false;
			}
			const char *varname = reference_getname(data);
			struct reg_db *ref = reference_getref(data);
			uint32 uid = reference_getid(data);
			int array_size = script->array_highest_key(st, sd, varname, ref);
//			for (int i = 0; i < array_size; ++i)
//				ShowDebug("%s[%d/%d] = %d \n", varname, i, array_size, (int)h64BPTRSIZE(script->get_val2(st, reference_uid(uid, i), ref)));

			WFIFOHEAD(sd->fd, sd->status.inventorySize *2 +4);
			WFIFOW(sd->fd, 0) = 0x0177;
			int j;
			for (int i = 0; i < sd->status.inventorySize; ++i) {
				if (sd->status.inventory[i].nameid > 0) {
					struct item_data *it = itemdb->exists(sd->status.inventory[i].nameid);
					j = 0;
					ARR_FIND(0, array_size, j, (int)h64BPTRSIZE(script->get_val2(st, reference_uid(uid, j), ref)) == sd->status.inventory[i].nameid);
					if (((1 << it->type) & item_type_flag) != 0 && j < array_size) {
						WFIFOW(sd->fd, c *2 +4) = i +2;
						++c;
					}
				}
			}
		}
		else {
			WFIFOHEAD(sd->fd, sd->status.inventorySize *2 +4);
			WFIFOW(sd->fd, 0) = 0x0177;
			for (int i = 0; i < sd->status.inventorySize; ++i) {
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

		pc->setreg(sd, reference_uid(script->add_variable("@iselect_id"), 0), sd->status.inventory[i].nameid);
		pc->setreg(sd, reference_uid(script->add_variable("@iselect_amount"), 0), sd->status.inventory[i].amount);
		pc->setreg(sd, reference_uid(script->add_variable("@iselect_refine"), 0), sd->status.inventory[i].refine);
		pc->setreg(sd, reference_uid(script->add_variable("@iselect_identify"), 0), sd->status.inventory[i].identify);
		pc->setreg(sd, reference_uid(script->add_variable("@iselect_attribute"), 0), sd->status.inventory[i].attribute);
		pc->setreg(sd, reference_uid(script->add_variable("@iselect_expire"), 0), sd->status.inventory[i].expire_time);
		pc->setreg(sd, reference_uid(script->add_variable("@iselect_bound"), 0), sd->status.inventory[i].bound);
		pc->setreg(sd, reference_uid(script->add_variable("@iselect_equip"), 0), pc->equippoint(sd, i));
		pc->setreg(sd, reference_uid(script->add_variable("@iselect_favorite"), 0), sd->status.inventory[i].favorite);
		for (j = 0; j < MAX_SLOTS; ++j) {
			safesnprintf(var, SCRIPT_VARNAME_LENGTH, "@iselect_card%d", j +1);
			pc->setreg(sd, reference_uid(script->add_variable(var), 0), sd->status.inventory[i].card[j]);
		}
		for (j = 0; j < MAX_ITEM_OPTIONS; ++j) {
			safesnprintf(var, SCRIPT_VARNAME_LENGTH, "@iselect_opt_id%d", j +1);
			pc->setreg(sd, reference_uid(script->add_variable(var), 0), sd->status.inventory[i].option[j].index);
			safesnprintf(var, SCRIPT_VARNAME_LENGTH, "@iselect_opt_val%d", j +1);
			pc->setreg(sd, reference_uid(script->add_variable(var), 0), sd->status.inventory[i].option[j].value);
			safesnprintf(var, SCRIPT_VARNAME_LENGTH, "@iselect_opt_param%d", j +1);
			pc->setreg(sd, reference_uid(script->add_variable(var), 0), sd->status.inventory[i].option[j].param);
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
	addScriptCommand("inventoryselect", "??", inventoryselect);
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
	script->set_constant("ITF_ALL", ITF_ALL, false, false);
}
