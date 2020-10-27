//===== Hercules Plugin ======================================
//= noitem mapflag
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.9
//===== Compatible With: ===================================== 
//= Hercules 2020-09-30
//===== Description: =========================================
//= block players from using items
//===== Topic ================================================
//= http://herc.ws/board/topic/4830-noitem-mapflag/
//===== Additional Comments: =================================  
//= prontera   mapflag   noitem   IT_WEAPON,IT_ARMOR
//= block players from equipping weapon and armor
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/map.h"
#include "map/npc.h"
#include "common/conf.h"
#include "common/memmgr.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"noitem",
	SERVER_TYPE_MAP,
	"1.9",
	HPM_VERSION,
};

struct mapflag_data {
	VECTOR_DECL(int) noitemlist;
};

//	flush all noitem mapflag back to default upon @reloadscript and @mapexit
void mapflag_remove(void) {
	int i;
	for (i = 0; i < map->count; ++i) {
		struct mapflag_data *mf = getFromMAPD(&map->list[i], 0);
		if (mf != NULL) {
			VECTOR_CLEAR(mf->noitemlist);
			removeFromMAPD(&map->list[i], 0);
		}
	}
	return;
}

void npc_parse_unknown_mapflag_pre(const char **name, const char **w3, const char **w4, const char **start, const char **buffer, const char **filepath, int **retval) {
	if (strcmp( *w3, "noitem" ) != 0)
		return;
	size_t l = strlen(*w4);
	if (l == 0) {
		ShowWarning( "npc_parse_mapflag: no Item ID/type input.\n           Mapflag noitem: At %s (file '%s', line '%d').\n", *name, *filepath, strline(*buffer, *start - *buffer));
		hookStop();
		return;
	}
	int id = 0, k = 0;
	unsigned short i = 0, j = 0;
	char *str = aStrdup(*w4), *temp = (char*)aMalloc(l +1);
	struct item_data *idata = NULL;
	int16 m = map->mapname2mapid( *name );
	struct mapflag_data *mf = getFromMAPD(&map->list[m], 0);
	if (mf != NULL) {
		VECTOR_CLEAR(mf->noitemlist);
		removeFromMAPD(&map->list[m], 0);
	}
	CREATE(mf, struct mapflag_data, 1);
	VECTOR_INIT(mf->noitemlist);
	while (i <= l) {
		if (str[i] != ' ' && str[i] != '	' && str[i] != ',' && str[i] != '\0') {
			temp[j++] = str[i];
		}
		else if (str[i-1] != ' ' && str[i-1] != '	' && str[i-1] != ',') {
			temp[j] = '\0';
			if (!strcmp( temp, "IT_HEALING" ) || !strcmp(temp, "0"))
				id = 0;
			else if (!strcmp(temp, "IT_USABLE") || !strcmp(temp, "2"))
				id = 2;
			else if (!strcmp(temp, "IT_WEAPON") || !strcmp(temp, "4"))
				id = 4;
			else if (!strcmp(temp, "IT_ARMOR") || !strcmp(temp, "5"))
				id = 5;
			else if (!strcmp(temp, "IT_CARD") || !strcmp(temp, "6"))
				id = 6;
			else if (!strcmp(temp, "IT_AMMO") || !strcmp(temp, "10"))
				id = 10;
			else if (!strcmp(temp, "IT_DELAYCONSUME") || !strcmp(temp, "11"))
				id = 11;
			else if (!strcmp(temp, "IT_CASH") || !strcmp( temp,"18"))
				id = 18;
			else if (atoi(temp) == 0) {
				idata = itemdb->search_name(temp);
				if (idata != NULL)
					id = idata->nameid;
				else {
					ShowWarning("npc_parse_mapflag: Item name \"%s\" does not exist.\n    Mapflag noitem: At %s (file '%s', line '%d').\n", temp, *name, *filepath, strline( *buffer, *start - *buffer));
					id = 0;
				}
			}
			else {
				id = atoi(temp);
				if (itemdb->exists(id) == NULL) {
					ShowWarning("npc_parse_mapflag: Item ID \"%s\" does not exist.\n    Mapflag noitem: At %s (file '%s', line '%d').\n", temp, *name, *filepath, strline(*buffer, *start - *buffer));
					id = 0;
				}
			}
			if (id > 0) {
				k = 0;
				ARR_FIND(0, VECTOR_LENGTH(mf->noitemlist), k, id == VECTOR_INDEX(mf->noitemlist, k));
				if (k < VECTOR_LENGTH(mf->noitemlist)) {
					if (id > 500) {
						idata = itemdb->search(id);
						ShowWarning("npc_parse_mapflag: Item \"%s(%d)\" is being repeated.\n    Mapflag noitem: At %s (file '%s', line '%d').\n", idata->jname, id, *name, *filepath, strline(*buffer, *start - *buffer));
					}
					else {
						char item_type[ITEM_NAME_LENGTH];
						switch(id) {
						case 0: safesnprintf(item_type, ITEM_NAME_LENGTH, "IT_HEALING"); break;
						case 2: safesnprintf(item_type, ITEM_NAME_LENGTH, "IT_USABLE"); break;
						case 4: safesnprintf(item_type, ITEM_NAME_LENGTH, "IT_WEAPON"); break;
						case 5: safesnprintf(item_type, ITEM_NAME_LENGTH, "IT_ARMOR"); break;
						case 6: safesnprintf(item_type, ITEM_NAME_LENGTH, "IT_CARD"); break;
						case 10: safesnprintf(item_type, ITEM_NAME_LENGTH, "IT_AMMO"); break;
						case 11: safesnprintf(item_type, ITEM_NAME_LENGTH, "IT_DELAYCONSUME"); break;
						case 18: safesnprintf(item_type, ITEM_NAME_LENGTH, "IT_CASH"); break;
						default: return; // hopefully doesn't run this
						}
						ShowWarning("npc_parse_mapflag: Item type \"%s(%d)\" is being repeated.\n    Mapflag noitem: At %s (file '%s', line '%d').\n", item_type, id, *name, *filepath, strline(*buffer, *start - *buffer));
					}
				}
				else {
					VECTOR_ENSURE(mf->noitemlist, 1, 1);
					VECTOR_PUSH(mf->noitemlist, id);
				}
			}
			j = 0;
			id = 0;
		}
		++i;
	}
	aFree(temp);
	aFree(str);
	addToMAPD(&map->list[m], mf, 0, true);
	hookStop();
	return;
}

int pc_isequip_post(int retVal, struct map_session_data *sd, int n) {
	if (retVal == 1) {
		struct mapflag_data *mf = getFromMAPD(&map->list[sd->bl.m], 0);
		if (mf && VECTOR_LENGTH(mf->noitemlist)) {
			struct item_data *item = sd->inventory_data[n];
			int i = 0, slot = 0;
			ARR_FIND(0, VECTOR_LENGTH(mf->noitemlist), i, item->nameid == VECTOR_INDEX(mf->noitemlist, i) || item->type == VECTOR_INDEX(mf->noitemlist, i));
			if (i < VECTOR_LENGTH(mf->noitemlist))
				return 0;
			if (!itemdb_isspecial( sd->status.inventory[n].card[0])) {
				for (slot = 0; slot < MAX_SLOTS; ++slot) {
					if (sd->status.inventory[n].card[slot]) {
						struct item_data *i_data = itemdb->exists(sd->status.inventory[n].card[slot]);
						ARR_FIND(0, VECTOR_LENGTH(mf->noitemlist), i, i_data->nameid == VECTOR_INDEX(mf->noitemlist, i) || i_data->type == VECTOR_INDEX(mf->noitemlist, i));
						if (i < VECTOR_LENGTH(mf->noitemlist))
							return 0;
					}
				}
			}
		}
	}
	return retVal;
}

int pc_isUseitem_post(int retVal, struct map_session_data *sd, int n) {
	if (retVal == 1) {
		struct mapflag_data *mf = getFromMAPD( &map->list[sd->bl.m], 0);
		if (mf && VECTOR_LENGTH(mf->noitemlist)) {
			struct item_data *item = sd->inventory_data[n];
			int i = 0;
			ARR_FIND(0, VECTOR_LENGTH(mf->noitemlist), i, VECTOR_INDEX(mf->noitemlist, i) == sd->status.inventory[n].nameid || item->type == VECTOR_INDEX(mf->noitemlist, i));
			if (i < VECTOR_LENGTH(mf->noitemlist))
				return 0;
		}
	}
	return retVal;
}

int pc_checkitem_post(int retVal, struct map_session_data *sd) {
	struct mapflag_data *mf = getFromMAPD(&map->list[sd->bl.m], 0);
	int i, calc_flag = 0;
	if (sd->state.vending)
		return 0;
	if (mf && VECTOR_LENGTH(mf->noitemlist)) {
		for (i = 0; i < MAX_INVENTORY; ++i) {
			int j = 0, slot = 0;
			struct item_data *i_data = itemdb->exists(sd->status.inventory[i].nameid);
			if (sd->status.inventory[i].nameid == 0 || !sd->status.inventory[i].equip)
				continue;
			ARR_FIND(0, VECTOR_LENGTH(mf->noitemlist), j, VECTOR_INDEX(mf->noitemlist, j) == i_data->type || VECTOR_INDEX(mf->noitemlist, j) == sd->status.inventory[i].nameid);
			if (j < VECTOR_LENGTH(mf->noitemlist)) {
				pc->unequipitem(sd, i, 2);
				calc_flag = 1;
				continue;
			}
			if (!itemdb_isspecial( sd->status.inventory[i].card[0])) {
				for (slot = 0; slot < MAX_SLOTS; ++slot) {
					if (sd->status.inventory[i].card[slot]) {
						struct item_data *i_datac = itemdb->exists( sd->status.inventory[i].card[slot]);
						ARR_FIND( 0, VECTOR_LENGTH(mf->noitemlist), j, VECTOR_INDEX(mf->noitemlist, j) == i_datac->type || VECTOR_INDEX(mf->noitemlist, j) == sd->status.inventory[i].card[slot]);
						if (j < VECTOR_LENGTH(mf->noitemlist)) {
							pc->unequipitem(sd, i, 2);
							calc_flag = 1;
							break;
						}
					}
				}
			}
		}
		if (calc_flag && sd->state.active) {
			pc->checkallowskill(sd);
			status_calc_pc(sd, SCO_NONE);
		}
	}
	return retVal;
}

HPExport void plugin_init (void) {
	addHookPre(map, flags_init, mapflag_remove);
	addHookPre(map, list_final, mapflag_remove);
	addHookPre(npc, parse_unknown_mapflag, npc_parse_unknown_mapflag_pre);
	addHookPost(pc, isequip, pc_isequip_post);
	addHookPost(pc, isUseitem, pc_isUseitem_post);
	addHookPost(pc, checkitem, pc_checkitem_post);
}
