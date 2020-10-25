//===== Hercules Plugin ======================================
//= cell_pvp
//===== By: ==================================================
//= AnnieRuru
//= based on [Ize] source code
//===== Current Version: =====================================
//= 1.0
//===== Compatible With: ===================================== 
//= Hercules 2020-10-22
//===== Description: =========================================
//= create a custom zone for players to pvp
//===== Topic ================================================
//= https://herc.ws/board/topic/19033-cell_pvp/
//===== Additional Comments: =================================
//= @cell_pvp 153 192 163 202
//= there's a bug, if you enable the @cell_pvp, then turn it off, still ok
//= but turn it on again doesn't work. @pvpon still works though ???
//= I purposely left the debug line, the problem seems to cause by VECTOR ...?
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/map.h"
#include "map/atcommand.h"
#include "common/timer.h"
#include "common/memmgr.h"
#include "common/nullpo.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"cell_pvp",
	SERVER_TYPE_MAP,
	"1.0",
	HPM_VERSION,
};

struct player_data {
	unsigned cellpvp_flag : 1;
};

struct cellpvpdata {
	int16 mapid;
	int x1;
	int y1;
	int x2;
	int y2;
};

VECTOR_DECL(struct cellpvpdata) cellpvp;

void walkin_cellpvp(struct map_session_data *sd) {
	nullpo_retv(sd);
	clif->map_property(sd, MAPPROPERTY_FREEPVPZONE);
	clif->maptypeproperty2(&sd->bl, SELF);
	if (sd->pvp_timer == INVALID_TIMER) {
		if (!map->list[sd->bl.m].flag.pvp_nocalcrank)
			sd->pvp_timer = timer->add(timer->gettick() + 200, pc->calc_pvprank_timer, sd->bl.id, 0);
		sd->pvp_rank = 0;
		sd->pvp_lastusers = 0;
		sd->pvp_point = 5;
		sd->pvp_won = 0;
		sd->pvp_lost = 0;
	}
	struct player_data *ssd = getFromMSD(sd, 0);
	nullpo_retv(ssd);
	ssd->cellpvp_flag = 1;
	return;
}

void walkout_cellpvp(struct map_session_data *sd) {
	nullpo_retv(sd);
	clif->map_property(sd, MAPPROPERTY_NOTHING);
	clif->maptypeproperty2(&sd->bl, SELF);
	clif->pvpset(sd, 0, 0, 2);
	if (sd->pvp_timer != INVALID_TIMER) {
		timer->delete(sd->pvp_timer, pc->calc_pvprank_timer);
		sd->pvp_timer = INVALID_TIMER;
	}
	struct player_data *ssd = getFromMSD(sd, 0);
	nullpo_retv(ssd);
	ssd->cellpvp_flag = 0;
	return;
}

int buildin_cellpvp(struct block_list *bl, va_list ap) {
	nullpo_ret(bl);
	Assert_ret(bl->type == BL_PC);
	struct map_session_data *sd = BL_CAST(BL_PC, bl);
	int flag = va_arg(ap, int);
	if (flag == true)
		walkin_cellpvp(sd);
	else
		walkout_cellpvp(sd);
	return true;
}

static int pc_reg_received_post(int retVal, struct map_session_data *sd) {
	if (retVal == 0 || sd == NULL)
		return retVal;
	struct player_data *ssd;
	CREATE(ssd, struct player_data, true);
	ssd->cellpvp_flag = 0;
	addToMSD(sd, ssd, 0, true);
	return 1;
}

void clif_parse_LoadEndAck_post(int fd, struct map_session_data *sd) {
	if (sd == NULL)
		return;
	if (pc_isinvisible(sd))
		return;
	int i;
	ARR_FIND(0, VECTOR_LENGTH(cellpvp), i, VECTOR_INDEX(cellpvp, i).mapid == sd->bl.m);
	if (i < VECTOR_LENGTH(cellpvp)) {
		if (sd->bl.x < VECTOR_INDEX(cellpvp, i).x1 || sd->bl.y < VECTOR_INDEX(cellpvp, i).y1 || sd->bl.x > VECTOR_INDEX(cellpvp, i).x2 || sd->bl.y > VECTOR_INDEX(cellpvp, i).y2)
			walkout_cellpvp(sd);
		else
			walkin_cellpvp(sd);
	}
	return;
}

static int pc_calc_pvprank_sub(struct block_list *bl, va_list ap) {
	struct map_session_data *sd1 = NULL;
	struct map_session_data *sd2 = va_arg(ap,struct map_session_data *);
	int *count = va_arg(ap, int *);
	nullpo_ret(bl);
	Assert_ret(bl->type == BL_PC);
	sd1 = BL_UCAST(BL_PC, bl);
	nullpo_ret(sd2);
	if (pc_isinvisible(sd1) || pc_isinvisible(sd2))
		return 0;
	if (sd1->pvp_point > sd2->pvp_point)
		sd2->pvp_rank++;
	(*count)++;
	return 0;
}

static int pc_calc_pvprank_pre(struct map_session_data **sd) {
	nullpo_ret(*sd);
	int i, count = 0;
	ARR_FIND(0, VECTOR_LENGTH(cellpvp), i, VECTOR_INDEX(cellpvp, i).mapid == (*sd)->bl.m);
	if (i < VECTOR_LENGTH(cellpvp)) {
		struct map_data *m = &map->list[(*sd)->bl.m];
		int old = (*sd)->pvp_rank;
		(*sd)->pvp_rank = 1;
		map->foreachinarea(pc_calc_pvprank_sub, (*sd)->bl.m, VECTOR_INDEX(cellpvp, i).x1, VECTOR_INDEX(cellpvp, i).y1, VECTOR_INDEX(cellpvp, i).x2, VECTOR_INDEX(cellpvp, i).y2, BL_PC, *sd, &count);
		if (old != (*sd)->pvp_rank || (*sd)->pvp_lastusers != count)
			clif->pvpset(*sd, (*sd)->pvp_rank, (*sd)->pvp_lastusers = count, 0);
		hookStop();
	}
	return (*sd)->pvp_rank;
}

static int pc_respawn_timer_pre(int *tid, int64 *tick, int *id, intptr_t *data) {
	struct map_session_data *sd = map->id2sd(*id);
	if (sd == NULL)
		return 0;
	int i;
	ARR_FIND(0, VECTOR_LENGTH(cellpvp), i, VECTOR_INDEX(cellpvp, i).mapid == sd->bl.m);
	if (i < VECTOR_LENGTH(cellpvp)) {
		sd->pvp_point = 0;
		hookStop();
	}
	return 0;
}

static void map_zone_change2_pre(int *m, struct map_zone_data **zone) {
	if (*zone == NULL)
		return;
	if (*zone != map->list[*m].prev_zone)
		return;
	int i;
	ARR_FIND(0, VECTOR_LENGTH(cellpvp), i, VECTOR_INDEX(cellpvp, i).mapid == *m);
	if (i < VECTOR_LENGTH(cellpvp)) {
		map->foreachinmap(buildin_cellpvp, *m, BL_PC, false);
		VECTOR_ERASE(cellpvp, i);
	}

//	for (int i = 0; i < VECTOR_LENGTH(cellpvp); i++)
//		ShowDebug("[%d/%d]. %s %d %d %d %d\n", i +1, VECTOR_LENGTH(cellpvp) , mapindex_id2name(map_id2index(VECTOR_INDEX(cellpvp, i).mapid)), VECTOR_INDEX(cellpvp, i).x1, VECTOR_INDEX(cellpvp, i).y1, VECTOR_INDEX(cellpvp, i).x2, VECTOR_INDEX(cellpvp, i).y2);
//	ShowDebug("VECTOR_LENGTH(cellpvp) : %d | VECTOR_CAPACITY(cellpvp) | %d\n", VECTOR_LENGTH(cellpvp), VECTOR_CAPACITY(cellpvp));
	return;
}

static int battle_check_target_post(int retVal, struct block_list *src, struct block_list *target, int flag) {
	if (src == NULL || target == NULL)
		return retVal;
	struct block_list *s_bl = src, *t_bl = target;
	if ((t_bl = battle->get_master(target)) == NULL)
		t_bl = target;
	if ((s_bl = battle->get_master(src)) == NULL)
		s_bl = src;
	if (s_bl->type == BL_PC && t_bl->type == BL_PC) {
		struct map_session_data *sd = BL_CAST(BL_PC, s_bl), *tsd = BL_CAST(BL_PC, t_bl);
		nullpo_ret(sd);
		nullpo_ret(tsd);
		if (retVal == 1) {
			int i;
			ARR_FIND(0, VECTOR_LENGTH(cellpvp), i, VECTOR_INDEX(cellpvp, i).mapid == sd->bl.m);
			if (i < VECTOR_LENGTH(cellpvp)) {
				if (sd->bl.x < VECTOR_INDEX(cellpvp, i).x1 || tsd->bl.x < VECTOR_INDEX(cellpvp, i).x1 || sd->bl.y < VECTOR_INDEX(cellpvp, i).y1 || tsd->bl.y < VECTOR_INDEX(cellpvp, i).y1 || sd->bl.x > VECTOR_INDEX(cellpvp, i).x2 || tsd->bl.x > VECTOR_INDEX(cellpvp, i).x2 || sd->bl.y > VECTOR_INDEX(cellpvp, i).y2 || tsd->bl.y > VECTOR_INDEX(cellpvp, i).y2)
				return 0;
			}
		}
	}
	return retVal;
}

static int unit_walk_toxy_timer_post(int retVal, int tid, int64 tick, int id, intptr_t data) {
	struct block_list *bl = map->id2bl(id);
	if (bl == NULL)
		return retVal;
	if (bl->type != BL_PC)
		return retVal;
	struct map_session_data *sd = BL_CAST(BL_PC, bl);
	if (sd == NULL)
		return retVal;
	int i;
	ARR_FIND(0, VECTOR_LENGTH(cellpvp), i, VECTOR_INDEX(cellpvp, i).mapid == sd->bl.m);
//	ShowDebug("VECTOR_LENGTH %d || i %d\n", VECTOR_LENGTH(cellpvp), i);
	if (i < VECTOR_LENGTH(cellpvp)) {
//		ShowDebug("MAP FOUND\n");
		struct player_data *ssd = getFromMSD(sd, 0);
		nullpo_ret(ssd);
		if (sd->bl.x < VECTOR_INDEX(cellpvp, i).x1 || sd->bl.y < VECTOR_INDEX(cellpvp, i).y1 || sd->bl.x > VECTOR_INDEX(cellpvp, i).x2 || sd->bl.y > VECTOR_INDEX(cellpvp, i).y2) {
//			ShowDebug("outside");
			if (ssd->cellpvp_flag == true)
				walkout_cellpvp(sd);
		}
		else {
//			ShowDebug("inside");
			if (ssd->cellpvp_flag == false)
				walkin_cellpvp(sd);
		}
	}
	return retVal;
}

static int unit_blown_post(int retVal, struct block_list *bl, int dx, int dy, int count, int flag) {
	if (bl == NULL)
		return retVal;
	if (bl->type != BL_PC)
		return retVal;
	struct map_session_data *sd = BL_CAST(BL_PC, bl);
	if (sd == NULL)
		return retVal;
	int i;
	ARR_FIND(0, VECTOR_LENGTH(cellpvp), i, VECTOR_INDEX(cellpvp, i).mapid == sd->bl.m);
	if (i < VECTOR_LENGTH(cellpvp)) {
		struct player_data *ssd = getFromMSD(sd, 0);
		nullpo_ret(ssd);
		if (sd->bl.x < VECTOR_INDEX(cellpvp, i).x1 || sd->bl.y < VECTOR_INDEX(cellpvp, i).y1 || sd->bl.x > VECTOR_INDEX(cellpvp, i).x2 || sd->bl.y > VECTOR_INDEX(cellpvp, i).y2) {
			if (ssd->cellpvp_flag == true)
				walkout_cellpvp(sd);
		}
		else {
			if (ssd->cellpvp_flag == false)
				walkin_cellpvp(sd);
		}
	}
	return retVal;
}

ACMD(cell_pvp) {
	if (map->list[sd->bl.m].flag.pvp) {
		clif->message(fd, "PvP is already On.");
		return false;
	}
	int x_1, y_1, x_2, y_2;
	if (sscanf(message, "%d %d %d %d", &x_1, &y_1, &x_2, &y_2) < 4) {
		char msg[CHAT_SIZE_MAX];
		clif->message(fd, "Syntax: @cell_pvp x1 y1 x2 y2");
		safesnprintf(msg, CHAT_SIZE_MAX, "Example: @cell_pvp %d %d %d %d", sd->bl.x, sd->bl.y, sd->bl.x +10, sd->bl.y +10);
		clif->message(fd, msg);
		return false;
	}
	struct cellpvpdata cellpvp_;
	cellpvp_.mapid = sd->bl.m;
	cellpvp_.x1 = x_1;
	cellpvp_.y1 = y_1;
	cellpvp_.x2 = x_2;
	cellpvp_.y2 = y_2;
	VECTOR_ENSURE(cellpvp, 1, 1);
	VECTOR_PUSH(cellpvp, cellpvp_);
	map->zone_change2(sd->bl.m, strdb_get(map->zone_db, MAP_ZONE_PVP_NAME));
	map->list[sd->bl.m].flag.pvp = 1;
	map->foreachinarea(buildin_cellpvp, sd->bl.m, x_1, y_1, x_2, y_2, BL_PC, true);

//	char msg[CHAT_SIZE_MAX];
//	for (int i = 0; i < VECTOR_LENGTH(cellpvp); i++) {
//		safesnprintf(msg, CHAT_SIZE_MAX, "[%d/%d]. %s %d %d %d %d", i +1, VECTOR_LENGTH(cellpvp) , mapindex_id2name(map_id2index(VECTOR_INDEX(cellpvp, i).mapid)), VECTOR_INDEX(cellpvp, i).x1, VECTOR_INDEX(cellpvp, i).y1, VECTOR_INDEX(cellpvp, i).x2, VECTOR_INDEX(cellpvp, i).y2);
//		clif->message(fd, msg);
//	}
//	safesnprintf(msg, CHAT_SIZE_MAX, "VECTOR_LENGTH(cellpvp) : %d | VECTOR_CAPACITY(cellpvp) | %d", VECTOR_LENGTH(cellpvp), VECTOR_CAPACITY(cellpvp));
//	clif->message(fd, msg);
	return true;
}
/*
ACMD(debug) {
	struct player_data *ssd = getFromMSD(sd, 0);
	nullpo_ret(ssd);
	char msg[CHAT_SIZE_MAX];
	safesnprintf(msg, CHAT_SIZE_MAX, "ssd->cellpvp_flag : %d", ssd->cellpvp_flag);
	clif->message(fd, msg);
//	for (int i = 0; i < VECTOR_LENGTH(cellpvp); i++) {
//		safesnprintf(msg, CHAT_SIZE_MAX, "[%d/%d]. %s %d %d %d %d", i +1, VECTOR_LENGTH(cellpvp) , mapindex_id2name(map_id2index(VECTOR_INDEX(cellpvp, i).mapid)), VECTOR_INDEX(cellpvp, i).x1, VECTOR_INDEX(cellpvp, i).y1, VECTOR_INDEX(cellpvp, i).x2, VECTOR_INDEX(cellpvp, i).y2);
//		clif->message(fd, msg);
//	}
//	safesnprintf(msg, CHAT_SIZE_MAX, "VECTOR_LENGTH(cellpvp) : %d | VECTOR_CAPACITY(cellpvp) | %d", VECTOR_LENGTH(cellpvp), VECTOR_CAPACITY(cellpvp));
//	clif->message(fd, msg);
	return true;
}
*/
BUILDIN(cell_pvp) {
	char mapname[MAP_NAME_LENGTH];
	if (strlen(script_getstr(st,2)) > MAP_NAME_LENGTH) {
		ShowError("buildin_cell_pvp: the map name must not longer than %d characters.\n", MAP_NAME_LENGTH);
		script_pushint(st, false);
		return false;
	}
	safestrncpy(mapname, script_getstr(st,2), MAP_NAME_LENGTH);
	int16 map_id = map->mapname2mapid(mapname);
	if (map_id < 0) {
		script_pushint(st, false);
		return false;
	}
	if (map->list[map_id].flag.pvp) {
		ShowError("buildin_cell_pvp: this map '%s' PvP is already On.", mapname);
		script_pushint(st, false);
		return false;
	}
	struct cellpvpdata cellpvp_;
	cellpvp_.mapid = map_id;
	cellpvp_.x1 = script_getnum(st,3);
	cellpvp_.y1 = script_getnum(st,4);
	cellpvp_.x2 = script_getnum(st,5);
	cellpvp_.y2 = script_getnum(st,6);
	VECTOR_ENSURE(cellpvp, 1, 1);
	VECTOR_PUSH(cellpvp, cellpvp_);
	map->zone_change2(map_id, strdb_get(map->zone_db, MAP_ZONE_PVP_NAME));
	map->list[map_id].flag.pvp = 1;
	map->foreachinarea(buildin_cellpvp, map_id, cellpvp_.x1, cellpvp_.y1, cellpvp_.x2, cellpvp_.y2, BL_PC, true);
	script_pushint(st, true);
	return true;
}

HPExport void plugin_init(void) {
	addHookPost(pc, reg_received, pc_reg_received_post);
	addHookPost(clif, pLoadEndAck, clif_parse_LoadEndAck_post);
	addHookPre(pc, calc_pvprank, pc_calc_pvprank_pre);
	addHookPre(pc, respawn_timer, pc_respawn_timer_pre);
	addHookPre(map, zone_change2, map_zone_change2_pre);
	addHookPost(battle, check_target, battle_check_target_post);
	addHookPost(unit, walk_toxy_timer, unit_walk_toxy_timer_post);
	addHookPost(unit, blown, unit_blown_post);
	addAtcommand("cell_pvp", cell_pvp);
//	addAtcommand("debug", debug);
	addScriptCommand("cell_pvp", "siiii", cell_pvp);
	VECTOR_INIT(cellpvp);
}

HPExport void plugin_final(void) {
	VECTOR_CLEAR(cellpvp);
}
