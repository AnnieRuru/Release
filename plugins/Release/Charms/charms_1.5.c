//===== Hercules Plugin ======================================
//= Charms
//===== By: ==================================================
//= AnnieRuru
//= original by digitalhamster
//===== Current Version: =====================================
//= 1.5
//===== Compatible With: ===================================== 
//= Hercules 2020-11-12
//===== Description: =========================================
//= Give item bonus like the game DiabloII
//===== Topic ================================================
//= http://herc.ws/board/topic/11575-charms/
//===== Additional Comments: =================================
//= if you set Charm_Stack: true, it still run the status_calc_pc even if
//= - you already having the charm, but just add it up
//= - you have 10 charms in stack (give only 1 time bonus), but drop 5 charms
//= it still run status_calc_pc everytime, and I don't know a better way to do this ...
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/npc.h"
#include "map/itemdb.h"
#include "map/status.h"
#include "common/conf.h"
#include "common/memmgr.h"
#include "common/nullpo.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"charms",
	SERVER_TYPE_MAP,
	"1.5",
	HPM_VERSION,
};

struct charm_item_data {
	bool charm;
	bool charm_stack;
};

struct mapflag_data {
	VECTOR_DECL(int) nocharmslist;
};

struct player_data { // this stupid player variable is needed to prevent item dup
	bool recalculate;
};

void itemdb_readdb_additional_fields_pre( int *itemid, struct config_setting_t **it, int *n, const char **source ) {
	struct item_data *idata = itemdb->load(*itemid);
	if (idata->type != IT_ETC)
		return;
	struct charm_item_data *cidata = getFromITEMDATA(idata, 0);
	if (cidata == NULL) {
		CREATE(cidata, struct charm_item_data, 1);
		addToITEMDATA(idata, cidata, 0, true);
	}
	struct config_setting_t *tt;
	if ((tt = libconfig->setting_get_member(*it, "Charm" )))
		if (libconfig->setting_get_bool(tt))
			cidata->charm = true;
	if ((tt = libconfig->setting_get_member(*it, "Charm_Stack")))
		if (libconfig->setting_get_bool(tt))
			cidata->charm_stack = true;
	return;
}

int itemdb_isstackable_pre(int *nameid) {
	struct item_data *idata = itemdb->search(*nameid);
	if (idata == NULL)
		return 1;
	if (idata->type != IT_ETC)
		return 1;
	struct charm_item_data *cidata = getFromITEMDATA(idata, 0);
	if (cidata == NULL)
		return 1;
	if (cidata->charm_stack == true) {
		hookStop();
		return 1;
	}
	if (cidata->charm == true) {
		hookStop();
		return 0;
	}
	return 1;
}

int itemdb_isstackable2_pre(struct item_data **data) {
	if (data == NULL)
		return 1;
	if ((*data)->type != IT_ETC)
		return 1;
	struct charm_item_data *cidata = getFromITEMDATA(*data, 0);
	if (cidata == NULL)
		return 1;
	if (cidata->charm_stack == true) {
		hookStop();
		return 1;
	}
	if (cidata->charm == true) {
		hookStop();
		return 0;
	}
	return 1;
}

//	flush all nocharms mapflag back to default upon @reloadscript and @mapexit
void mapflag_remove(void) {
	int i;
	for (i = 0; i < map->count; ++i) {
		struct mapflag_data *mf = getFromMAPD(&map->list[i], 0);
		if (mf != NULL) {
			VECTOR_CLEAR(mf->nocharmslist);
			removeFromMAPD(&map->list[i], 0);
		}
	}
	return;
}

void npc_parse_unknown_mapflag_pre(const char **name, const char **w3, const char **w4, const char **start, const char **buffer, const char **filepath, int **retval) {
	if (strcmp( *w3, "nocharms" ))
		return;
	size_t l = strlen(*w4);
	unsigned short i = 0, j = 0;
	int id = 0, k = 0;
	char *str = aStrdup(*w4), *temp = (char*)aMalloc(l +1);
	int16 m = map->mapname2mapid( *name );
	struct item_data *idata = NULL;
	struct charm_item_data *cidata = NULL;
	struct mapflag_data *mf = getFromMAPD(&map->list[m], 0);
	if (mf != NULL) {
		VECTOR_CLEAR(mf->nocharmslist);
		removeFromMAPD(&map->list[m], 0);
	}
	CREATE(mf, struct mapflag_data, 1);
	VECTOR_INIT(mf->nocharmslist);
	if (l == 0) {
		VECTOR_ENSURE(mf->nocharmslist, 1, 1);
		VECTOR_PUSH(mf->nocharmslist, -1);
	}
	else {
		while (i <= l) {
			if (str[i] != ' ' && str[i] != '	' && str[i] != ',' && str[i] != '\0') {
				temp[j++] = str[i];
			}
			else if (str[i-1] != ' ' && str[i-1] != '	' && str[i-1] != ',') {
				temp[j] = '\0';
				if (atoi(temp) == 0) {
					idata = itemdb->search_name(temp);
					if (idata != NULL)
						id = idata->nameid;
					else
						ShowWarning("npc_parse_mapflag: Item name \"%s\" does not exist.\n    Mapflag nocharms: At %s (file '%s', line '%d').\n", temp, *name, *filepath, strline(*buffer, *start - *buffer));
				}
				else {
					id = atoi(temp);
					if (itemdb->exists(id) == NULL) {
						ShowWarning("npc_parse_mapflag: Item ID \"%s\" does not exist.\n    Mapflag nocharms: At %s (file '%s', line '%d').\n", temp, *name, *filepath, strline(*buffer, *start - *buffer));
						id = 0;
					}
				}
				if (id > 0) {
					idata = itemdb->search(id);
					ARR_FIND(0, VECTOR_LENGTH(mf->nocharmslist), k, id == VECTOR_INDEX(mf->nocharmslist, k));
					if (k < VECTOR_LENGTH(mf->nocharmslist))
						ShowWarning( "npc_parse_mapflag: Item \"%s(%d)\" is being repeated.\n    Mapflag nocharms: At %s (file '%s', line '%d').\n", idata->jname, id, *name, *filepath, strline(*buffer, *start - *buffer));
					else if (idata->type != IT_ETC)
						ShowWarning( "npc_parse_mapflag: Item \"%s(%d)\" is not a Charms.\n    Mapflag nocharms: At %s (file '%s', line '%d').\n", idata->jname, id, *name, *filepath, strline(*buffer, *start - *buffer));
					else {
						cidata = getFromITEMDATA(idata, 0);
						if (cidata == NULL)
							ShowWarning("npc_parse_mapflag: Item \"%s(%d)\" is not a Charms.\n    Mapflag nocharms: At %s (file '%s', line '%d').\n", idata->jname, id, *name, *filepath, strline(*buffer, *start - *buffer));
						else if (cidata->charm == false)
							ShowWarning("npc_parse_mapflag: Item \"%s(%d)\" is not a Charms.\n    Mapflag nocharms: At %s (file '%s', line '%d').\n", idata->jname, id, *name, *filepath, strline(*buffer, *start - *buffer));
						else {
							VECTOR_ENSURE(mf->nocharmslist, 1, 1);
							VECTOR_PUSH(mf->nocharmslist, id);
						}
					}
				}
				id = 0;
				j = 0;
			}
			++i;
		}
	}
	aFree(temp);
	aFree(str);
	addToMAPD(&map->list[m], mf, 0, true);
//	ShowDebug("%d\n", VECTOR_LENGTH(mf->nocharmslist));
//	for (i = 0; i < VECTOR_LENGTH(mf->nocharmslist); ++i)
//		ShowDebug("%d", VECTOR_INDEX(mf->nocharmslist, i));
	hookStop();
	return;
}

// TODO: Maybe should add those job/level/upper flag restriction like I did on eathena ? hmm ... but hercules omit those fields ... using default
void status_calc_pc_additional_pre(struct map_session_data **sd, enum e_status_calc_opt *opt) {
	int i = 0, j = 0;
	struct mapflag_data *mf = getFromMAPD(&map->list[(*sd)->bl.m], 0);
	if (mf != NULL)
		if (VECTOR_LENGTH(mf->nocharmslist) > 0)
			if ( VECTOR_INDEX(mf->nocharmslist, 0) == -1)
				return;
	struct charm_item_data *cidata = NULL;
	for (i = 0; i < MAX_INVENTORY; ++i) {
		if ( (*sd)->inventory_data[i] == NULL)
			continue;
		if ((*sd)->inventory_data[i]->type != IT_ETC)
			continue;
		cidata = getFromITEMDATA( (*sd)->inventory_data[i], 0);
		if (cidata == NULL)
			continue;
		if (cidata->charm == false)
			continue;
		if (mf != NULL) {
			ARR_FIND(0, VECTOR_LENGTH(mf->nocharmslist), j, (*sd)->status.inventory[i].nameid == VECTOR_INDEX(mf->nocharmslist, j));
			if (j < VECTOR_LENGTH(mf->nocharmslist))
				continue;
		}
		if ((*sd)->inventory_data[i]->script)
			script->run((*sd)->inventory_data[i]->script, 0, (*sd)->bl.id, 0);
	}
	return;
}

int pc_additem_post(int retVal, struct map_session_data *sd, const struct item *item_data ,int amount, e_log_pick_type log_type) {
	if (retVal != 0)
		return retVal;
	struct item_data *idata = itemdb->search(item_data->nameid);
	if (idata->type != IT_ETC ) 
		return retVal;
	struct charm_item_data *cidata = getFromITEMDATA(idata, 0);
	if (cidata == NULL)
		return retVal;
	if (cidata->charm == true) {
//		ShowDebug("run recalculation");
		status_calc_pc(sd, SCO_NONE);
		if (idata->equip_script)
			script->run(idata->equip_script, 0, sd->bl.id, 0);
	}
	return retVal;
}

int pc_delitem_pre(struct map_session_data **sd, int *n, int *amount, int *type, short *reason, e_log_pick_type *log_type) {
	if (*sd == NULL)
		return 0;
	if ((*sd)->status.inventory[*n].nameid == 0 || *amount <= 0 || (*sd)->status.inventory[*n].amount < *amount || (*sd)->inventory_data[*n] == NULL) {
		hookStop();
		return 1;
	}
	if ((*sd)->inventory_data[*n]->type != IT_ETC)
		return 0;
	struct charm_item_data *cidata = getFromITEMDATA((*sd)->inventory_data[*n], 0);
	if (cidata == NULL)
		return 0;
	if (cidata->charm == true) {
		struct player_data *ssd = getFromMSD(*sd, 0);
		if (ssd == NULL) {
			CREATE(ssd, struct player_data, 1);
			ssd->recalculate = 1;
			addToMSD(*sd, ssd, 0, true);
		}
		else
			ssd->recalculate = 1;
		if ((*sd)->inventory_data[*n]->unequip_script)
			script->run((*sd)->inventory_data[*n]->unequip_script, 0, (*sd)->bl.id, 0);
	}
	return 0;
}
// maybe I should've just overload this function ...
int pc_delitem_post(int retVal, struct map_session_data *sd, int n, int amount, int type, short reason, e_log_pick_type log_type) {
	struct player_data *ssd = getFromMSD(sd, 0);
	nullpo_ret(ssd);
	if (ssd != NULL && ssd->recalculate == 1) {
//		ShowDebug("run recalculation");
		status_calc_pc(sd, SCO_NONE);
		ssd->recalculate = 0;
	}
	return retVal;
}

HPExport void plugin_init (void) {
	addHookPre(itemdb, readdb_additional_fields, itemdb_readdb_additional_fields_pre);
	addHookPre(itemdb, isstackable, itemdb_isstackable_pre);
	addHookPre(itemdb, isstackable2, itemdb_isstackable2_pre);
	addHookPre(map, flags_init, mapflag_remove);
	addHookPre(map, list_final, mapflag_remove);
	addHookPre(npc, parse_unknown_mapflag, npc_parse_unknown_mapflag_pre);
	addHookPre(status, calc_pc_additional, status_calc_pc_additional_pre);
	addHookPost(pc, additem, pc_additem_post);
	addHookPre(pc, delitem, pc_delitem_pre);
	addHookPost(pc, delitem, pc_delitem_post);
}
