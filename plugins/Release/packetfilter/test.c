#include "common/hercules.h"
#include "map/clif.h"
#include "common/socket.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"test",
	SERVER_TYPE_MAP,
	"x.x",
	HPM_VERSION,
};

static int clif_send_sub_pre(struct block_list **bl, va_list ap) {
	void *buf = va_arg(ap, void*);
	int len = va_arg(ap ,int);
	struct block_list *src_bl = va_arg(ap, struct block_list*);
	if (src_bl == NULL)
		return 0;
	int type = va_arg(ap, int);
	ShowDebug("%d 0x%x\n", type, RBUFW(buf, 0));
	return 0;
}

HPExport void plugin_init(void) {
	addHookPre(clif, send_sub, clif_send_sub_pre);
}
/*
ShowDebug("02. %d %d\n", RBUFL(buf, 2), RBUFW(buf, 2));
ShowDebug("03. %d %d\n", RBUFL(buf, 3), RBUFW(buf, 3));
ShowDebug("04. %d %d\n", RBUFL(buf, 4), RBUFW(buf, 4));
ShowDebug("05. %d %d\n", RBUFL(buf, 5), RBUFW(buf, 5));
ShowDebug("06. %d %d\n", RBUFL(buf, 6), RBUFW(buf, 6));
ShowDebug("07. %d %d\n", RBUFL(buf, 7), RBUFW(buf, 7));
ShowDebug("08. %d %d\n", RBUFL(buf, 8), RBUFW(buf, 8));
ShowDebug("09. %d %d\n", RBUFL(buf, 9), RBUFW(buf, 9));
ShowDebug("10. %d %d\n", RBUFL(buf, 10), RBUFW(buf, 10));
ShowDebug("11. %d %d\n", RBUFL(buf, 11), RBUFW(buf, 11));
ShowDebug("12. %d %d\n", RBUFL(buf, 12), RBUFW(buf, 12));
ShowDebug("13. %d %d\n", RBUFL(buf, 13), RBUFW(buf, 13));
ShowDebug("14. %d %d\n", RBUFL(buf, 14), RBUFW(buf, 14));
ShowDebug("15. %d %d\n", RBUFL(buf, 15), RBUFW(buf, 15));
ShowDebug("16. %d %d\n", RBUFL(buf, 16), RBUFW(buf, 16));
ShowDebug("17. %d %d\n", RBUFL(buf, 17), RBUFW(buf, 17));
ShowDebug("18. %d %d\n", RBUFL(buf, 18), RBUFW(buf, 18));
ShowDebug("19. %d %d\n", RBUFL(buf, 19), RBUFW(buf, 19));
ShowDebug("20. %d %d\n", RBUFL(buf, 20), RBUFW(buf, 20));
*/