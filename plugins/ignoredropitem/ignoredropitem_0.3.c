//===== Hercules Plugin ======================================
//= @ignoredropitem
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.3
//===== Compatible With: ===================================== 
//= Hercules 2020-09-19
//===== Description: =========================================
//= a gm command to ignore items drop by players ... or *makeitem script command
//===== Topic ================================================
//= https://herc.ws/board/topic/18873-urgent-request-ignoredropseeitems/
//===== Additional Comments: =================================  
//= yeah, windows compiler sucks
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "common/memmgr.h"
#include "common/nullpo.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"ignoredropitem",
	SERVER_TYPE_MAP,
	"0.3",
	HPM_VERSION,
};

struct player_data {
	bool ignoredropitemflag;
};

static inline int itemtype(int type)
{
	switch( type ) {
#if PACKETVER >= 20080827
		case IT_WEAPON:
			return IT_ARMOR;
		case IT_ARMOR:
		case IT_PETARMOR:
#endif
		case IT_PETEGG:
			return IT_WEAPON;
		default:
			return type;
	}
}

int buildin_dropflooritem_flagcheck(struct block_list *bl, va_list ap) {
	struct map_session_data *sd = BL_CAST(BL_PC, bl);
	if (sd == NULL)
		return;
	struct player_data *ssd = getFromMSD(sd, 0);
	struct flooritem_data *fitem = va_arg(ap, struct flooritem_data *);
	
	struct packet_dropflooritem p;
	int view;

	p.PacketType = dropflooritemType;
	p.ITAID = fitem->bl.id;
	p.ITID = ((view = itemdb_viewid(fitem->item_data.nameid)) > 0) ? view : fitem->item_data.nameid;
#if PACKETVER >= 20130000 /* not sure date */
	p.type = itemtype(itemdb_type(fitem->item_data.nameid));
#endif
	p.IsIdentified = fitem->item_data.identify ? 1 : 0;
	p.xPos = fitem->bl.x;
	p.yPos = fitem->bl.y;
	p.subX = fitem->subx;
	p.subY = fitem->suby;
	p.count = fitem->item_data.amount;
#if defined(PACKETVER_ZERO) || PACKETVER >= 20180418
	if (fitem->showdropeffect) {
		p.showdropeffect = itemdb_showdropeffect(fitem->item_data.nameid);
		p.dropeffectmode = itemdb_dropeffectmode(fitem->item_data.nameid);
	} else {
		p.showdropeffect = 0;
		p.dropeffectmode = 0;
	}
#endif

	if (ssd->ignoredropitemflag == 0)
		clif->send(&p, sizeof(p), &sd->bl, SELF);
	return true;
}

static void clif_dropflooritem_overload(struct flooritem_data *fitem) {
	nullpo_retv(fitem);
	if (fitem->item_data.nameid <= 0)
		return;
	map->foreachinrange(buildin_dropflooritem_flagcheck, &fitem->bl, battle->bc->area_size, BL_PC, fitem);
}

static void clif_getareachar_item_pre(struct map_session_data **sd, struct flooritem_data **fitem) {
	struct player_data *ssd = getFromMSD(*sd, 0);
	if (ssd->ignoredropitemflag == 1)
		hookStop();
	return;
}

bool pc_authok_pre( struct map_session_data **sd, int *login_id2, time_t *expiration_time, int *group_id, const struct mmo_charstatus **st, bool *changing_mapservers ) {
	struct player_data *ssd;
	CREATE( ssd, struct player_data, true );
	ssd->ignoredropitemflag = 0;
	addToMSD( *sd, ssd, 0, true );
	return 0;
}

ACMD(ignoredropitem) {
	struct player_data *ssd = getFromMSD(sd, 0);
	if (ssd->ignoredropitemflag) {
		ssd->ignoredropitemflag = 0;
		clif->message(sd->fd, "@ignoredropitem is now turn OFF.");
	}
	else {
		ssd->ignoredropitemflag = 1;
		clif->message(sd->fd, "@ignoredropitem is now turn ON.");
	}
	clif->refresh(sd);
	return true;
}

HPExport void plugin_init(void) {
	addHookPre(pc, authok, pc_authok_pre);
	addAtcommand("ignoredropitem", ignoredropitem);
	clif->dropflooritem = &clif_dropflooritem_overload;
	addHookPre(clif, getareachar_item, clif_getareachar_item_pre);
}
