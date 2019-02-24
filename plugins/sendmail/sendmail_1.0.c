//===== Hercules Plugin ======================================
//= *sendmail Script Command
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.0a
//===== Compatible With: ===================================== 
//= Hercules 2014-02-25
//===== Description: =========================================
//= send a mail to a player with updated notifications
//===== Topic ================================================
//= http://hercules.ws/board/topic/4608-sendmail/
//===== Additional Comments: =================================  
//= Thanks to Ind point out no need throw error anymore !
//= sendmail .@cid, .@sender$, .@title$, .@msg$;
//= sendmail .@cid, .@sender$, .@title$, .@msg$, .@zeny;
//= sendmail .@cid, .@sender$, .@title$, .@msg$, .@zeny, .@itemid, .@amount;
//= sendmail .@cid, .@sender$, .@title$, .@msg$, .@zeny, .@itemid, .@amount, .@refine, .@attribute, .@card0, .@card1, .@card2, .@card3;
//============================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../map/script.h"
#include "../map/itemdb.h"
#include "../map/status.h"
#include "../map/pc.h"
#include "../map/intif.h"
#include "../common/HPMi.h"
#include "../common/strlib.h"

#include "../common/HPMDataCheck.h" // should always be the last file included! (if you don't make it last, it'll intentionally break compile time)

HPExport struct hplugin_info pinfo = {
	"sendmail",		// Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"0.1",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

BUILDIN(sendmail) {
	struct map_session_data *sd;
	struct mail_message mail;
	int charid, zeny = 0, refine = 0, identify = 0, attribute = 0;
	short itemid = 0, amount = 0, card0 = 0, card1 = 0, card2 = 0, card3 = 0;
	const char *send_name, *title, *body;

	charid = script_getnum(st,2);
	if ( !( sd = map->charid2sd( charid ) ) ) {
		ShowError( "buildin_sendmail: Player CID %d is not online, destination could not reach.\n", charid );
		script_pushint(st,-1);
		return false;
	}

	send_name = script_getstr(st,3);
	if ( strlen(send_name) >= NAME_LENGTH ) {
		ShowError( "buildin_sendmail: Send name must not more than %d length !\n", NAME_LENGTH -1 );
		script_pushint(st,-2);
		return false;
	}

	title = script_getstr(st,4);
	if ( strlen(title) > MAIL_TITLE_LENGTH ) {
		ShowError( "buildin_sendmail: Mail Title must not more than %d length !\n", MAIL_TITLE_LENGTH );
		script_pushint(st,-3);
		return false;
	}

	body = script_getstr(st,5);
	if ( strlen(body) > MAIL_BODY_LENGTH ) {
		ShowError( "buildin_sendmail: Mail message must not more than %d length !\n", MAIL_BODY_LENGTH );
		script_pushint(st,-4);
		return false;
	}

	if ( script_hasdata(st,6) ) {
		zeny = script_getnum(st,6);
		if ( zeny < 0 || zeny > MAX_ZENY ) {
			ShowError( "buildin_sendmail: Invalid Zeny value %d !\n", zeny );
			script_pushint(st,-5);
			return false;
		}
	}

	if ( script_hasdata(st,7) ) {
		if ( data_isstring( script_getdata(st,7) ) ) {
			const char *item_name = script_getstr(st,7);
			struct item_data *idata = itemdb->search_name(item_name);
			if ( idata == NULL ) {
				ShowError( "buildin_sendmail: Unknown item name %s.\n", item_name );
				script_pushint(st,-6);
				return false;
			}
			itemid = idata->nameid;
			if ( idata->type == IT_WEAPON ||  idata->type == IT_ARMOR )
				identify = 1;
		}
		else {
			itemid = script_getnum(st,7);
			if ( itemdb->exists(itemid) == 0 ) {
				ShowError( "buildin_sendmail: Unknown item ID %d.\n", itemid );
				script_pushint(st,-6);
				return false;
			}
			else {
				struct item_data *idata = itemdb->search(itemid);
				if ( idata->type == IT_WEAPON ||  idata->type == IT_ARMOR )
					identify = 1;
			}
		}
	}

	if ( script_hasdata(st,8) ) {
		if ( itemdb->isstackable(itemid) )
			amount = script_getnum(st,8);
		else
			amount = 1;
		if ( amount < 0 || amount > MAX_AMOUNT ) {
			ShowError( "buildin_sendmail: Invalid item amount %d given.\n", amount );
			script_pushint(st,-7);
			return false;
		}
	}
	else if ( script_hasdata(st,7) )
		amount = 1;

	if ( script_hasdata(st,9) ) {
		refine = script_getnum(st,9);
		if ( refine < 0 || refine > MAX_REFINE ) {
			ShowError( "buildin_sendmail: Invalid refine amount %d given.\n", refine );
			script_pushint(st,-8);
			return false;
		}
		if ( script_hasdata(st,10) && script_hasdata(st,11) && script_hasdata(st,12) && script_hasdata(st,13) && script_hasdata(st,14) ) {
			attribute = script_getnum(st,10);
			if ( attribute < 0 )
				attribute = 0;
			else if ( attribute > 1 )
				attribute = 1;
			card0 = (short)script_getnum(st,11);
			card1 = (short)script_getnum(st,12);
			card2 = (short)script_getnum(st,13);
			card3 = (short)script_getnum(st,14);
		}
		else {
			ShowError( "buildin_sendmail: Not enough arguments for attibute or cards !\n" );
			script_pushint(st,-9);
			return false;
		}
	}
	memset( &mail, 0, sizeof(mail) );
	mail.dest_id = charid;
	safestrncpy( mail.send_name, send_name, NAME_LENGTH );
	safestrncpy( mail.dest_name, sd->status.name, NAME_LENGTH );
	safestrncpy( mail.title, title, MAIL_TITLE_LENGTH );
	safestrncpy( mail.body, body, MAIL_BODY_LENGTH );
	mail.zeny = zeny;
	mail.item.nameid = itemid;
	mail.item.amount = amount;
	mail.item.identify = identify;
	mail.item.refine = refine;
	mail.item.attribute = attribute;
	mail.item.card[0] = card0;
	mail.item.card[1] = card1;
	mail.item.card[2] = card2;
	mail.item.card[3] = card3;
	mail.timestamp = time(NULL);

	intif->Mail_send( sd->status.account_id, &mail );
	script_pushint(st,0);

	return true;
}

HPExport void plugin_init (void) {
	script = GET_SYMBOL("script");
	map = GET_SYMBOL("map");
	itemdb = GET_SYMBOL("itemdb");
	intif = GET_SYMBOL("intif");
	strlib = GET_SYMBOL("strlib");

	addScriptCommand( "sendmail", "isss?????????", sendmail );
}