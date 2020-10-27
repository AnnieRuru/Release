//===== Hercules Plugin ======================================
//= @packetfilter
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: =====================================
//= Hercules 2020-09-24
//===== Description: =========================================
//= if every player enable this during WoE, it can reduce massive lag
//===== Topic ================================================
//= https://herc.ws/board/topic/18902-packetfilter/
//===== Additional Comments: =================================
//= everything starts small, right ? so do one step at a time
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/clif.h"
#include "common/socket.h"
#include "common/memmgr.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"packetfilter",
	SERVER_TYPE_MAP,
	"0.1",
	HPM_VERSION,
};

struct player_data {
	int packetfilter_flag;
};

static int pc_reg_received_post(int retVal, struct map_session_data *sd) {
	if (retVal == 0)
		return 0;
	struct player_data *ssd;
	CREATE(ssd, struct player_data, true);
	ssd->packetfilter_flag = 0;
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
	if (type == AREA_WOC && RBUFW(buf, 0) == 0x8d && ssd->packetfilter_flag == 1) {
		hookStop();
		return 0;
	}
	return 1;
}

ACMD(packetfilter) {
	struct player_data *ssd = getFromMSD(sd, 0);
	if (ssd->packetfilter_flag) {
		ssd->packetfilter_flag = 0;
		clif->message(sd->fd, "@packetfilter is now turn OFF.");
	}
	else {
		ssd->packetfilter_flag = 1;
		clif->message(sd->fd, "@packetfilter is now turn ON.");
	}
	return true;
}

HPExport void plugin_init (void) {
	addAtcommand( "packetfilter", packetfilter);
	addHookPost(pc, reg_received, pc_reg_received_post);
	addHookPre(clif, send_sub, clif_send_sub_pre);
}
