//===== Hercules Plugin ======================================
//= @market clone
//===== By: ==================================================
//= AnnieRuru
//= Credit - Dastgir -> http://hercules.ws/board/topic/7188-market/
//===== Current Version: =====================================
//= 1.2
//===== Compatible With: ===================================== 
//= Hercules 2014-09-20
//===== Description: =========================================
//= Create a market clone, to leave a message for other players
//= while the player can go hunting/questing/events
//===== Topic ================================================
//= http://hercules.ws/board/topic/7242-
//===== Additional Comments: =================================  
//= -- AtCommands --
//= @market "<Title>" "<Message>"
//=   create a market clone with a chat room titled -> "<Title>"
//=   when players tries to join the chat room, it refuse the joining, but instead leave a message -> "<Message>"
//= @marketkill
//=   kill the market clone without logging off
//=
//= a GM doing a @killmonster @killmonster2, or *killmonster *kilmonsterall script command will not remove the clone
//= but @reloadscript, however, will remove it
//============================================================

//	how many seconds delay from creating another market clone ?
int market_clone_delay = 10; // 10 seconds

//	the color message when clicking on the clone's chat room
int market_msg_color = 0xFFC0CB; // PINK !

//============================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../map/pc.h"
#include "../map/mob.h"
#include "../map/npc.h"
#include "../map/chat.h"
#include "../map/clif.h"
#include "../map/unit.h"
#include "../common/malloc.h"
#include "../common/strlib.h"
#include "../common/socket.h"
#include "../common/HPMi.h"
#include "../common/HPMDataCheck.h" // should always be the last file included! (if you don't make it last, it'll intentionally break compile time)

HPExport struct hplugin_info pinfo = {
	"market_clone",	// Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"0.1",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

struct player_data {
	int market_clone_id;
	int market_clone_delay;
};

struct monster_data {
	int market_chat_id;
	char market_message[CHAT_SIZE_MAX];
};

int mob_clone_spawn_market( struct map_session_data *sd, int16 m, int16 x, int16 y, char market_title[], char market_msg[] ) { //Copy of mob_clone_spawn with some modification.
	int class_;
	struct mob_data *md;
	struct monster_data *mmd;
	struct mob_db* db;
	struct status_data *mstatus;
	struct chat_data* cd;

	ARR_FIND( MOB_CLONE_START, MOB_CLONE_END, class_, mob->db_data[class_] == NULL );
	if ( class_ >= MOB_CLONE_END )
		return 0;

	db = mob->db_data[class_] = (struct mob_db*)aCalloc( 1, sizeof(struct mob_db) );
	mstatus = &db->status;
	safestrncpy( db->sprite, sd->status.name, NAME_LENGTH );
	safestrncpy( db->name, sd->status.name, NAME_LENGTH );
	safestrncpy( db->jname, sd->status.name, NAME_LENGTH );
	db->lv = status->get_lv(&sd->bl);
	memcpy( mstatus, &sd->base_status, sizeof( struct status_data ) );
	mstatus->rhw.atk = mstatus->rhw.atk2 = mstatus->lhw.atk = mstatus->lhw.atk2 = mstatus->hp = mstatus->max_hp = mstatus->sp = mstatus->max_sp =  1;
	mstatus->mode = 0;
	memcpy( &db->vd, &sd->vd, sizeof( struct view_data ) );
	db->base_exp = db->job_exp = db->range2 = db->range3 = 1;
	db->option = 0;

	md = mob->once_spawn_sub( &sd->bl, m, x, y, sd->status.name, class_, "", SZ_SMALL, AI_NONE );
	if ( !md )
		return 0;
	md->special_state.clone = 1;
	mob->spawn(md);
	unit->setdir( &md->bl, unit->getdir(&sd->bl) );
	cd = chat->create( &md->bl, market_title, "", 1, false, 0, "", 0, 1, MAX_LEVEL );
	if ( !cd )
		return 0;
	mmd = getFromMOBDATA(md,0);
	CREATE( mmd, struct monster_data, 1 );
	mmd->market_chat_id = cd->bl.id;
	safestrncpy( mmd->market_message, market_msg, CHAT_SIZE_MAX );
	addToMOBDATA( md, mmd, 0, true );
	clif->dispchat( cd, 0 );
	if ( sd->vd.dead_sit == 2 )
		clif->sitting( &md->bl );
	return md->bl.id;
}

ACMD(market){
	struct player_data *ssd;
	char title[CHAT_SIZE_MAX], msg[CHAT_SIZE_MAX], atcmd_output[CHAT_SIZE_MAX];
	if ( !( ssd = getFromMSD(sd,0) ) ) {
		CREATE( ssd, struct player_data, 1 );
		ssd->market_clone_id = 0;
		ssd->market_clone_delay = 0;
		addToMSD( sd, ssd, 0, true );
	}
	if ( ssd->market_clone_id ) {
		clif->message( fd, "You already have a Market clone summoned. Type '@marketkill' to remove that clone.");
		return false;
	}
	if ( ssd->market_clone_delay + market_clone_delay > (int)time(NULL) ) {
		safesnprintf( atcmd_output, CHAT_SIZE_MAX, "You must wait %d seconds before using this command again.", ssd->market_clone_delay + market_clone_delay - (int)time(NULL) );
		clif->message( fd, atcmd_output );
		return false;
	}
	if ( !map->list[sd->bl.m].flag.town ) {
		clif->message( fd, "You can only use @market in a town.");
		return false;
	}
	if ( map->getcell( sd->bl.m, sd->bl.x, sd->bl.y, CELL_CHKNOCHAT ) ) {
		clif->message( fd, "You cannot use @market in this area.");
		return false;
	}
	if ( pc_isdead(sd) ) {
		clif->message( fd, "You can't create a Market clone while you are dead." );
		return false;
	}
	if ( npc->isnear( &sd->bl ) ) {
		clif->message( fd, "You can't create a Market clone too near to an npc." );
		return false;
	}
	if ( !message || !*message ) {
		clif->message( fd, "Syntax: @market \"<Title>\" \"<Message>\"");
		return false;
	}
//	if ( sscanf( message, "\"%[^\"]\" \"%[^\"]\"", title, msg ) < 2 ) {
//		clif->message( fd, "Remember the Quotation Mark -> \"");
//		clif->message( fd, "Syntax: @market \"<Title>\" \"<Message>\"");
//		return false;
//	} no more f*cking sscanf
	{ // say hello to the dirties string calculation ~ Hooray ~ !!
		int i = 0, j = 0, l = strlen( message ) +1;
		char *temp = (char*)aMalloc( strlen( message ) +1 );
		if ( message[0] != '\"' ) {
			clif->message( fd, "Remember the <Title> should start with a Quotation Mark -> \"");
			clif->message( fd, "Syntax: @market \"<Title>\" \"<Message>\"");
			return false;
		}
		i = 1;
		while ( i <= l ) {
			if ( message[i] == '\"' || message[i] == '\0' )
				break;
			else
				temp[j++] = message[i];
			i++;
		}
		if ( message[i] != '\"' ) {
			clif->message( fd, "Remember the <Title> should end with a Quotation Mark -> \"");
			clif->message( fd, "Syntax: @market \"<Title>\" \"<Message>\"");
			return false;
		}
		temp[j] = '\0';
		safestrncpy( title, temp, CHAT_SIZE_MAX );		
		i++;
		if ( message[i] != ' ' ) {
			clif->message( fd, "Remember the [Space] between the <Title> and <Message>.");
			clif->message( fd, "Syntax: @market \"<Title>\" \"<Message>\"");
			return false;
		}
		i++;
		if ( message[i] != '\"' ) {
			clif->message( fd, "Remember the <Message> should start with a Quotation Mark -> \"");
			clif->message( fd, "Syntax: @market \"<Title>\" \"<Message>\"");
			return false;
		}
		i++;
		j = 0;
		while ( i <= l ) {
			if ( message[i] == '\"' || message[i] == '\0' )
				break;
			else
				temp[j++] = message[i];
			i++;
		}
		if ( message[i] != '\"' ) {
			clif->message( fd, "Remember the <Message> should end with a Quotation Mark -> \"");
			clif->message( fd, "Syntax: @market \"<Title>\" \"<Message>\"");
			return false;
		}
		temp[j] = '\0';
		safestrncpy( msg, temp, CHAT_SIZE_MAX );
		aFree( temp );
	}
	if ( strlen( title ) < 4 ) {
		clif->message( fd, "The Title must more than 4 characters." );
		return false;
	}
	if ( strlen( title ) >= CHATROOM_TITLE_SIZE ) {
		safesnprintf( atcmd_output, CHAT_SIZE_MAX, "The Title must not more than %d characters.", CHATROOM_TITLE_SIZE );
		clif->message( fd, atcmd_output );
		return false;
	}
	if ( strlen( msg ) < 4 ) {
		clif->message( fd, "The Message must more than 4 characters." );
		return false;
	}
	ssd->market_clone_id = mob_clone_spawn_market( sd, sd->bl.m, sd->bl.x, sd->bl.y, title, msg );
	if ( !ssd->market_clone_id ) {
		clif->message( fd, "Market clone Cannot be Created." );
		return false;
	}
	clif->message( fd, "Market clone Created." );
	return true;
}

ACMD(marketkill){
	struct player_data *ssd;
	if ( !( ssd = getFromMSD(sd,0) ) ) {
		CREATE( ssd, struct player_data, 1 );
		ssd->market_clone_id = 0;
		ssd->market_clone_delay = 0;
		addToMSD( sd, ssd, 0, true );
	}
	if ( !ssd->market_clone_id ) {
		clif->message( fd, "You don't have a market clone yet. Type '@market \"<title>\" \"<message>\"' to create one.");
		return false;
	}
	status->damage( NULL, map->id2bl( ssd->market_clone_id ), 100, 0, 0, 5 );
	clif->message( fd, "Your market clone has removed." );
	ssd->market_clone_id = 0;
	ssd->market_clone_delay = (int)time(NULL);
	return true;
}

//	overwrite @reloadscript to reset player's sd->market_clone_id
ACMD(reloadscript) {
	struct s_mapiterator* iter;
	struct map_session_data* pl_sd;
	struct player_data *ssd;

	//atcommand_broadcast( fd, sd, "@broadcast", "Server is reloading scripts..." );
	//atcommand_broadcast( fd, sd, "@broadcast", "You will feel a bit of lag at this point !" );
	
	iter = mapit->alloc( MAPIT_NORMAL, BL_PC );
	for( pl_sd = (TBL_PC*)mapit->first(iter); mapit->exists(iter); pl_sd = (TBL_PC*)mapit->next(iter) ) {
		if (pl_sd->npc_id || pl_sd->npc_shopid) {
			if (pl_sd->state.using_fake_npc) {
				clif->clearunit_single(pl_sd->npc_id, CLR_OUTSIGHT, pl_sd->fd);
				pl_sd->state.using_fake_npc = 0;
			}
			if (pl_sd->state.menu_or_input)
				pl_sd->state.menu_or_input = 0;
			if (pl_sd->npc_menu)
				pl_sd->npc_menu = 0;

			pl_sd->npc_id = 0;
			pl_sd->npc_shopid = 0;
			if (pl_sd->st && pl_sd->st->state != END)
				pl_sd->st->state = END;
		}
		if (( ssd = getFromMSD(pl_sd,0) )) {
			if ( ssd->market_clone_id ) {
				status->damage( NULL, map->id2bl( ssd->market_clone_id ), 100, 0, 0, 5 );
				ssd->market_clone_id = 0;
				ssd->market_clone_delay = (int)time(NULL);
			}
		}
	}
	mapit->free(iter);
	
	flush_fifos();
	map->reloadnpc(true, NULL, 0); // reload config files seeking for npcs
	script->reload();
	npc->reload();
	
	clif->message( fd, msg_txt(100) ); // Scripts have been reloaded.
	
	return true;
}

int battle_check_target_post( int retVal, struct block_list *src, struct block_list *target, int *flag ) {
	if ( retVal == 1 && target->type == BL_MOB ) {
		struct monster_data *mmd;
		if (( mmd = getFromMOBDATA( (TBL_MOB*)target, 0 ) ))
			return -1;
	}
	return retVal;
}

bool chat_joinchat_pre( struct map_session_data *sd, int *chatid, const char *pass ) {
	struct chat_data* cd = (struct chat_data*)map->id2bl( *chatid );
	if( cd == NULL || cd->bl.type != BL_CHAT || cd->bl.m != sd->bl.m || sd->state.vending || sd->state.buyingstore || sd->chatID || ((cd->owner->type == BL_NPC) ? cd->users+1 : cd->users) >= cd->limit ) {
		clif->joinchatfail(sd,0); // room full
		hookStop();
		return false;
	}
	if ( cd->owner->type == BL_MOB ) {
		struct mob_data *md = (TBL_MOB*)cd->owner;
		struct monster_data *mmd;
		if (( mmd = getFromMOBDATA(md,0) )) {
			short msg_len = 0;
			int colorcode = (market_msg_color & 0x0000FF) << 16 | (market_msg_color & 0x00FF00) | (market_msg_color & 0xFF0000) >> 16;
			char output[CHAT_SIZE_MAX];
			safesnprintf( output, CHAT_SIZE_MAX, "%s : %s", md->name, mmd->market_message );
			msg_len = strlen( output )+1;
			WFIFOHEAD( sd->fd, msg_len + 12 );
			WFIFOW( sd->fd, 0 ) = 0x2C1;
			WFIFOW( sd->fd, 2 ) = msg_len + 12;
			WFIFOL( sd->fd, 4 ) = 0;
			WFIFOL( sd->fd, 8 ) = colorcode;
			safestrncpy( (char*)WFIFOP( sd->fd,12 ), output, msg_len );
			WFIFOSET( sd->fd, msg_len + 12 );
			hookStop();
			return true;
		}
	}
	return true;
}

void clif_getareachar_unit_post( struct map_session_data *sd, struct block_list *bl ) {
	if ( bl->type == BL_MOB ) {
		struct monster_data *mmd;
		if (( mmd = getFromMOBDATA( (TBL_MOB*)bl, 0) ))
			clif->dispchat( (struct chat_data*)map->id2bl( mmd->market_chat_id ), sd->fd );
	}
	return;
}

int map_quit_pre( struct map_session_data *sd ) {
	struct player_data *ssd;
	if (( ssd = getFromMSD(sd,0) ))
		if ( ssd->market_clone_id )
			status->damage( NULL, map->id2bl( ssd->market_clone_id ), 100, 0, 0, 5 );
	return 0;
}

int atkillmonster_sub_pre( struct block_list *bl, va_list *ap ) {
	struct mob_data *md = (TBL_MOB*)bl;
	struct monster_data *mmd;

	if ( ( mmd = getFromMOBDATA( md, 0 ) ) ) {
		hookStop();
		return 0;
	}
	return 0;
}

int buildin_killmonster_sub_strip_pre( struct block_list *bl, va_list *ap ) {
	struct mob_data *md = (TBL_MOB*)bl;
	struct monster_data *mmd;

	if ( ( mmd = getFromMOBDATA( md, 0 ) ) ) {
		hookStop();
		return 0;
	}
	return 0;
}

int buildin_killmonster_sub_pre( struct block_list *bl, va_list *ap ) {
	struct mob_data *md = (TBL_MOB*)bl;
	struct monster_data *mmd;

	if ( ( mmd = getFromMOBDATA( md, 0 ) ) ) {
		hookStop();
		return 0;
	}
	return 0;
}

int buildin_killmonsterall_sub_strip_pre( struct block_list *bl, va_list *ap ) {
	struct mob_data *md = (TBL_MOB*)bl;
	struct monster_data *mmd;

	if ( ( mmd = getFromMOBDATA( md, 0 ) ) ) {
		hookStop();
		return 0;
	}
	return 0;
}

int buildin_killmonsterall_sub_pre( struct block_list *bl, va_list *ap ) {
	struct mob_data *md = (TBL_MOB*)bl;
	struct monster_data *mmd;

	if ( ( mmd = getFromMOBDATA( md, 0 ) ) ) {
		hookStop();
		return 0;
	}
	return 0;
}

HPExport void plugin_init (void) {
	map = GET_SYMBOL("map");
	mob = GET_SYMBOL("mob");
	npc = GET_SYMBOL("npc");
	chat = GET_SYMBOL("chat");
	clif = GET_SYMBOL("clif");
	unit = GET_SYMBOL("unit");
	mapit = GET_SYMBOL("mapit");
	sockt = GET_SYMBOL("sockt");
	script = GET_SYMBOL("script");
	status = GET_SYMBOL("status");
	strlib = GET_SYMBOL("strlib");
	session = GET_SYMBOL("session");
	iMalloc = GET_SYMBOL("iMalloc");
	atcommand = GET_SYMBOL("atcommand");

	addAtcommand( "market", market );
	addAtcommand( "marketkill", marketkill );
	addAtcommand( "reloadscript", reloadscript );

	addHookPost( "battle->check_target", battle_check_target_post );
	addHookPre( "chat->join", chat_joinchat_pre );
	addHookPost( "clif->getareachar_unit", clif_getareachar_unit_post );
	addHookPre( "map->quit", map_quit_pre );
	addHookPre( "atcommand->atkillmonster_sub", atkillmonster_sub_pre );
	addHookPre( "script->buildin_killmonster_sub_strip", buildin_killmonster_sub_strip_pre );
	addHookPre( "script->buildin_killmonster_sub", buildin_killmonster_sub_pre );
	addHookPre( "script->buildin_killmonsterall_sub_strip", buildin_killmonsterall_sub_strip_pre );
	addHookPre( "script->buildin_killmonsterall_sub", buildin_killmonsterall_sub_pre );
}