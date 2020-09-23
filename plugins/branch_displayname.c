//===== Hercules Plugin ======================================
//= branch_displayname
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2020-09-23
//===== Description: =========================================
//= Shows player's name when summon a dead branch
//===== Topic ================================================
//= discord
//===== Additional Comments: =================================  
/*= honestly, just show the player's name on the branch, simple as that
{
	Id: 604
	AegisName: "Branch_Of_Dead_Tree"
	Name: "Dead Branch"
	Type: "IT_USABLE"
	Buy: 50
	Weight: 50
	BuyingStore: true
	Script: <" monster "this",-1,-1,strcharinfo(PC_NAME),-1,1,""; ">
},
//= I just wasted my time doing this stupid things
//==========================================================*/

#include "common/hercules.h"
#include "map/pc.h"
#include "map/mob.h"
#include "map/guild.h"
#include "common/memmgr.h"
#include "common/timer.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"branch_displayname",
	SERVER_TYPE_MAP,
	"0.1",
	HPM_VERSION,
};

static int mob_once_spawn_overload(struct map_session_data *sd, int16 m, int16 x, int16 y, const char *mobname, int class_, int amount, const char *event, unsigned int size, unsigned int ai) {
	bool no_guardian_data = false;

	if (ai > 0 && (ai & 0x200) == 0x200) {
		no_guardian_data = true;
		ai &= ~0x200;
	}

	if (m < 0 || amount <= 0)
		return 0;

	struct mob_data *md = NULL;

	for (int i = 0; i < amount; i++) {
		int mob_id = class_;

		if (mob_id < 0) {
			mob_id = mob->get_random_id(-class_ - 1, (battle->bc->random_monster_checklv == 1) ? 3 : 1,
						    (sd != NULL) ? sd->status.base_level : 255);
		}

		md = mob->once_spawn_sub((sd != NULL) ? &sd->bl : NULL, m, x, y, mobname, mob_id, event, size, ai,
					 (sd != NULL) ? sd->npc_id : 0);

		if (md == NULL)
			continue;

//		WHY THE FUCK ONLY 5 LINES EDIT NEEDS TO FUCKING OVERLOAD THIS FUNCTION !!!
		if (sd != NULL && class_ < 0) {
			char tmp[NAME_LENGTH];
			safesnprintf(tmp, NAME_LENGTH, "%s> %s", sd->status.name, md->name);
			safesnprintf(md->name, NAME_LENGTH, "%s", tmp);
		}
//		1. BECAUSE IT RUNS INSIDE A LOOP ? 2. THERE IS NO WAY TO HOOK THIS FUNCTION BECAUSE OF MOB->GET_RANDOM_ID

		if (class_ == MOBID_EMPELIUM && !no_guardian_data) {
			struct guild_castle *gc = guild->mapindex2gc(map_id2index(m));

			if (gc != NULL) {
				struct guild *g = guild->search(gc->guild_id);
				
				md->guardian_data = (struct guardian_data*)aCalloc(1, sizeof(struct guardian_data));
				md->guardian_data->castle = gc;
				md->guardian_data->number = MAX_GUARDIANS;

				if (g != NULL)
					md->guardian_data->g = g;
				else if (gc->guild_id > 0) /// Guild not yet available, retry in 5s.
					timer->add(timer->gettick() + 5000, mob->spawn_guardian_sub, md->bl.id,
						   gc->guild_id);
			}
		}

		mob->spawn(md);

		if (class_ < 0 && battle->bc->dead_branch_active == 1) {
			/// Behold Aegis' masterful decisions yet again...
			/// "I understand the "Aggressive" part, but the "Can Move" and "Can Attack" is just stupid" [Poki]
			sc_start4(NULL, &md->bl, SC_MODECHANGE, 100, 1, 0, MD_AGGRESSIVE|MD_CANATTACK|MD_CANMOVE|MD_ANGRY,
				  0, 60000);
		}
	}

	return (md != NULL) ? md->bl.id : 0; /// ID of last spawned mob.
}

HPExport void plugin_init (void) {
	mob->once_spawn = &mob_once_spawn_overload;
}
