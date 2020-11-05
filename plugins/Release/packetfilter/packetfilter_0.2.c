//===== Hercules Plugin ======================================
//= @packetfilter
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.2
//===== Compatible With: =====================================
//= Hercules 2020-11-05
//===== Description: =========================================
//= if every player enable this during WoE, it can reduce massive lag
//===== Topic ================================================
//= https://herc.ws/board/topic/18902-packetfilter/
//===== Additional Comments: =================================
//= @packetfilter C E I A B T G S
//=   C = ignore normal Chat, including Dancer's scream, Bard's frost joke, and pet's talk (pet start to talk when intimacy > 900)
//=   E = ignore emotion, including monster and pet emotion
//=   I = ignore Item usage animation
//=   A = ignore normal Attack animation (receiving attack)
//=   B = Buffing skill animations 
//=   T = offensive single Target skill/spell animations
//=   G = Ground based skills (eg:storm gust)
//=   S = Songs from Bard/Dancer
//= after the Main type, also support additional flags
//=   S = Self
//=   P = Party
//=   G = Guild
//=   B = Battleground 
//=   O = Other players, none of the above
//=   H = block pet/homunculus/elementals/mercenary and player's @summon
//=   M = block monsters type
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
#include "common/nullpo.h"
#include "common/cbasetypes.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"packetfilter",
	SERVER_TYPE_MAP,
	"0.2",
	HPM_VERSION,
};

enum filter_flag {
	FILTER_SELF  = 1,
	FILTER_PARTY = 2,
	FILTER_GUILD = 4,
	FILTER_BG    = 8,
	FILTER_OTHER = 16,
	FILTER_PET   = 32,
	FILTER_MOB   = 64,
};

struct player_data {
	unsigned block_ : 1;
	int block_chat;
	int block_emotion;
//	int block_item;
//	int block_attack;
//	int block_buff;
//	int block_target_spell;
//	int block_aoe_spell;
//	int block_songs;
};

static int pc_reg_received_post(int retVal, struct map_session_data *sd) {
	if (retVal == 0)
		return 0;
	struct player_data *ssd;
	CREATE(ssd, struct player_data, true);
	ssd->block_ = 0;
	ssd->block_chat = 0;
	ssd->block_emotion = 0;
//	ssd->block_item = 0;
//	ssd->block_attack = 0;
//	ssd->block_buff = 0;
//	ssd->block_target_spell = 0;
//	ssd->block_aoe_spell = 0;
//	ssd->block_songs = 0;
	addToMSD(sd, ssd, 0, true);
	return 1;
}

static struct map_session_data *target_sd(struct block_list *bl) {
	nullpo_retr(NULL, bl);
	struct map_session_data *sd = NULL;
	switch (bl->type) {
	case BL_PC:	{
		sd = BL_UCAST(BL_PC, bl);
		break;
	}
	case BL_PET: {
		struct pet_data *pd = BL_UCAST(BL_PET, bl);
		sd = pd->msd;
		break;
	}
	case BL_HOM: {
		struct homun_data *hd = BL_UCAST(BL_HOM, bl);
		sd = hd->master;
		break;
	}
	case BL_MER: {
		struct mercenary_data *mc = BL_UCAST(BL_MER, bl);
		sd = mc->master;
		break;
	}
	case BL_ELEM: {
		struct elemental_data *ed = BL_UCAST(BL_ELEM, bl);
		sd = ed->master;
		break;
	}
	case BL_MOB: {
		struct mob_data *md = BL_UCAST(BL_MOB, bl);
		if (md->master_id > 0) {
			struct block_list *mbl = map->id2bl(md->master_id);
			if (mbl->type == BL_PC)
				sd = map->id2sd(md->master_id);
			else
				sd = NULL;
		}
		else
			sd = NULL;
		break;
	}
	default:
		sd = NULL;
	}
	return sd;
}

static bool check_filter(int flag, struct map_session_data *sd, struct block_list *tbl, struct map_session_data *tsd) {
	nullpo_retr(false, sd);
	nullpo_retr(false, tbl);
	if ((flag & FILTER_SELF) != 0 && tsd != NULL) {
		if (sd->status.account_id == tsd->status.account_id)
			return true;
	}
	if ((flag & FILTER_PARTY) != 0 && sd->status.party_id > 0 && tsd != NULL) {
		if (sd->status.account_id != tsd->status.account_id && sd->status.party_id == tsd->status.party_id)
			return true;
	}
	if ((flag & FILTER_GUILD) != 0 && sd->status.guild_id > 0 && tsd != NULL) {
		if (sd->status.account_id != tsd->status.account_id && sd->status.guild_id == tsd->status.guild_id)
			return true;
	}
	if ((flag & FILTER_BG) != 0 && sd->bg_id > 0 && tsd != NULL) {
		if (sd->status.account_id != tsd->status.account_id && sd->bg_id == tsd->bg_id)
			return true;
	}
	if ((flag & FILTER_OTHER) != 0 && tsd != NULL) {
		if (sd->bg_id > 0 && sd->bg_id != tsd->bg_id) // since you have to be on a battleground map to have a battleground ID anyway
			return true;
		if (sd->status.party_id != tsd->status.party_id && sd->status.guild_id != tsd->status.guild_id)
			return true;
	}
	if ((flag & FILTER_PET) != 0) {
		if (tbl->type == BL_PET || tbl->type == BL_HOM || tbl->type == BL_MER || tbl->type == BL_ELEM || (tbl->type == BL_MOB && tsd != NULL))
			return true;
	}
	if ((flag & FILTER_MOB) != 0 && tbl->type == BL_MOB && tsd == NULL)
		return true;
	return false;
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
	struct player_data *ssd = getFromMSD(sd, 0);
	nullpo_ret(ssd);
	if (ssd->block_ == 0)
		return 0;

	fd = sd->fd;
	if (fd == 0 || sockt->session[fd] == NULL) //Don't send to disconnected clients.
		return 0;

	buf = va_arg(ap, void*);
	len = va_arg(ap ,int);
	if ((src_bl = va_arg(ap, struct block_list*)) == NULL)
		return len; // some compiler throw stupid error, so let's just do that
	type = va_arg(ap, int);

	if (ssd->block_chat > 0 && type == AREA_WOC && RBUFW(buf, 0) == 0x8d) {
		struct block_list *tbl = map->id2bl(RBUFL(buf, 4));
		struct map_session_data *tsd = target_sd(tbl);
		if (check_filter(ssd->block_chat, sd, tbl, tsd))
			hookStop();
	}
	else if (ssd->block_emotion > 0 && type == AREA && RBUFW(buf, 0) == 0xc0) {
		struct block_list *tbl = map->id2bl(RBUFL(buf, 2));
		struct map_session_data *tsd = target_sd(tbl);
		if (check_filter(ssd->block_emotion, sd, tbl, tsd))
			hookStop();
	}
	return 0;
}

int set_flag(int fd, char *tmp) {
	char msg[CHAT_SIZE_MAX];
	size_t l = strlen(tmp);
	unsigned short k = 1;
	int flag = 0;
	char type[2];
	type[0] = TOUPPER(tmp[0]);
	type[1] = '\0';
	if (l == 1) {
		safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in '%s' type. Must have additional fields.", type);
		clif->message(fd, msg);
		return 0;
	}
	if (l > 8) {
		safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in '%s' type. There are too many types!", type);
		clif->message(fd, msg);
		return 0;
	}
	for (k = 1; k < l; k++) {
		switch(tmp[k]) {
		case 'S': case 's':
			if ((flag & FILTER_SELF) != 0) {
				safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in '%s' type. [S]elf flag has been set.", type);
				clif->message(fd, msg);
				return 0;
			}
			flag |= FILTER_SELF;
			break;
		case 'P': case 'p':
			if ((flag & FILTER_PARTY) != 0) {
				safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in '%s' type. [P]arty flag has been set.", type);
				clif->message(fd, msg);
				return 0;
			}
			flag |= FILTER_PARTY;
			break;
		case 'G': case 'g':
			if ((flag & FILTER_GUILD) != 0) {
				safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in '%s' type. [G]uild flag has been set.", type);
				clif->message(fd, msg);
				return 0;
			}
			flag |= FILTER_GUILD;
			break;
		case 'B': case 'b':
			if ((flag & FILTER_BG) != 0) {
				safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in '%s' type. [B]attleground flag has been set.", type);
				clif->message(fd, msg);
				return 0;
			}
			flag |= FILTER_BG;
			break;
		case 'O': case 'o':
			if ((flag & FILTER_OTHER) != 0) {
				safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in '%s' type. [O]thers flag has been set.", type);
				clif->message(fd, msg);
				return 0;
			}
			flag |= FILTER_OTHER;
			break;
		case 'H': case 'h':
			if ((flag & FILTER_PET) != 0) {
				safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in '%s' type. [H]omunculus/Pets/Mercenary/Elemental flag has been set.", type);
				clif->message(fd, msg);
				return 0;
			}
			flag |= FILTER_PET;
			break;
		case 'M': case 'm':
			if ((flag & FILTER_MOB) != 0) {
				safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in '%s' type. [M]onster flag has been set.", type);
				clif->message(fd, msg);
				return 0;
			}
			flag |= FILTER_MOB;
			break;
		default:
			char subtype[2];
			subtype[0] = TOUPPER(tmp[0]);
			subtype[1] = '\0';
			safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in '%s' type. Unknown type '%s'.", type, subtype);
			clif->message(fd, msg);
			return 0;
		}
	}
	return flag;
}

void print_packetfilter_sub(int fd, int flag, const char *message) {
	char msg[CHAT_SIZE_MAX];
	clif->message(fd, message);
	if ((flag & FILTER_SELF) != 0) {
		safesnprintf(msg, CHAT_SIZE_MAX, "    -> Self");
		clif->message(fd, msg);
	}
	if ((flag & FILTER_PARTY) != 0) {
		safesnprintf(msg, CHAT_SIZE_MAX, "    -> Party");
		clif->message(fd, msg);
	}
	if ((flag & FILTER_GUILD) != 0) {
		safesnprintf(msg, CHAT_SIZE_MAX, "    -> Guild");
		clif->message(fd, msg);
	}
	if ((flag & FILTER_BG) != 0) {
		safesnprintf(msg, CHAT_SIZE_MAX, "    -> Battleground");
		clif->message(fd, msg);
	}
	if ((flag & FILTER_OTHER) != 0) {
		safesnprintf(msg, CHAT_SIZE_MAX, "    -> Other players");
		clif->message(fd, msg);
	}
	if ((flag & FILTER_PET) != 0) {
		safesnprintf(msg, CHAT_SIZE_MAX, "    -> Homunculus/Pets/Mercenary/Elemental");
		clif->message(fd, msg);
	}
	if ((flag & FILTER_MOB) != 0) {
		safesnprintf(msg, CHAT_SIZE_MAX, "    -> Monster");
		clif->message(fd, msg);
	}
	return;
}

void print_packetfilter(int fd, struct player_data *ssd) {
	char msg[CHAT_SIZE_MAX];
	if (ssd->block_chat != 0) {
		safesnprintf(msg, CHAT_SIZE_MAX, "Enabled Chat filter.");
		print_packetfilter_sub(fd, ssd->block_chat, msg);
	}
	if (ssd->block_emotion != 0) {
		safesnprintf(msg, CHAT_SIZE_MAX, "Enabled Emotion filter.");
		print_packetfilter_sub(fd, ssd->block_emotion, msg);
	}
	return;
}

void print_packetfilter_guide(int fd) {
	clif->message(fd, " ==== @packetfilter ===");
	clif->message(fd, "Main types are:");
	clif->message(fd, "    'C' -> block normal chat. Include dancer's scream and bard's frost joke.");
	clif->message(fd, "    'E' -> block emotions.");
	clif->message(fd, "Sub types are:");
	clif->message(fd, "    'S' -> Self");
	clif->message(fd, "    'P' -> Party");
	clif->message(fd, "    'G' -> Guild");
	clif->message(fd, "    'B' -> Battleground");
	clif->message(fd, "    'O' -> Other players");
	clif->message(fd, "    'H' -> Homunculus/Pets/Mercenary/Elemental");
	clif->message(fd, "    'M' -> Monsters");
	clif->message(fd, "Example: @packetfilter COHM EOHM");
	clif->message(fd, "    -> block normal chat and emotion from non-related players, pets, homunculus and monsters.");
	clif->message(fd, "         this doesn't block normal talk from yourself or party/guild members.");
	return;
}

ACMD(packetfilter) {
	char msg[CHAT_SIZE_MAX];
	struct player_data *ssd = getFromMSD(sd, 0);
	nullpo_ret(ssd);
	if (strlen(message) == 0 || !*message) {
		if (ssd->block_ == 0)
			print_packetfilter_guide(fd);
		else
			print_packetfilter(fd, ssd);
		return true;
	}
	if (stristr(message, "off") && stristr("off", message)) {
		ssd->block_ = 0;
		ssd->block_chat = 0;
		safesnprintf(msg, CHAT_SIZE_MAX, "@packetfilter has turn OFF.");
		clif->message(fd, msg);
		return true;
	}
	size_t l = strlen(message);
	char *tmp = (char*)aMalloc(l +1);
	unsigned short i = 0, j = 0;
	int chat_flag = 0;
	int emotion_flag = 0;
	while (i <= l) {
		if (message[i] != ' ' && message[i] != '\0')
			tmp[j++] = message[i];
		else if (message[i-1] != ' ') {
			tmp[j] = '\0';
			switch(tmp[0]) {
			case 'C': case 'c':
				chat_flag = set_flag(fd, tmp);
				if (chat_flag == 0) {
					aFree(tmp);
					return false;
				}
				break;
			case 'E': case 'e':
				emotion_flag = set_flag(fd, tmp);
				if (emotion_flag == 0) {
					aFree(tmp);
					return false;
				}
				break;
			default:
				char type[2];
				type[0] = tmp[0];
				type[1] = '\0';
				safesnprintf(msg, CHAT_SIZE_MAX, "Unknown type '%s'.", type);
				clif->message(fd, msg);
				aFree(tmp);
				return false;
			}
			j = 0;
		}
		i++;
	}
	ssd->block_ = 1;
	ssd->block_chat = chat_flag;
	ssd->block_emotion = emotion_flag;
	print_packetfilter(fd, ssd);
	aFree(tmp);
	return true;
}

HPExport void plugin_init (void) {
	addAtcommand("packetfilter", packetfilter);
	addHookPost(pc, reg_received, pc_reg_received_post);
	addHookPre(clif, send_sub, clif_send_sub_pre);
}
