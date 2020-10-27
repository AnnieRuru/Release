//===== Hercules Plugin ======================================
//= getstorageitem
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.0
//===== Compatible With: ===================================== 
//= Hercules 2020-10-22
//===== Description: =========================================
//= storage related script commands
//===== Topic ================================================
//= 
//===== Additional Comments: =================================
//= 
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/script.h"
#include "map/itemdb.h"
#include "map/storage.h"
#include "common/nullpo.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"getstorageitem",
	SERVER_TYPE_MAP,
	"1.0",
	HPM_VERSION,
};

BUILDIN(getstorageitem) {
	struct map_session_data *sd = NULL;
	if (script_hasdata(st,4))
		sd = map->id2sd(script_getnum(st,4));
	else
		sd = script->rid2sd(st);
	if (sd == NULL)
		return true;

	int amount = script_getnum(st,3);
	if (amount <= 0)
		return true;

	int nameid = 0;
	struct item_data *idata;
	if (script_isstring(st,2)) {
		const char *name = script_getstr(st,2);
		idata = itemdb->search_name(name);
		if (idata == NULL) {
			ShowError("buildin_getstorageitem: Non-existant item %s requested.\n", name);
			return false;
		}
		nameid = idata->nameid;
	}
	else {
		nameid = script_getnum(st,2);
		if (itemdb->exists(nameid) == NULL) {
			ShowError("buildin_getstorageitem: Non-existant item %d requested.\n", nameid);
			return false;
		}
	}

	struct item it;
	memset(&it, 0, sizeof(it));
	it.nameid = nameid;
	it.identify = 1;
	if (itemdb->isstackable(nameid))
		storage->additem(sd, &it, amount);
	else
		for (int i = 0; i < amount; i++)
			storage->additem(sd, &it, 1);

	clif->storageclose(sd);
	sd->state.storage_flag = STORAGE_FLAG_CLOSED;
	return true;
}

BUILDIN(countstorageitem) {
	struct map_session_data *sd = NULL;
	if (script_hasdata(st,3))
		sd = map->id2sd(script_getnum(st,3));
	else
		sd = script->rid2sd(st);
	if (sd == NULL)
		return true;

	int nameid = 0;
	if (script_isstring(st,2)) {
		const char *name = script_getstr(st,2);
		struct item_data *idata = itemdb->search_name(name);
		if (idata == NULL) {
			script_pushint(st,0);
			ShowError("buildin_countstorageitem: Non-existant item %s requested.\n", name);
			return false;
		}
		nameid = idata->nameid;
	}
	else {
		nameid = script_getnum(st,2);
		if (itemdb->exists(nameid) == 0) {
			script_pushint(st,0);
			ShowError("buildin_countstorageitem: Non-existant item %d requested.\n", nameid);
			return false;
		}
	}

	int count = 0;
	for (int i = 0; i < VECTOR_LENGTH(sd->storage.item); i++) {
		struct item *sitem = &VECTOR_INDEX(sd->storage.item, i);
		nullpo_ret(sitem);
		if (sitem->nameid == nameid)
			count += sitem->amount;
	}

	script_pushint(st, count);
	return true;
}

BUILDIN(delstorageitem) {
	struct map_session_data *sd = NULL;
	if (script_hasdata(st,4))
		sd = map->id2sd(script_getnum(st,4));
	else
		sd = script->rid2sd(st);
	if (sd == NULL)
		return true;

	int amount = script_getnum(st,3);
	if (amount <= 0)
		return true;

	int nameid = 0;
	if (script_isstring(st,2)) {
		const char *name = script_getstr(st,2);
		struct item_data *idata = itemdb->search_name(name);
		if (idata == NULL) {
			ShowError("buildin_delstorageitem: Non-existant item (%s) requested.\n", name);
			st->state = END;
			return false;
		}
		nameid = idata->nameid;
	}
	else {
		nameid = script_getnum(st,2);
		if (itemdb->exists(nameid) == 0) {
			ShowError("buildin_delstorageitem: Non-existant item (%d) requested.\n", nameid);
			st->state = END;
			return false;
		}
	}

	if (itemdb->isstackable(nameid)) {
		for (int i = 0; i < VECTOR_LENGTH(sd->storage.item); i++) {
			struct item *sitem = &VECTOR_INDEX(sd->storage.item, i);
			nullpo_ret(sitem);
			if (sitem->nameid == nameid)
				storage->delitem(sd, i, (amount > sitem->amount)? sitem->amount : amount);
		}
	}
	else {
		int j = 0;
		for (int i = 0; i < VECTOR_LENGTH(sd->storage.item); i++) {
			struct item *sitem = &VECTOR_INDEX(sd->storage.item, i);
			nullpo_ret(sitem);
			if (sitem->nameid == nameid) {
				storage->delitem(sd, i, 1);
				j++;
				if (j == amount)
					break;
			}
		}
	}
	clif->storageclose(sd);
	sd->state.storage_flag = STORAGE_FLAG_CLOSED;
	return true;
}

BUILDIN(getstoragelist) {
	struct map_session_data *sd = script->rid2sd(st);
	if (sd == NULL)
		return true;

	char script_var[SCRIPT_VARNAME_LENGTH];
	int i = 0, j = 0, k = 0;

	for (i = 0; i < VECTOR_LENGTH(sd->storage.item); i++) {
		struct item *sitem = &VECTOR_INDEX(sd->storage.item, i);
		nullpo_ret(sitem);
		if (sitem->nameid > 0 && sitem->amount > 0) {
			pc->setreg(sd, reference_uid(script->add_variable("@storagelist_id"), j), sitem->nameid);
			pc->setreg(sd, reference_uid(script->add_variable("@storagelist_amount"), j), sitem->amount);
			pc->setreg(sd, reference_uid(script->add_variable("@storagelist_refine"), j), sitem->refine);
			pc->setreg(sd, reference_uid(script->add_variable("@storagelist_identify"), j), sitem->identify);
			pc->setreg(sd, reference_uid(script->add_variable("@storagelist_attribute"), j), sitem->attribute);
			for (k = 0; k < MAX_SLOTS; k++) {
				sprintf(script_var, "@storagelist_card%d", k + 1);
				pc->setreg(sd, reference_uid(script->add_variable(script_var), j), sitem->card[k]);
			}
			for (k = 0; k < MAX_ITEM_OPTIONS; k++) {
				sprintf(script_var, "@storagelist_opt_id%d", k + 1);
				pc->setreg(sd, reference_uid(script->add_variable(script_var), j), sitem->option[k].index);
				sprintf(script_var, "@storagelist_opt_val%d", k + 1);
				pc->setreg(sd, reference_uid(script->add_variable(script_var), j), sitem->option[k].value);
				sprintf(script_var, "@storagelist_opt_param%d", k + 1);
				pc->setreg(sd, reference_uid(script->add_variable(script_var), j), sitem->option[k].param);
			}
			pc->setreg(sd, reference_uid(script->add_variable("@storagelist_expire"), j), sitem->expire_time);
			pc->setreg(sd, reference_uid(script->add_variable("@storagelist_bound"), j), sitem->bound);
			pc->setreg(sd, reference_uid(script->add_variable("@storagelist_favorite"), j), sitem->favorite);
			pc->setreg(sd, reference_uid(script->add_variable("@storagelist_idx"), j), i);
			j++;
		}
	}
	pc->setreg(sd, script->add_variable("@storagelist_count"), j);
	return true;
}

BUILDIN(delstorageitemidx) {
	struct map_session_data *sd = NULL;
	if (script_hasdata(st,4))
		sd = map->id2sd(script_getnum(st,4));
	else
		sd = script->rid2sd(st);
	if (sd == NULL)
		return true;

	int index = script_getnum(st,2);
	if (index < 0 || index >= MAX_STORAGE) {
		ShowError("buildin_delstorageitemidx: Index (%d) should be from 0-%d.\n", index, MAX_STORAGE);
		st->state = END;
		return false;
	}

	struct item *sitem = &VECTOR_INDEX(sd->storage.item, index);
	if (sitem == NULL) {
		ShowError("buildin_delstorageitemidx: Index (%d) doesn't exist on this character's storage(max %d).\n", index, VECTOR_LENGTH(sd->storage.item));
		st->state = END;
		return false;
	}

	int amount = script_getnum(st,3);
	if (amount <= 0)
		return true;
	if (amount > sitem->amount) {
		ShowError("buildin_delstorageitemidx: Attempt to delete more amount(%d) then the stack(%d) on that index.\n", amount, sitem->amount);
		ShowError("buildin_delstorageitemidx: Use *getstoragelist to confirm the amount on that stack.\n");
		st->state = END;
		return false;
	}

	storage->delitem(sd, index, amount);

	clif->storageclose(sd);
	sd->state.storage_flag = STORAGE_FLAG_CLOSED;
	return true;
}

HPExport void plugin_init(void) {
	addScriptCommand("getstorageitem", "vi?", getstorageitem);
	addScriptCommand("countstorageitem", "v?", countstorageitem);
	addScriptCommand("delstorageitem", "vi?", delstorageitem);
	addScriptCommand("getstoragelist", "", getstoragelist);
	addScriptCommand("delstorageitemidx", "ii?", delstorageitemidx);
}
