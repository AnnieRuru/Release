//===== Hercules Plugin ======================================
//= @filterattack
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= x.x
//===== Compatible With: =====================================
//= Hercules 2020-09-25
//===== Description: =========================================
//= block attack animations from other players
//===== Topic ================================================
//= https://herc.ws/board/topic/18902-packetfilter/
//===== Additional Comments: =================================
//= this plugin is made purposely just to answer a post on that topic
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/mob.h"
#include "map/pet.h"
#include "map/clif.h"
#include "map/elemental.h"
#include "map/mercenary.h"
#include "map/homunculus.h"
#include "common/socket.h"
#include "common/memmgr.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"filterattack",
	SERVER_TYPE_MAP,
	"x.x",
	HPM_VERSION,
};


struct player_data {
	int filterattack_flag;
};

static int pc_reg_received_post(int retVal, struct map_session_data *sd) {
	if (retVal == 0)
		return 0;
	struct player_data *ssd;
	CREATE(ssd, struct player_data, true);
	ssd->filterattack_flag = 0;
	addToMSD(sd, ssd, 0, true);
	return 1;
}

static int clif_send_sub_pre(struct block_list **bl, va_list ap) {
	struct block_list *src_bl;
	struct map_session_data *sd;
	void *buf;
	int len, type, fd;

	if (*bl == NULL)
		return 0;
	if ((*bl)->type != BL_PC)
		return 0;
	sd = BL_UCAST(BL_PC, *bl);

	fd = sd->fd;
	if (!fd || sockt->session[fd] == NULL) //Don't send to disconnected clients.
		return 0;

	buf = va_arg(ap, void*);
	len = va_arg(ap ,int);
	if ((src_bl = va_arg(ap, struct block_list*)) == NULL)
		return 0;
	type = va_arg(ap, int);

	struct player_data *ssd = getFromMSD(sd, 0);
	if (type == AREA && RBUFW(buf, 0) == 0x8C8 && ssd->filterattack_flag > 0) {
		struct block_list *abl = map->id2bl(RBUFL(buf, 2));
		struct map_session_data *asd = NULL;
		switch (abl->type) {
		case BL_PC:	{
			asd = BL_UCAST(BL_PC, abl);
			break;
		}
		case BL_PET: {
			struct pet_data *pd = BL_UCAST(BL_PET, abl);
			asd = pd->msd;
			break;
		}
		case BL_HOM: {
			struct homun_data *hd = BL_UCAST(BL_HOM, abl);
			asd = hd->master;
			break;
		}
		case BL_MER: {
			struct mercenary_data *mc = BL_UCAST(BL_MER, abl);
			asd = mc->master;
			break;
		}
		case BL_ELEM: {
			struct elemental_data *ed = BL_UCAST(BL_ELEM, abl);
			asd = ed->master;
			break;
		}
		case BL_MOB: {
			struct mob_data *md = BL_UCAST(BL_MOB, abl);
			if (md->master_id > 0)
				asd = map->id2sd(md->master_id);
			break;
		}
		default:
			return 0;
		}

		if ((ssd->filterattack_flag & 1) && asd != NULL) {
			if (sd->status.account_id == asd->status.account_id) {
				hookStop();
				return 0;
			}
		}
		if ((ssd->filterattack_flag & 2) && asd != NULL) {
			if (sd->status.account_id != asd->status.account_id && sd->status.party_id == asd->status.party_id) {
				hookStop();
				return 0;
			}
		}
		if ((ssd->filterattack_flag & 4) && asd != NULL) {
			if (sd->status.account_id != asd->status.account_id && sd->status.guild_id == asd->status.guild_id) {
				hookStop();
				return 0;
			}
		}
		if ((ssd->filterattack_flag & 8) && asd != NULL) {
			if (sd->status.party_id != asd->status.party_id && sd->status.guild_id != asd->status.guild_id) {
				hookStop();
				return 0;
			}
		}
		if (ssd->filterattack_flag & 16) {
			if (abl->type == BL_PET || abl->type == BL_HOM || abl->type == BL_MER || abl->type == BL_ELEM || (abl->type == BL_MOB && asd != NULL)) {
				hookStop();
				return 0;
			}
		}
		if ((ssd->filterattack_flag & 32) && abl->type == BL_MOB && asd == NULL) {
			hookStop();
			return 0;
		}
	}
	return 1;
}

ACMD(filterattack) {
	struct player_data *ssd = getFromMSD(sd, 0);
	int type = atoi(message);
	if (type == 0) {
		if (ssd->filterattack_flag > 0) {
			ssd->filterattack_flag = 0;
			clif->message(sd->fd, "@filterattack is now turn OFF.");
			return true;
		}
		else {
			clif->message(sd->fd, "Syntax: @filterattack <flag>.");
			clif->message(sd->fd, "flag |1 block self damage packet");
			clif->message(sd->fd, "flag |2 block party member's damage packet");
			clif->message(sd->fd, "flag |4 block guild member's damage packet");
			clif->message(sd->fd, "flag |8 block other player's(non-party/guild) damage packet");
			clif->message(sd->fd, "flag |16 block pet/homun/elemental/mercenary's damage packet");
			clif->message(sd->fd, "flag |32 block monster's damage packet");
			clif->message(sd->fd, "Example: @filterattack 24 will block non-friendly player's and monster's damage packet");
			clif->message(sd->fd, "PS: I think I'll go for P|G|S|E|A for final release, players don't like mathematics");
			return false;
		}
	}
	clif->message(sd->fd, "@filterattack will now block");
	if (type & 1)
		clif->message(sd->fd, "- self");
	if (type & 2)
		clif->message(sd->fd, "- party");
	if (type & 4)
		clif->message(sd->fd, "- guild");
	if (type & 8)
		clif->message(sd->fd, "- other players");
	if (type & 16)
		clif->message(sd->fd, "- pet/homun/elemental/mercenary");
	if (type & 32)
		clif->message(sd->fd, "- monster");
	ssd->filterattack_flag = type;
	return true;
}

HPExport void plugin_init (void) {
	addAtcommand("filterattack", filterattack);
	addHookPost(pc, reg_received, pc_reg_received_post);
	addHookPre(clif, send_sub, clif_send_sub_pre);
}
