//===== Hercules Plugin ======================================
//= getinventorylistidx
//===== By: ==================================================
//= Idea by Myriad aka. EyesOfAHawk aka. [Wolfie] .... aka ... how many name does he has ?
//= 4144 for the inventory index idea -> https://github.com/HerculesWS/Hercules/pull/2355#issuecomment-456516090
//= Fixed by AnnieRuru
//===== Current Version: =====================================
//= 1.0
//===== Compatible With: ===================================== 
//= Hercules 2019-03-01
//===== Description: =========================================
//= introduce getinventorylistidx, specifically meant for delitemidx and equipidx
//===== Topic ================================================
//= http://herc.ws/board/topic/16604-retrieve-inventory-index-getinventorylistidx-delitemidx-equipidx/
//===== Additional Comments: =================================  
//= 2 reasons why this is made plugin and not merge in Github
//=   1. members might confuse which one to use, getinventorylist() or getinventorylistidx() ??
//=   2. as Myriad stated, our inventory index can be empty, -> https://github.com/HerculesWS/Hercules/pull/2394#issuecomment-468517166
//=      perhaps maybe after `sd->inventory` has been VECTORize, then this might have more chance getting accepted
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/script.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"getinventorylistidx",
	SERVER_TYPE_MAP,
	"1.0",
	HPM_VERSION,
};

BUILDIN(getinventorylistidx) {
	struct map_session_data *sd = script->rid2sd(st);

	if (sd == NULL)
		return true;

	char card_var[SCRIPT_VARNAME_LENGTH];
	int i, j;
	// Clear previous data
	script->cleararray_pc(sd, "@inventorylistidx_id", (void*)0);
	script->cleararray_pc(sd, "@inventorylistidx_amount", (void*)0);
	script->cleararray_pc(sd, "@inventorylistidx_equip", (void*)0);
	script->cleararray_pc(sd, "@inventorylistidx_refine", (void*)0);
	script->cleararray_pc(sd, "@inventorylistidx_identify", (void*)0);
	script->cleararray_pc(sd, "@inventorylistidx_attribute", (void*)0);
	for (j = 0; j < MAX_SLOTS; j++) {
		sprintf(card_var, "@inventorylistidx_card%d", j + 1);
		script->cleararray_pc(sd, card_var, (void*)0);
	}
	for (j = 0; j < MAX_ITEM_OPTIONS; j++) {
		sprintf(card_var, "@inventorylistidx_opt_id%d", j + 1);
		script->cleararray_pc(sd, card_var, (void*)0);
		sprintf(card_var, "@inventorylistidx_opt_val%d", j + 1);
		script->cleararray_pc(sd, card_var, (void*)0);
		sprintf(card_var, "@inventorylistidx_opt_param%d", j + 1);
		script->cleararray_pc(sd, card_var, (void*)0);
	}
	script->cleararray_pc(sd, "@inventorylistidx_expire", (void*)0);
	script->cleararray_pc(sd, "@inventorylistidx_bound", (void*)0);
	script->cleararray_pc(sd, "@inventorylistidx_canequip", (void*)0);
	
	for (i = 0; i < sd->status.inventorySize; i++) {
		if (sd->status.inventory[i].nameid > 0 && sd->status.inventory[i].amount > 0) {
			pc->setreg(sd, reference_uid(script->add_variable("@inventorylistidx_id"), i), sd->status.inventory[i].nameid);
			pc->setreg(sd, reference_uid(script->add_variable("@inventorylistidx_amount"), i), sd->status.inventory[i].amount);
			if(sd->status.inventory[i].equip)
				pc->setreg(sd, reference_uid(script->add_variable("@inventorylistidx_equip"), i), pc->equippoint(sd,i));
			else
				pc->setreg(sd, reference_uid(script->add_variable("@inventorylistidx_equip"), i), 0);
			pc->setreg(sd, reference_uid(script->add_variable("@inventorylistidx_refine"), i), sd->status.inventory[i].refine);
			pc->setreg(sd, reference_uid(script->add_variable("@inventorylistidx_identify"), i), sd->status.inventory[i].identify);
			pc->setreg(sd, reference_uid(script->add_variable("@inventorylistidx_attribute"), i), sd->status.inventory[i].attribute);
			for (j = 0; j < MAX_SLOTS; j++) {
				sprintf(card_var, "@inventorylistidx_card%d", j+1);
				pc->setreg(sd, reference_uid(script->add_variable(card_var), i), sd->status.inventory[i].card[j]);
			}
			for (j = 0; j < MAX_ITEM_OPTIONS; j++) {
				sprintf(card_var, "@inventorylistidx_opt_id%d", j + 1);
				pc->setreg(sd, reference_uid(script->add_variable(card_var), i), sd->status.inventory[i].option[j].index);
				sprintf(card_var, "@inventorylistidx_opt_val%d", j + 1);
				pc->setreg(sd, reference_uid(script->add_variable(card_var), i), sd->status.inventory[i].option[j].value);
				sprintf(card_var, "@inventorylistidx_opt_param%d", j + 1);
				pc->setreg(sd, reference_uid(script->add_variable(card_var), i), sd->status.inventory[i].option[j].param);
			}
			pc->setreg(sd, reference_uid(script->add_variable("@inventorylistidx_expire"), i), sd->status.inventory[i].expire_time);
			pc->setreg(sd, reference_uid(script->add_variable("@inventorylistidx_bound"), i), sd->status.inventory[i].bound);
			pc->setreg(sd, reference_uid(script->add_variable("@inventorylistidx_canequip"), i), pc->isequip(sd, i));
		}
	}
	return true;
}

BUILDIN(delitemidx) {
	struct map_session_data *sd;

	if (script_hasdata(st, 4)) {
		if ((sd = script->id2sd(st, script_getnum(st, 4))) == NULL) {
			st->state = END;
			return true;
		}
	} else {
		if ((sd = script->rid2sd(st)) == NULL)
			return true;
	}

	int i = script_getnum(st, 2);
	if (i < 0 || i >= sd->status.inventorySize) {
		ShowError("buildin_delitemidx: Index (%d) should be from 0-%d.\n", i, sd->status.inventorySize - 1);
		st->state = END;
		return false;
	}

	int amount = 0;
	if (script_hasdata(st, 3)) {
		if ((amount = script_getnum(st, 3)) > sd->status.inventory[i].amount)
			amount = sd->status.inventory[i].amount;
	} else {
		amount = sd->status.inventory[i].amount;
	}

	if (amount > 0)
		script->buildin_delitem_delete(sd, i, &amount, true);

	return true;
}

BUILDIN(equipidx) {
	struct map_session_data *sd = script->rid2sd(st);

	if (sd == NULL)
		return true;

	int i = script_getnum(st, 2);
	if (i < 0 || i >= sd->status.inventorySize) {
		ShowError("buildin_equipidx: Index (%d) should be from 0-%d.\n", i, sd->status.inventorySize - 1);
		return false;
	}

	if (sd->status.inventory[i].equip != 0) // item already equipped, run silently
		return true;

	int nameid = sd->status.inventory[i].nameid;
	struct item_data *idata = itemdb->exists(nameid);
	if (idata == NULL) {
		ShowError("buildin_equipidx: Invalid Item ID (%d).\n", nameid);
		return false;
	}

	if (pc->equipitem(sd, i, idata->equip) == 0) {
		ShowWarning("buildin_equipidx: Item ID (%d) at index (%d) cannot be equipped.\n", nameid, i);
		return false;
	}

	return true;
}

HPExport void plugin_init (void) {
	addScriptCommand( "getinventorylistidx", "", getinventorylistidx );
	addScriptCommand( "delitemidx", "i??", delitemidx );
	addScriptCommand( "equipidx", "i", equipidx );
}
