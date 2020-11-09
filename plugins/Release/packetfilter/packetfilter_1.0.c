//===== Hercules Plugin ======================================
//= @packetfilter
//===== By: ==================================================
//= Original Idea from eAmod
//= AnnieRuru
//===== Current Version: =====================================
//= 1.0a
//===== Compatible With: =====================================
//= Hercules 2020-11-09
//===== Description: =========================================
//= if every player enable this during WoE, it can reduce massive lag
//===== Topic ================================================
//= https://herc.ws/board/topic/18902-packetfilter/
//===== Additional Comments: =================================
//= @packetfilter C E I A B S T G M D
//=   C = ignore normal Chat, including Dancer's scream, Bard's frost joke, and pet's talk (pet start to talk when intimacy > 900)
//=   E = ignore emotion, including monster and pet emotion (but NOT pet performance)
//=   I = ignore Item use effect
//=   A = ignore normal Attack animation (based on receiving side), your own attack animation are always shown
//=   B = Buffing skill animations (based on receiving side), your own casting animation are always shown
//=   S = Status effect
//=   T = offensive single Target skill/spell animations (based on receiving side), your own casting animation are always shown
//=   G = Ground based skills (eg:storm gust), your own casting animation are always shown
//=   M = Music dance/songs from Bard/Dancer
//=   D = Direction. Recommend filter on Party/Guild type and not enemies. Highly Recommend for supportive type Class. In case you don't know, the server send direction packet every attack and every skill use, this isn't just filter Shift-click...
//= after the Main type, also support additional flags
//=   S = Self
//=   P = Party
//=   G = Guild
//=   B = Battleground 
//=   C = Clan
//=   O = Other players, none of the above
//=   H = Homunculus/pets/elementals/mercenary and player's @summon
//=   M = Monsters
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/mob.h"
#include "map/pet.h"
#include "map/clif.h"
#include "map/atcommand.h"
#include "map/elemental.h"
#include "map/mercenary.h"
#include "map/homunculus.h"
#include "map/packets_struct.h"
#include "common/mmo.h"
#include "common/memmgr.h"
#include "common/nullpo.h"
#include "common/socket.h"
#include "common/strlib.h"
#include "common/cbasetypes.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"packetfilter",
	SERVER_TYPE_MAP,
	"1.0",
	HPM_VERSION,
};

enum filter_flag {
	FILTER_SELF  = 1,
	FILTER_PARTY = 2,
	FILTER_GUILD = 4,
	FILTER_BG    = 8,
	FILTER_CLAN  = 16,
	FILTER_OTHER = 32,
	FILTER_HOMUN = 64,
	FILTER_MOB   = 128,
};

struct player_data {
	unsigned block_ : 1;
	int block_chat;
	int block_emotion;
	int block_item;
	int block_attack;
	int block_buff;
	int block_status;
	int block_target_spell;
	int block_aoe_spell;
	int block_music;
	int block_direction;
};

static int pc_reg_received_post(int retVal, struct map_session_data *sd) {
	if (retVal == 0)
		return 0;
	struct player_data *ssd;
	CREATE(ssd, struct player_data, true);
	ssd->block_ = 0;
	ssd->block_chat = 0;
	ssd->block_emotion = 0;
	ssd->block_item = 0;
	ssd->block_attack = 0;
	ssd->block_buff = 0;
	ssd->block_status = 0;
	ssd->block_target_spell = 0;
	ssd->block_aoe_spell = 0;
	ssd->block_music = 0;
	ssd->block_direction = 0;
	addToMSD(sd, ssd, 0, true);
	return 1;
}

static struct map_session_data *target_sd(struct block_list *bl) {
	nullpo_retr(NULL, bl);
	if ((bl->type & (BL_PC|BL_PET|BL_HOM|BL_MER|BL_ELEM|BL_MOB)) == 0)
		return NULL;
	struct map_session_data *sd = NULL;
	switch (bl->type) {
	case BL_PC: {
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
	if ((tbl->type & (BL_PC|BL_PET|BL_HOM|BL_MER|BL_ELEM|BL_MOB)) == 0)
		return true;
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
	if ((flag & FILTER_CLAN) != 0 && sd->status.clan_id > 0 && tsd != NULL) {
		if (sd->status.account_id != tsd->status.account_id && sd->status.clan_id == tsd->status.clan_id)
			return true;
	}
	if ((flag & FILTER_OTHER) != 0 && tsd != NULL) {
		if (sd->bg_id > 0 && sd->bg_id != tsd->bg_id) // when you are on a Battleground team, you have to stay on the map anyway... and your guildmates can also register on the other BG team
			return true;
		if ((sd->status.party_id == 0 || (sd->status.party_id > 0 && sd->status.party_id != tsd->status.party_id)) &&
			(sd->status.guild_id == 0 || (sd->status.guild_id > 0 && sd->status.guild_id != tsd->status.guild_id)) &&
			(sd->status.clan_id == 0 || (sd->status.clan_id > 0 && sd->status.clan_id != tsd->status.clan_id)))
			return true;
	}
	if ((flag & FILTER_HOMUN) != 0) {
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

	if (*bl == NULL)
		return 0;
	if ((*bl)->type != BL_PC)
		return 0;
	sd = BL_UCAST(BL_PC, *bl);
	struct player_data *ssd = getFromMSD(sd, 0);
	nullpo_ret(ssd);
	if (ssd->block_ == 0)
		return 0;

	if (sd->fd == 0 || sockt->session[sd->fd] == NULL) //Don't send to disconnected clients.
		return 0;

	buf = va_arg(ap, void*);
	va_arg(ap ,int);
	if ((src_bl = va_arg(ap, struct block_list*)) == NULL)
		return 0;
	va_arg(ap, int);

	switch(RBUFW(buf, 0)) {
	case 0x8d: // clif_GlobalMessage
		if (ssd->block_chat > 0) {
			struct block_list *tbl = map->id2bl(RBUFL(buf, 4));
			struct map_session_data *tsd = target_sd(tbl);
			if (check_filter(ssd->block_chat, sd, tbl, tsd))
				hookStop();
		}
		break;
	case 0xc0: // clif_emotion
		if (ssd->block_emotion > 0) {
			struct block_list *tbl = map->id2bl(RBUFL(buf, 2));
			struct map_session_data *tsd = target_sd(tbl);
			if (check_filter(ssd->block_emotion, sd, tbl, tsd))
				hookStop();
		}
		break;
	case 0x1c8: // clif_useitemack
		if (ssd->block_item > 0) {
#if PACKETVER_MAIN_NUM >= 20181121 || PACKETVER_RE_NUM >= 20180704 || PACKETVER_ZERO_NUM >= 20181114
		//	ShowDebug("useItemAckType ItemID: %d | src: %d\n", RBUFL(buf, 4), RBUFL(buf, 8));
			struct block_list *tbl = map->id2bl(RBUFL(buf, 8));
#else // nobody uses below PACKETVER 3 right ? RIGHT ???
		//	ShowDebug("useItemAckType ItemID: %d | src: %d\n", RBUFW(buf, 4), RBUFL(buf, 6));
			struct block_list *tbl = map->id2bl(RBUFL(buf, 6));
#endif
			struct map_session_data *tsd = target_sd(tbl);
			if (check_filter(ssd->block_item, sd, tbl, tsd))
				hookStop();
		}
		break;
	case damageType: // clif_damage & clif_delay_damage ... it seems the src and target having at the same packet placement
		if (ssd->block_attack > 0) {
			if (RBUFL(buf, 2) == sd->status.account_id) // always show self attack
				return 0;
			struct block_list *tbl = map->id2bl(RBUFL(buf, 6));
			struct map_session_data *tsd = target_sd(tbl);
			if (check_filter(ssd->block_attack, sd, tbl, tsd))
				hookStop();
		}
		break;
	case HEADER_ZC_USESKILL_ACK: // clif_useskill ... it seems the src and target having at the same packet placement
	//	ShowDebug("HEADER_ZC_USESKILL_ACK 0xb1a src: %d | target: %d | skill ID: %d\n", RBUFL(buf, 2), RBUFL(buf, 6), RBUFW(buf, 14));
		if (ssd->block_buff > 0 || ssd->block_target_spell > 0 || ssd->block_aoe_spell > 0) {
			if (RBUFL(buf, 2) == sd->status.account_id) // always show self casting animation
				return 0;
			struct block_list *tbl = map->id2bl(RBUFL(buf, ((skill->get_inf(RBUFW(buf, 14)) & INF_GROUND_SKILL) != 0)? 2:6)); // if its ground skill, use src, else base on target
			struct map_session_data *tsd = target_sd(tbl);
			if (ssd->block_buff > 0 && (skill->get_inf(RBUFW(buf, 14)) & (INF_SUPPORT_SKILL|INF_SELF_SKILL)) != 0 && check_filter(ssd->block_buff, sd, tbl, tsd))
				hookStop();
			else if (ssd->block_target_spell > 0 && (skill->get_inf(RBUFW(buf, 14)) & INF_ATTACK_SKILL) != 0 && check_filter(ssd->block_target_spell, sd, tbl, tsd))
				hookStop();
			else if (ssd->block_aoe_spell > 0 && (skill->get_inf(RBUFW(buf, 14)) & INF_GROUND_SKILL) != 0 && check_filter(ssd->block_aoe_spell, sd, tbl, tsd))
				hookStop();
		}
		break;
	case 0x9cb: // clif_skill_nodamage
	//	ShowDebug("clif_skill_nodamage skill ID: %d | target: %d | src: %d\n", RBUFW(buf, 2), RBUFL(buf, 8), RBUFL(buf, 12));
		if (ssd->block_buff > 0 && (skill->get_inf(RBUFW(buf, 2)) & (INF_SUPPORT_SKILL|INF_SELF_SKILL)) != 0 && (skill->get_inf2(RBUFW(buf, 2)) & (INF2_SONG_DANCE|INF2_ENSEMBLE_SKILL)) == 0) {
			if (RBUFL(buf, 12) == sd->status.account_id) // always show self buffing animation
				return 0;
			struct block_list *tbl = map->id2bl(RBUFL(buf, 8)); // ... hopefully nobody complain about this
			struct map_session_data *tsd = target_sd(tbl);
			if (check_filter(ssd->block_buff, sd, tbl, tsd))
				hookStop();
		}
		else if (ssd->block_target_spell > 0 && (skill->get_inf(RBUFW(buf, 2)) & INF_ATTACK_SKILL) != 0) {
			if (RBUFL(buf, 12) == sd->status.account_id) // always show spell attack animation
				return 0;
			struct block_list *tbl = map->id2bl(RBUFL(buf, 8)); // ... hopefully nobody complain about this
			struct map_session_data *tsd = target_sd(tbl);
			if (check_filter(ssd->block_target_spell, sd, tbl, tsd))
				hookStop();
		}
		else if (ssd->block_music > 0 && (skill->get_inf2(RBUFW(buf, 2)) & (INF2_SONG_DANCE|INF2_ENSEMBLE_SKILL)) != 0) {
			struct block_list *tbl = map->id2bl(RBUFL(buf, 12)); // ... hopefully nobody complain about this
			struct map_session_data *tsd = target_sd(tbl);
			if (check_filter(ssd->block_music, sd, tbl, tsd))
				hookStop();
		}
		break;
	case 0x1de: // clif_skill_damage
	//	ShowDebug("clif_skill_damage skill ID: %d | target: %d | src: %d\n", RBUFW(buf, 2), RBUFL(buf, 8), RBUFL(buf, 4));
		if (ssd->block_target_spell > 0 && (skill->get_inf(RBUFW(buf, 2)) & INF_ATTACK_SKILL) != 0) {
			if (RBUFL(buf, 4) == sd->status.account_id) // always show spell attack animation
				return 0;
			struct block_list *tbl = map->id2bl(RBUFL(buf, 8)); // ... hopefully nobody complain about this
			struct map_session_data *tsd = target_sd(tbl);
			if (check_filter(ssd->block_target_spell, sd, tbl, tsd))
				hookStop();
		}
		break;
	case 0x117: // clif_skill_poseffect
	//	ShowDebug("clif_skill_poseffect skill ID: %d | src: %d\n", RBUFW(buf, 2), RBUFL(buf, 4));
		if (ssd->block_aoe_spell > 0) { 
			if (RBUFL(buf, 4) == sd->status.account_id) // always show spell attack animation
				return 0;
			struct block_list *tbl = map->id2bl(RBUFL(buf, 4));
			struct map_session_data *tsd = target_sd(tbl);
			if (check_filter(ssd->block_aoe_spell, sd, tbl, tsd))
				hookStop();
		}
		break;
	case 0x9c: // clif_changed_dir
	//	ShowDebug("clif_changed_dir src: %d\n", RBUFL(buf, 2));
		if (ssd->block_direction > 0) { 
			struct block_list *tbl = map->id2bl(RBUFL(buf, 2));
			struct map_session_data *tsd = target_sd(tbl);
			if (check_filter(ssd->block_direction, sd, tbl, tsd))
				hookStop();
		}
		break;
	case 0x28a: // clif_changeoption2
		if (ssd->block_status > 0) {
			struct block_list *tbl = map->id2bl(RBUFL(buf, 2));
			struct map_session_data *tsd = target_sd(tbl);
			if (check_filter(ssd->block_status, sd, tbl, tsd))
				hookStop();
		}
		break;
	case 0x983: // clif_status_change_sub ... nobody uses client before 2009 right ?
		if (ssd->block_status > 0) {
			struct block_list *tbl = map->id2bl(RBUFL(buf, 4));
			struct map_session_data *tsd = target_sd(tbl);
			if (check_filter(ssd->block_status, sd, tbl, tsd))
				hookStop();
		}
		break;
//	case idle_unitType: // clif_set_unit_idle ... 0x9ff ? <-- doesn't run clif_send_sub because it is using SELF flag
//		ShowDebug("idle_unitType 0x%x", RBUFW(buf, 0));
//		if (ssd->block_status > 0) {
//			struct packet_idle_unit *p = (struct packet_idle_unit *)idle_unitType;
//			p->virtue = 0;
//			clif->send(&p, sizeof(p), src_bl, SELF);
//			hookStop();
//		}
//		break;
//	case unit_walkingType: // clif_set_unit_walking ... 0x9fd ? <-- doesn't run clif_send_sub because it is using SELF flag
//		ShowDebug("unit_walkingType 0x%x", RBUFW(buf, 0));
//		if (ssd->block_status > 0) {
//			struct packet_unit_walking *p = (struct packet_unit_walking *)unit_walkingType;
//			p->virtue = 0;
//			clif->send(&p, sizeof(p), src_bl, SELF);
//			hookStop();
//		}
//		break;
//	case spawn_unitType: // clif_spawn_unit 0x9fe <-- can run when using @go/@warp
//		ShowDebug("spawn_unitType 0x%x | src AID: %d | src CID %d | job %d | LOOK_HAIR %d", RBUFW(buf,0), RBUFL(buf,5), RBUFL(buf,9), RBUFW(buf,23), RBUFW(buf,25));
//		if (ssd->block_status > 0) {
//			struct packet_spawn_unit *p = (struct packet_spawn_unit*)RBUFP(buf, 0);
//			p->virtue = 0;
//			clif->send(&p, sizeof(p), src_bl, SELF);
//			hookStop();
//		}
//		break;
//	case 0x229: // ????? UNKNOWN TERRITORY !!!! Don't enable this, this is for OPT1 & OPT2. Enable this will cause players can see hiding/cloacking characters !!
//		if (ssd->block_status > 0) {
//			ShowDebug("src: %d | option1 %d | option2 %d\n", RBUFL(buf, 2), RBUFW(buf, 6), RBUFW(buf, 8));
//			struct block_list *tbl = map->id2bl(RBUFL(buf, 2));
//			struct map_session_data *tsd = target_sd(tbl);
//			if (check_filter(ssd->block_status, sd, tbl, tsd))
//				hookStop();
//		}
//		break;
	default:
		break;
	}
	return 0;
}

int set_flag(int fd, char *tmp) {
	char msg[CHAT_SIZE_MAX];
	size_t l = strlen(tmp);
	unsigned short i = 1;
	int flag = 0;
	char type[2];
	type[0] = TOUPPER(tmp[0]);
	type[1] = '\0';
	if (l == 1) {
		safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in [%s] type. Must have additional fields.", type);
		clif->message(fd, msg);
		return 0;
	}
	if (l > 9) {
		safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in [%s] type. There are too many types!", type);
		clif->message(fd, msg);
		return 0;
	}
	for (i = 1; i < l; i++) {
		switch(tmp[i]) {
		case 'S': case 's':
			if ((flag & FILTER_SELF) != 0) {
				safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in [%s] type. [S]elf flag has been set.", type);
				clif->message(fd, msg);
				return 0;
			}
			flag |= FILTER_SELF;
			break;
		case 'P': case 'p':
			if ((flag & FILTER_PARTY) != 0) {
				safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in [%s] type. [P]arty flag has been set.", type);
				clif->message(fd, msg);
				return 0;
			}
			flag |= FILTER_PARTY;
			break;
		case 'G': case 'g':
			if ((flag & FILTER_GUILD) != 0) {
				safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in [%s] type. [G]uild flag has been set.", type);
				clif->message(fd, msg);
				return 0;
			}
			flag |= FILTER_GUILD;
			break;
		case 'C': case 'c':
			if ((flag & FILTER_CLAN) != 0) {
				safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in [%s] type. [C]lan flag has been set.", type);
				clif->message(fd, msg);
				return 0;
			}
			flag |= FILTER_CLAN;
			break;
		case 'B': case 'b':
			if ((flag & FILTER_BG) != 0) {
				safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in [%s] type. [B]attleground flag has been set.", type);
				clif->message(fd, msg);
				return 0;
			}
			flag |= FILTER_BG;
			break;
		case 'O': case 'o':
			if ((flag & FILTER_OTHER) != 0) {
				safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in [%s] type. [O]thers flag has been set.", type);
				clif->message(fd, msg);
				return 0;
			}
			flag |= FILTER_OTHER;
			break;
		case 'H': case 'h':
			if ((flag & FILTER_HOMUN) != 0) {
				safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in [%s] type. [H]omunculus/Pets/Mercenary/Elemental flag has been set.", type);
				clif->message(fd, msg);
				return 0;
			}
			flag |= FILTER_HOMUN;
			break;
		case 'M': case 'm':
			if ((flag & FILTER_MOB) != 0) {
				safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in [%s] type. [M]onster flag has been set.", type);
				clif->message(fd, msg);
				return 0;
			}
			flag |= FILTER_MOB;
			break;
		default: {
			char subtype[2];
			subtype[0] = TOUPPER(tmp[i]);
			subtype[1] = '\0';
			safesnprintf(msg, CHAT_SIZE_MAX, "Invalid format in [%s] type. Unknown type '%s'.", type, subtype);
			clif->message(fd, msg);
			return 0;
		}
		}
	}
	return flag;
}

void print_packetfilter_sub(int fd, int flag, const char *message) {
	clif->message(fd, message);
	if ((flag & FILTER_SELF) != 0)
		clif->message(fd, "    -> Self");
	if ((flag & FILTER_PARTY) != 0)
		clif->message(fd, "    -> Party");
	if ((flag & FILTER_GUILD) != 0)
		clif->message(fd, "    -> Guild");
	if ((flag & FILTER_BG) != 0)
		clif->message(fd, "    -> Battleground");
	if ((flag & FILTER_CLAN) != 0)
		clif->message(fd, "    -> Clan");
	if ((flag & FILTER_OTHER) != 0)
		clif->message(fd, "    -> Other players");
	if ((flag & FILTER_HOMUN) != 0)
		clif->message(fd, "    -> Homunculus/Pets/Mercenary/Elemental");
	if ((flag & FILTER_MOB) != 0)
		clif->message(fd, "    -> Monster");
	return;
}

void print_packetfilter(int fd, struct player_data *ssd) {
	if (ssd->block_chat != 0)
		print_packetfilter_sub(fd, ssd->block_chat, "Enabled [C]hat filter.");
	if (ssd->block_emotion != 0)
		print_packetfilter_sub(fd, ssd->block_emotion, "Enabled [E]motion filter.");
	if (ssd->block_item != 0)
		print_packetfilter_sub(fd, ssd->block_item, "Enabled [I]tem use filter.");
	if (ssd->block_attack != 0)
		print_packetfilter_sub(fd, ssd->block_attack, "Enabled normal [A]ttack filter.");
	if (ssd->block_buff != 0)
		print_packetfilter_sub(fd, ssd->block_buff, "Enabled skill [B]uff filter.");
	if (ssd->block_status != 0)
		print_packetfilter_sub(fd, ssd->block_status, "Enabled [S]tatus effect filter.");
	if (ssd->block_target_spell != 0)
		print_packetfilter_sub(fd, ssd->block_target_spell, "Enabled single [T]arget offensive skill/spell filter.");
	if (ssd->block_aoe_spell != 0)
		print_packetfilter_sub(fd, ssd->block_aoe_spell, "Enabled [G]round based spell or area spell filter.");
	if (ssd->block_music != 0)
		print_packetfilter_sub(fd, ssd->block_music, "Enabled [M]usic filter from bard and dancer songs.");
	if (ssd->block_direction != 0)
		print_packetfilter_sub(fd, ssd->block_direction, "Enabled [D]irection filter.");
	return;
}

void print_packetfilter_guide(int fd) {
	clif->message(fd, " ==== @packetfilter ===");
	clif->message(fd, "Main types are:");
	clif->message(fd, "    [C] -> block normal Chat. Include dancer's scream and bard's frost joke.");
	clif->message(fd, "    [E] -> block Emotions. (not include pet performance)");
	clif->message(fd, "    [I] -> block Item use effect.");
	clif->message(fd, "    [A] -> block normal Attack animation, base on receiving side. Your own attack are always shown.");
	clif->message(fd, "    [B] -> block Buff spell effect, base on receiving side. Your own action are always shown.");
	clif->message(fd, "    [S] -> block Status effect.");
	clif->message(fd, "    [T] -> block Target skill/spell effect, base on receiving side. Your own action are always shown.");
	clif->message(fd, "    [G] -> block Ground based spell or aoe spell effect. Some skills are not filtered.");
	clif->message(fd, "    [M] -> block Music from bard and dancer songs.");
	clif->message(fd, "    [D] -> block Direction and face direction.");
	clif->message(fd, "Sub types are:");
	clif->message(fd, "    [S] -> Self");
	clif->message(fd, "    [P] -> Party");
	clif->message(fd, "    [G] -> Guild");
	clif->message(fd, "    [C] -> Clan");
	clif->message(fd, "    [B] -> Battleground");
	clif->message(fd, "    [O] -> Other players");
	clif->message(fd, "    [H] -> Homunculus/Pets/Mercenary/Elemental");
	clif->message(fd, "    [M] -> Monsters");
	clif->message(fd, "Example: @packetfilter COHM EOHM");
	clif->message(fd, "    -> block normal chat and emotion from non-related players, pets, homunculus and monsters.");
	clif->message(fd, "         this doesn't block normal chat from party/guild members.");
	clif->message(fd, "Example: @packetfilter off");
	clif->message(fd, "    -> turn packet filter off");
	return;
}

void remove_packetfilter(int fd, struct player_data *ssd) {
	ssd->block_ = 0;
	ssd->block_chat = 0;
	ssd->block_emotion = 0;
	ssd->block_item = 0;
	ssd->block_attack = 0;
	ssd->block_buff = 0;
	ssd->block_status = 0;
	ssd->block_target_spell = 0;
	ssd->block_aoe_spell = 0;
	ssd->block_music = 0;
	ssd->block_direction = 0;
	clif->message(fd, "@packetfilter has turn OFF.");
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
		remove_packetfilter(fd, ssd);
		return true;
	}
	size_t l = strlen(message);
	char *tmp = (char*)aMalloc(l +1);
	unsigned short i = 0, j = 0;
	int chat_flag = 0;
	int emotion_flag = 0;
	int item_flag = 0;
	int attack_flag = 0;
	int buff_flag = 0;
	int status_flag = 0;
	int target_spell_flag = 0;
	int aoe_spell_flag = 0;
	int music_flag = 0;
	int direction_flag = 0;
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
			case 'I': case 'i':
				item_flag = set_flag(fd, tmp);
				if (item_flag == 0) {
					aFree(tmp);
					return false;
				}
				break;
			case 'A': case 'a':
				attack_flag = set_flag(fd, tmp);
				if (attack_flag == 0) {
					aFree(tmp);
					return false;
				}
				break;
			case 'B': case 'b':
				buff_flag = set_flag(fd, tmp);
				if (buff_flag == 0) {
					aFree(tmp);
					return false;
				}
				break;
			case 'S': case 's':
				status_flag = set_flag(fd, tmp);
				if (status_flag == 0) {
					aFree(tmp);
					return false;
				}
				break;
			case 'T': case 't':
				target_spell_flag = set_flag(fd, tmp);
				if (target_spell_flag == 0) {
					aFree(tmp);
					return false;
				}
				break;
			case 'G': case 'g':
				aoe_spell_flag = set_flag(fd, tmp);
				if (aoe_spell_flag == 0) {
					aFree(tmp);
					return false;
				}
				break;
			case 'M': case 'm':
				music_flag = set_flag(fd, tmp);
				if (music_flag == 0) {
					aFree(tmp);
					return false;
				}
				break;
			case 'D': case 'd':
				direction_flag = set_flag(fd, tmp);
				if (direction_flag == 0) {
					aFree(tmp);
					return false;
				}
				break;
			default: {
				char type[2];
				type[0] = tmp[0];
				type[1] = '\0';
				safesnprintf(msg, CHAT_SIZE_MAX, "Unknown type '%s'.", type);
				clif->message(fd, msg);
				aFree(tmp);
				return false;
			}
			}
			j = 0;
		}
		i++;
	}
	if ((chat_flag & FILTER_SELF) != 0) { // the client just send the chat anyway
		clif->message(fd, "Warning: [C]hat type can't filter [S]elf. Disable this filter.");
		chat_flag &= ~FILTER_SELF;
	}
	if ((emotion_flag & FILTER_SELF) != 0) { // why there is a weird bug that after @packetfilter off, I can't even use /emote anymore ??
		clif->message(fd, "Warning: [E]motion type can't filter [S]elf. Disable this filter.");
		emotion_flag &= ~FILTER_SELF;
	}
	if ((item_flag & FILTER_SELF) != 0) { // it looks like bug ? after using the item, the item amount in inventory still stay the same, better disable it
		clif->message(fd, "Warning: [I]tem type can't filter [S]elf. Disable this filter.");
		item_flag &= ~FILTER_SELF;
	}
	if ((attack_flag & FILTER_SELF) != 0) { // the very defination of moonwalk, you walk a few steps, suddenly go back a few cell, and walk in front again, and backstep again ...
		clif->message(fd, "Warning: [A]ttack type can't filter [S]elf. Disable this filter.");
		attack_flag &= ~FILTER_SELF;
	}
	if ((status_flag & FILTER_SELF) != 0) { // this is no brainer, why you want to block your own status change effect ?
		clif->message(fd, "Warning: [S]tatus type can't filter [S]elf. Disable this filter.");
		status_flag &= ~FILTER_SELF;
	}
	if ((target_spell_flag & FILTER_SELF) != 0) { // ever heard of stutter step ? if you get his by level 10 firebolt, you should be stun for 1 second
		clif->message(fd, "Warning: [T]arget type skill/spells can't filter [S]elf. Disable this filter.");
		target_spell_flag &= ~FILTER_SELF;
	}
	if ((direction_flag & FILTER_SELF) != 0) { // the client just send the direction anyway
		clif->message(fd, "Warning: [D]irection type can't filter [S]elf. Disable this filter.");
		direction_flag &= ~FILTER_SELF;
	}
	if ((status_flag & FILTER_PARTY) != 0)
		clif->message(fd, "Caution: Enable [S]tatus type filter on [P]arty will cause 'Party Member abnormal status' to malfunction.");
	if (status_flag != 0 && buff_flag == 0)
		clif->message(fd, "Caution: Enable [S]tatus type filter but without [B]uff type filter will still cause status effect to shown on cast.");
	if ((direction_flag & (FILTER_OTHER|FILTER_MOB)) != 0) {
		clif->message(fd, "Caution: Enable [D]irection type filter may cause certain skills to not work correctly, example like Rogue's backstab.");
		clif->message(fd, "     Recommend use on filtering Party/Guild. Highly Recommend for supportive class use, like Priest, Soul Linker etc.");
	}
	if (!chat_flag && !emotion_flag && !item_flag && !attack_flag && !buff_flag && !status_flag && !target_spell_flag && !aoe_spell_flag && !music_flag && !direction_flag) {
		clif->message(fd, "No packet filter has been set.");
		remove_packetfilter(fd, ssd);
		aFree(tmp);
		return true;
	}
	ssd->block_ = 1;
	ssd->block_chat = chat_flag;
	ssd->block_emotion = emotion_flag;
	ssd->block_item = item_flag;
	ssd->block_attack = attack_flag;
	ssd->block_buff = buff_flag;
	ssd->block_status = status_flag;
	ssd->block_target_spell = target_spell_flag;
	ssd->block_aoe_spell = aoe_spell_flag;
	ssd->block_music = music_flag;
	ssd->block_direction = direction_flag;
	print_packetfilter(fd, ssd);
	safesnprintf(msg, CHAT_SIZE_MAX, "@packetfilter has turn ON.");
	clif->message(fd, msg);
	aFree(tmp);
	return true;
}

HPExport void plugin_init (void) {
	addAtcommand("packetfilter", packetfilter);
	addHookPost(pc, reg_received, pc_reg_received_post);
	addHookPre(clif, send_sub, clif_send_sub_pre);
}
