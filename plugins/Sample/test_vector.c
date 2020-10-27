//===== Hercules Plugin ======================================
//= test_vector
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= x.x
//===== Compatible With: ===================================== 
//= Hercules 2020-10-27
//===== Description: =========================================
//= VECTOR_ test
//===== Topic ================================================
//= 
//===== Additional Comments: =================================
//= I was doubting this doesn't work, but it work indeed
//============================================================

#include "common/hercules.h"
#include "map/clif.h"
#include "map/atcommand.h"
#include "common/memmgr.h"
#include "common/strlib.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"test_vector",
	SERVER_TYPE_MAP,
	"x.x",
	HPM_VERSION,
};

struct vector_data {
	int abc;
	int xyz;
};

VECTOR_DECL(struct vector_data) vectordata;
VECTOR_DECL(int) asdf;

ACMD(iadd) {
	int i, num = atoi(message);
	char msg[CHAT_SIZE_MAX];

	ARR_FIND(0, VECTOR_LENGTH(asdf), i, VECTOR_INDEX(asdf, i) == num);
	if (i < VECTOR_LENGTH(asdf)) {
		safesnprintf(msg, CHAT_SIZE_MAX, "This value %d is found in 'asdf' index %d.", num, i);
		clif->message(fd, msg);
		return true;
	}

	VECTOR_ENSURE(asdf, 1, 1);
	VECTOR_PUSH(asdf, num);
	safesnprintf(msg, CHAT_SIZE_MAX, "This value %d is added in 'asdf' as index %d.", num, i);
	clif->message(fd, msg);
	return true;
}

ACMD(idel) {
	int i, num = atoi(message);
	char msg[CHAT_SIZE_MAX];

	ARR_FIND(0, VECTOR_LENGTH(asdf), i, VECTOR_INDEX(asdf, i) == num);
	if (i == VECTOR_LENGTH(asdf)) {
		safesnprintf(msg, CHAT_SIZE_MAX, "This value %d is not found in 'asdf'.", num);
		clif->message(fd, msg);
		return true;
	}

	VECTOR_ERASE(asdf, i);
	safesnprintf(msg, CHAT_SIZE_MAX, "This value %d has remove in 'asdf' from index %d.", num, i);
	clif->message(fd, msg);
	return true;
}

ACMD(ishow) {
	char msg[CHAT_SIZE_MAX];

	if (VECTOR_LENGTH(asdf) == 0) {
		clif->message(fd, "No entry");
		return true;
	}
	for (int i = 0; i < VECTOR_LENGTH(asdf); i++) {
		safesnprintf(msg, CHAT_SIZE_MAX, "[%d/%d]. %d", i +1, VECTOR_LENGTH(asdf), VECTOR_INDEX(asdf, i));
		clif->message(fd, msg);
	}
	return true;
}

ACMD(sadd) {
	int i, num = atoi(message);
	char msg[CHAT_SIZE_MAX];

	ARR_FIND(0, VECTOR_LENGTH(vectordata), i, VECTOR_INDEX(vectordata, i).abc == num);
	if (i < VECTOR_LENGTH(vectordata)) {
		safesnprintf(msg, CHAT_SIZE_MAX, "This value %d is found in 'abc' index %d.", num, i);
		clif->message(fd, msg);
		return true;
	}

	struct vector_data data_;
	data_.abc = num;
	data_.xyz = num +100;

	VECTOR_ENSURE(vectordata, 1, 1);
	VECTOR_PUSH(vectordata, data_);
	safesnprintf(msg, CHAT_SIZE_MAX, "This value %d is added in 'abc' as index %d.", num, i);
	clif->message(fd, msg);
	return true;
}

ACMD(sdel) {
	int i, num = atoi(message);
	char msg[CHAT_SIZE_MAX];

	ARR_FIND(0, VECTOR_LENGTH(vectordata), i, VECTOR_INDEX(vectordata, i).abc == num);
	if (i == VECTOR_LENGTH(vectordata)) {
		safesnprintf(msg, CHAT_SIZE_MAX, "This value %d is not found in 'abc'.", num);
		clif->message(fd, msg);
		return true;
	}

	VECTOR_ERASE(vectordata, i);
	safesnprintf(msg, CHAT_SIZE_MAX, "This value %d has remove in 'abc' from index %d.", num, i);
	clif->message(fd, msg);
	return true;
}

ACMD(sshow) {
	char msg[CHAT_SIZE_MAX];

	if (VECTOR_LENGTH(vectordata) == 0) {
		clif->message(fd, "No entry");
		return true;
	}
	for (int i = 0; i < VECTOR_LENGTH(vectordata); i++) {
		safesnprintf(msg, CHAT_SIZE_MAX, "[%d/%d]. %d %d", i +1, VECTOR_LENGTH(vectordata), VECTOR_INDEX(vectordata, i).abc, VECTOR_INDEX(vectordata, i).xyz);
		clif->message(fd, msg);
	}
	return true;
}

HPExport void plugin_init(void) {
	addAtcommand("iadd", iadd);
	addAtcommand("idel", idel);
	addAtcommand("ishow", ishow);
	VECTOR_INIT(asdf);
	addAtcommand("sadd", sadd);
	addAtcommand("sdel", sdel);
	addAtcommand("sshow", sshow);
	VECTOR_INIT(vectordata);
}

HPExport void plugin_final(void) {
	VECTOR_CLEAR(vectordata);
	VECTOR_CLEAR(asdf);
}
