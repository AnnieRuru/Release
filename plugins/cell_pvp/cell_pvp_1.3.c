//===== Hercules Plugin ======================================
//= cell_pvp
//===== By: ==================================================
//= AnnieRuru
//= based on [Ize] source code
//===== Current Version: =====================================
//= 1.3
//===== Compatible With: ===================================== 
//= Hercules 2020-10-27
//===== Description: =========================================
//= create a custom zone for players to pvp
//===== Topic ================================================
//= https://herc.ws/board/topic/19033-cell_pvp/
//===== Additional Comments: =================================
//= this plugin overload `@pvpoff` atcommand and `*pvpoff` script command
//=
//= -- AtCommands --
//= @cell_pvp <x1> <y1> <x2> <y2> <respawn> <skillallow>
//= @pvpoff
//=
//= -- Script Commands --
//= *cell_pvp "<mapname>", <x1>, <y1>, <x2>, <y2>, <respawn>, <skillallow>;
//=   <respawn> flag are:
//=     CELL_PVP_RESPAWN_DISABLE
//=     CELL_PVP_RESPAWN_INSTANT
//=     CELL_PVP_RESPAWN_NORMAL
//=  <skillallow> : disallow skill usage outside pvp zone. This will disallow receiving buffs from players outside PvP Zone
//=     CELL_PVP_SKILL_DISALLOW
//=     CELL_PVP_SKILL_ALLOW
//=
//= remember MF_PVP_NOPARTY and MF_PVP_NOGUILD mapflag
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/map.h"
#include "map/battle.h"
#include "map/atcommand.h"
#include "common/timer.h"
#include "common/memmgr.h"
#include "common/nullpo.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"cell_pvp",
	SERVER_TYPE_MAP,
	"1.3",
	HPM_VERSION,
};

enum cellpvp_respawn_flag {
	CELL_PVP_RESPAWN_DISABLE,
	CELL_PVP_RESPAWN_INSTANT,
	CELL_PVP_RESPAWN_NORMAL,
};

enum cellpvp_skillonoff_flag {
	CELL_PVP_SKILL_DISALLOW,
	CELL_PVP_SKILL_ALLOW,
};

struct player_data {
	unsigned cellpvp_flag : 1;
};

struct map_cellpvp_data {
	unsigned map_cellpvp_flag : 1;
	int x1;
	int y1;
	int x2;
	int y2;
	unsigned map_cellpvp_respawn : 2;
	unsigned map_cellpvp_skillonoff : 1;
};

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
	struct map_cellpvp_data *mf = getFromMAPD(&map->list[sd->bl.m], 0);
	if (mf == NULL || mf->map_cellpvp_flag == false)
		return;
	if (sd->bl.x < mf->x1 || sd->bl.y < mf->y1 || sd->bl.x > mf->x2 || sd->bl.y > mf->y2)
		walkout_cellpvp(sd);
	else
		walkin_cellpvp(sd);
	return;
}

static int pc_calc_pvprank_sub(struct block_list *bl, va_list ap) {
	struct map_session_data *sd1 = NULL;
	struct map_session_data *sd2 = va_arg(ap,struct map_session_data *);
	int *count = va_arg(ap, int *);
	nullpo_ret(bl);
	Assert_ret(bl->type == BL_PC);
	sd1 = BL_CAST(BL_PC, bl);
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
	int count = 0;
	struct map_cellpvp_data *mf = getFromMAPD(&map->list[(*sd)->bl.m], 0);
	if (mf == NULL || mf->map_cellpvp_flag == false)
		return (*sd)->pvp_rank;
	struct map_data *m = &map->list[(*sd)->bl.m];
	int old = (*sd)->pvp_rank;
	(*sd)->pvp_rank = 1;
	map->foreachinarea(pc_calc_pvprank_sub, (*sd)->bl.m, mf->x1, mf->y1, mf->x2, mf->y2, BL_PC, *sd, &count);
	if (old != (*sd)->pvp_rank || (*sd)->pvp_lastusers != count)
		clif->pvpset(*sd, (*sd)->pvp_rank, (*sd)->pvp_lastusers = count, 0);
	hookStop();
	return (*sd)->pvp_rank;
}

static int pc_dead_post(int retVal, struct map_session_data *sd, struct block_list *src) {
	if (sd == NULL)
		return retVal;
	struct map_cellpvp_data *mf = getFromMAPD(&map->list[sd->bl.m], 0);
	if (mf == NULL || mf->map_cellpvp_flag == false)
		return retVal;
	if (mf->map_cellpvp_respawn == CELL_PVP_RESPAWN_INSTANT)
		timer->add(timer->gettick() + 1, pc->respawn_timer, sd->bl.id, 0);
	if (mf->map_cellpvp_respawn == CELL_PVP_RESPAWN_NORMAL && battle->bc->pk_mode == 1 && map->list[sd->bl.m].flag.pvp_nocalcrank == 0) {
		sd->pvp_point -= 5;
		sd->pvp_lost++;
		if (src != NULL && src->type == BL_PC) {
			struct map_session_data *ssd = BL_UCAST(BL_PC, src);
			ssd->pvp_point++;
			ssd->pvp_won++;
		}
		if (sd->pvp_point < 0) {
			timer->add(timer->gettick() + 1, pc->respawn_timer, sd->bl.id, 0);
			return 1|8;
		}
	}
	return retVal;
}

static int pc_respawn_timer_pre(int *tid, int64 *tick, int *id, intptr_t *data) {
	struct map_session_data *sd = map->id2sd(*id);
	if (sd == NULL)
		return 0;
	struct map_cellpvp_data *mf = getFromMAPD(&map->list[sd->bl.m], 0);
	if (mf == NULL || mf->map_cellpvp_flag == false)
		return 0;
	if (mf->map_cellpvp_respawn == CELL_PVP_RESPAWN_DISABLE) {
		sd->pvp_point = 0;
		hookStop();
	}
	return 0;
}

//	flush all cell_pvp data back to default upon @reloadscript
void map_flags_init_pre(void) {
	for (int i = 0; i < map->count; ++i) {
		struct map_cellpvp_data *mf = getFromMAPD(&map->list[i], 0);
		if (mf != NULL)
			removeFromMAPD(&map->list[i], 0);
	}
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
	if (s_bl->type != BL_PC || t_bl->type != BL_PC)
		return retVal;
	struct map_session_data *sd = BL_CAST(BL_PC, s_bl), *tsd = BL_CAST(BL_PC, t_bl);
	nullpo_ret(sd);
	nullpo_ret(tsd);
	if (retVal != 1)
		return retVal;
	struct map_cellpvp_data *mf = getFromMAPD(&map->list[sd->bl.m], 0);
	if (mf == NULL || mf->map_cellpvp_flag == false)
		return retVal;
	if (sd->bl.x < mf->x1 || tsd->bl.x < mf->x1 || sd->bl.y < mf->y1 || tsd->bl.y < mf->y1 || sd->bl.x > mf->x2 || tsd->bl.x > mf->x2 || sd->bl.y > mf->y2 || tsd->bl.y > mf->y2)
		return 0;
	return retVal;
}

static int skill_check_condition_castbeginend_post(int retVal, struct map_session_data *sd, uint16 skill_id, uint16 skill_lv) {
	if (sd == NULL || retVal == 0)
		return 0;
	if (((sd->auto_cast_current.itemskill_conditions_checked || !sd->auto_cast_current.itemskill_check_conditions)
	    && sd->auto_cast_current.type == AUTOCAST_ITEM) || sd->auto_cast_current.type == AUTOCAST_IMPROVISE) {
		return 1;
	}
	if (pc_has_permission(sd, PC_PERM_SKILL_UNCONDITIONAL) && sd->auto_cast_current.type != AUTOCAST_ITEM) {
		sd->state.arrow_atk = skill->get_ammotype(skill_id)? 1:0;
		sd->spiritball_old = sd->spiritball;
		return 1;
	}
	struct status_data *st = &sd->battle_status;
	struct status_change *sc = &sd->sc;
	if (!sc->count)
		sc = NULL;
	switch(skill_id) { // Turn off check.
	case BS_MAXIMIZE:
	case NV_TRICKDEAD:
	case TF_HIDING:
	case AS_CLOAKING:
	case CR_AUTOGUARD:
	case ML_AUTOGUARD:
	case CR_DEFENDER:
	case ML_DEFENDER:
	case ST_CHASEWALK:
	case PA_GOSPEL:
	case CR_SHRINK:
	case TK_RUN:
	case GS_GATLINGFEVER:
	case TK_READYCOUNTER:
	case TK_READYDOWN:
	case TK_READYSTORM:
	case TK_READYTURN:
	case SG_FUSION:
	case RA_WUGDASH:
	case KO_YAMIKUMO:
	case SU_HIDE:
		if (sc && sc->data[status->skill2sc(skill_id)])
			return 1;
	default:
		break;
	}
	struct map_cellpvp_data *mf = getFromMAPD(&map->list[sd->bl.m], 0);
	if (mf == NULL || mf->map_cellpvp_flag == false)
		return retVal;
	if (sd->bl.x < mf->x1 || sd->bl.y < mf->y1 || sd->bl.x > mf->x2 || sd->bl.y > mf->y2) {
		if (mf->map_cellpvp_skillonoff == CELL_PVP_SKILL_DISALLOW)
			return 0;
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
	struct map_cellpvp_data *mf = getFromMAPD(&map->list[sd->bl.m], 0);
	if (mf == NULL || mf->map_cellpvp_flag == false)
		return retVal;
	struct player_data *ssd = getFromMSD(sd, 0);
	nullpo_ret(ssd);
	if (sd->bl.x < mf->x1 || sd->bl.y < mf->y1 || sd->bl.x > mf->x2 || sd->bl.y > mf->y2) {
		if (ssd->cellpvp_flag == true)
			walkout_cellpvp(sd);
	}
	else {
		if (ssd->cellpvp_flag == false)
			walkin_cellpvp(sd);
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
	struct map_cellpvp_data *mf = getFromMAPD(&map->list[sd->bl.m], 0);
	if (mf == NULL || mf->map_cellpvp_flag == false)
		return retVal;
	struct player_data *ssd = getFromMSD(sd, 0);
	nullpo_ret(ssd);
	if (sd->bl.x < mf->x1 || sd->bl.y < mf->y1 || sd->bl.x > mf->x2 || sd->bl.y > mf->y2) {
		if (ssd->cellpvp_flag == true)
			walkout_cellpvp(sd);
	}
	else {
		if (ssd->cellpvp_flag == false)
			walkin_cellpvp(sd);
	}
	return retVal;
}

ACMD(cell_pvp) {
	if (map->list[sd->bl.m].flag.pvp) {
		clif->message(fd, "PvP is already On.");
		return false;
	}
	char msg[CHAT_SIZE_MAX];
	int x_1, y_1, x_2, y_2, respawn, skillonoff;
	if (sscanf(message, "%d %d %d %d %d %d", &x_1, &y_1, &x_2, &y_2, &respawn, &skillonoff) < 6) {
		clif->message(fd, "Syntax: @cell_pvp <x1> <y1> <x2> <y2> <respawn> <skillallow>");
		clif->message(fd, "    <respawn> : when player is dead -> 0 - no respawn. 1 - respawn instantly.");
		clif->message(fd, "        2 - respawn normally from counting official PvP Points. ");
		clif->message(fd, "    <skillallow> : 0 - disallow skill outside pvp zone. 1 - allow skill outside pvp zone");
		safesnprintf(msg, CHAT_SIZE_MAX, "Example: @cell_pvp %d %d %d %d 1 0", sd->bl.x, sd->bl.y, sd->bl.x +10, sd->bl.y +10);
		clif->message(fd, msg);
		clif->message(fd, "    Create a PvP zone for an Event with spectators.");
		clif->message(fd, "    When participant dead they respawn instantly.");
		clif->message(fd, "    Spectators can't interupt by casting skill outside PvP zone.");
		safesnprintf(msg, CHAT_SIZE_MAX, "Example: @cell_pvp %d %d %d %d 0 1", sd->bl.x, sd->bl.y, sd->bl.x +10, sd->bl.y +10);
		clif->message(fd, msg);
		clif->message(fd, "    Create a PvP zone just for fun or practice combat.");
		clif->message(fd, "    Useful for friendly PvP inside a custom house.");
		clif->message(fd, "    Allow interruption and doesn't annoy other players on the map.");
		return false;
	}
	if (respawn < CELL_PVP_RESPAWN_DISABLE || respawn > CELL_PVP_RESPAWN_NORMAL) {
		clif->message(fd, "<respawn> flag must within the value 0-2.");
		clif->message(fd, "Syntax: @cell_pvp <x1> <y1> <x2> <y2> <respawn> <skillallow>");
		clif->message(fd, "    <respawn> : when player is dead -> 0 - no respawn. 1 - respawn instantly.");
		clif->message(fd, "        2 - respawn normally from counting official PvP Points. ");
		return false;
	}
	if (skillonoff < CELL_PVP_SKILL_DISALLOW || skillonoff > CELL_PVP_SKILL_ALLOW) {
		clif->message(fd, "<skillallow> flag must within the value 0-1.");
		clif->message(fd, "Syntax: @cell_pvp <x1> <y1> <x2> <y2> <respawn> <skillallow>");
		clif->message(fd, "    <skillallow> : 0 - no skill allow outside pvp zone. 1 - allow skill casting outside pvp zone");
		return false;
	}
	struct map_cellpvp_data *mf = getFromMAPD(&map->list[sd->bl.m], 0);
	if (mf == NULL) {
		CREATE(mf, struct map_cellpvp_data, 1);
		mf->map_cellpvp_flag = true;
		mf->x1 = x_1;
		mf->y1 = y_1;
		mf->x2 = x_2;
		mf->y2 = y_2;
		mf->map_cellpvp_respawn = respawn;
		mf->map_cellpvp_skillonoff = skillonoff;
		addToMAPD(&map->list[sd->bl.m], mf, 0, true);
	}
	else {
		mf->map_cellpvp_flag = true;
		mf->x1 = x_1;
		mf->y1 = y_1;
		mf->x2 = x_2;
		mf->y2 = y_2;
		mf->map_cellpvp_respawn = respawn;
		mf->map_cellpvp_skillonoff = skillonoff;
	}
	map->zone_change2(sd->bl.m, strdb_get(map->zone_db, MAP_ZONE_PVP_NAME));
	map->list[sd->bl.m].flag.pvp = 1;
	map->foreachinarea(buildin_cellpvp, sd->bl.m, x_1, y_1, x_2, y_2, BL_PC, true);
	char msg_respawn[CHAT_SIZE_MAX];
	if (respawn == CELL_PVP_RESPAWN_DISABLE)
		safesnprintf(msg_respawn, CHAT_SIZE_MAX, "%s", "no respawn");
	else if (respawn == CELL_PVP_RESPAWN_INSTANT)
		safesnprintf(msg_respawn, CHAT_SIZE_MAX, "%s", "instant respawn");
	else if (respawn == CELL_PVP_RESPAWN_NORMAL)
		safesnprintf(msg_respawn, CHAT_SIZE_MAX, "%s", "normal respawn");
	char msg_skillonoff[CHAT_SIZE_MAX];
	if (skillonoff == CELL_PVP_SKILL_DISALLOW)
		safesnprintf(msg_skillonoff, CHAT_SIZE_MAX, "%s", "disallow skill");
	else if (skillonoff == CELL_PVP_SKILL_ALLOW)
		safesnprintf(msg_skillonoff, CHAT_SIZE_MAX, "%s", "allow skill");
	safesnprintf(msg, CHAT_SIZE_MAX, "%s has created a PvP Zone with %s and %s outside PvP Zone.", sd->status.name, msg_respawn, msg_skillonoff);
	clif->broadcast(&sd->bl, msg, CHAT_SIZE_MAX, BC_DEFAULT, ALL_SAMEMAP);
	return true;
}

BUILDIN(cell_pvp) {
	char mapname[MAP_NAME_LENGTH];
	if (strlen(script_getstr(st,2)) > MAP_NAME_LENGTH) {
		ShowError("buildin_cell_pvp: the map name must not longer than %d characters.\n", MAP_NAME_LENGTH);
		return false;
	}
	safestrncpy(mapname, script_getstr(st,2), MAP_NAME_LENGTH);
	int16 map_id = map->mapname2mapid(mapname);
	if (map_id < 0) {
		return false;
	}
	if (map->list[map_id].flag.pvp) {
		ShowError("buildin_cell_pvp: this map '%s' PvP is already On.\n", mapname);
		return false;
	}
	int respawn = script_getnum(st,7);
	if (respawn < CELL_PVP_RESPAWN_DISABLE || respawn > CELL_PVP_RESPAWN_NORMAL) {
		ShowError("buildin_cell_pvp: respawn flag must within constant value CELL_PVP_RESPAWN_DISABLE and CELL_PVP_RESPAWN_NORMAL.\n");
		return false;
	}
	int skillonoff = script_getnum(st,8);
	if (skillonoff < CELL_PVP_SKILL_DISALLOW || skillonoff > CELL_PVP_SKILL_ALLOW) {
		ShowError("buildin_cell_pvp: skillallow flag must within constant value CELL_PVP_SKILL_DISALLOW and CELL_PVP_SKILL_ALLOW.\n");
		return false;
	}
	struct map_cellpvp_data *mf = getFromMAPD(&map->list[map_id], 0);
	if (mf == NULL) {
		CREATE(mf, struct map_cellpvp_data, 1);
		mf->map_cellpvp_flag = true;
		mf->x1 = script_getnum(st,3);
		mf->y1 = script_getnum(st,4);
		mf->x2 = script_getnum(st,5);
		mf->y2 = script_getnum(st,6);
		mf->map_cellpvp_respawn = respawn;
		mf->map_cellpvp_skillonoff = skillonoff;
		addToMAPD(&map->list[map_id], mf, 0, true);
	}
	else {
		mf->map_cellpvp_flag = true;
		mf->x1 = script_getnum(st,3);
		mf->y1 = script_getnum(st,4);
		mf->x2 = script_getnum(st,5);
		mf->y2 = script_getnum(st,6);
		mf->map_cellpvp_respawn = respawn;
		mf->map_cellpvp_skillonoff = skillonoff;
	}
	map->zone_change2(map_id, strdb_get(map->zone_db, MAP_ZONE_PVP_NAME));
	map->list[map_id].flag.pvp = 1;
	map->foreachinarea(buildin_cellpvp, map_id, mf->x1, mf->y1, mf->x2, mf->y2, BL_PC, true);
	return true;
}

ACMD(pvpoff) {
	if (!map->list[sd->bl.m].flag.pvp) {
		clif->message(fd, msg_fd(fd,160)); // PvP is already Off.
		return false;
	}
	map->zone_change2(sd->bl.m,map->list[sd->bl.m].prev_zone);
	map->list[sd->bl.m].flag.pvp = 0;
	if (!battle->bc->pk_mode) {
		clif->map_property_mapall(sd->bl.m, MAPPROPERTY_NOTHING);
		clif->maptypeproperty2(&sd->bl,ALL_SAMEMAP);
	}
	map->foreachinmap(atcommand->pvpoff_sub,sd->bl.m, BL_PC);
	map->foreachinmap(atcommand->stopattack,sd->bl.m, BL_CHAR, 0);
	clif->message(fd, msg_fd(fd,31)); // PvP: Off.

	struct map_cellpvp_data *mf = getFromMAPD(&map->list[sd->bl.m], 0);
	if (mf != NULL && mf->map_cellpvp_flag == true) {
		clif->map_property_mapall(sd->bl.m, MAPPROPERTY_NOTHING);
		clif->maptypeproperty2(&sd->bl,ALL_SAMEMAP);
		removeFromMAPD(&map->list[sd->bl.m], 0);
	}
	return true;
}

BUILDIN(pvpoff) {
	int16 m;
	const char *str;
	struct block_list bl;

	memset(&bl, 0, sizeof(bl));
	str=script_getstr(st,2);
	m = map->mapname2mapid(str);
	if(m < 0 || !map->list[m].flag.pvp)
		return true;

	map->zone_change2(m, map->list[m].prev_zone);
	map->list[m].flag.pvp = 0;
	clif->map_property_mapall(m, MAPPROPERTY_NOTHING);
	bl.type = BL_NUL;
	bl.m = m;
	clif->maptypeproperty2(&bl,ALL_SAMEMAP);
	if (!battle->bc->pk_mode)
		map->foreachinmap(script->buildin_pvpoff_sub, m, BL_PC);

	struct map_cellpvp_data *mf = getFromMAPD(&map->list[m], 0);
	if (mf != NULL && mf->map_cellpvp_flag == true) {
		map->foreachinmap(script->buildin_pvpoff_sub, m, BL_PC);
		removeFromMAPD(&map->list[m], 0);
	}
	return true;
}

HPExport void plugin_init(void) {
	addHookPost(pc, reg_received, pc_reg_received_post);
	addHookPost(clif, pLoadEndAck, clif_parse_LoadEndAck_post);
	addHookPre(pc, calc_pvprank, pc_calc_pvprank_pre);
	addHookPost(pc, dead, pc_dead_post);
	addHookPre(pc, respawn_timer, pc_respawn_timer_pre);
	addHookPre(map, flags_init, map_flags_init_pre);
	addHookPost(battle, check_target, battle_check_target_post);
	addHookPost(skill, check_condition_castbegin, skill_check_condition_castbeginend_post);
	addHookPost(skill, check_condition_castend, skill_check_condition_castbeginend_post);
	addHookPost(unit, walk_toxy_timer, unit_walk_toxy_timer_post);
	addHookPost(unit, blown, unit_blown_post);
	addAtcommand("cell_pvp", cell_pvp);
	addScriptCommand("cell_pvp", "siiiiii", cell_pvp);

	addAtcommand("pvpoff", pvpoff); // overwrite
	addScriptCommand("pvpoff", "s", pvpoff); // overwrite

	script->set_constant("CELL_PVP_RESPAWN_DISABLE", CELL_PVP_RESPAWN_DISABLE, false, false);
	script->set_constant("CELL_PVP_RESPAWN_INSTANT", CELL_PVP_RESPAWN_INSTANT, false, false);
	script->set_constant("CELL_PVP_RESPAWN_NORMAL", CELL_PVP_RESPAWN_NORMAL, false, false);
	script->set_constant("CELL_PVP_SKILL_DISALLOW", CELL_PVP_SKILL_DISALLOW, false, false);
	script->set_constant("CELL_PVP_SKILL_ALLOW", CELL_PVP_SKILL_ALLOW, false, false);
}
