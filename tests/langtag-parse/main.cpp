#include <stdio.h>

#include "clocaledata.h"

void testParse(const char* lang_tag);
void testMatch(const char* tag1, const char* tag2);

int main(int /*argc*/, char* /*argv*/[]) {
	testParse("en");
	testParse("eng");
	testParse("en-US");
	testParse("eng-US");
	testParse("eng-USA");
	testParse("eng_USA");
	testParse("en_us");
	testParse("en-GB");
	testParse("eng-GBR");

	testParse("ber_dz");
	testParse("ber_ma");
	testParse("ku_am");
	testParse("ku_iq");
	testParse("ku_ir");
	testParse("mn-CN");
	testParse("mn-MN");
	testParse("mn-Cyrl-MN");
	testParse("mn_cyrl");
	testParse("pa-PK");
	testParse("pap-AN");
	testParse("pap-AW");

	testParse("ru");
	testParse("ru-RU");
	testParse("rus-RUS");
	testParse("ru-Cyrl-RU");
	testParse("ru-Cyrl");

	testParse("und_zmth");
	testParse("und_zsye");

	// by full name
	testParse("english");
	testParse("russian");

	// invalid
	testParse("cba-CBA");
	testParse("en-CBA");

	printf("\n");

	testMatch("en", "eng");
	testMatch("en", "en-US");
	testMatch("en-US", "en");
	testMatch("en-US", "en-US");
	testMatch("en-US", "en-GB");

	return 0;
}

void testParse(const char* lang_tag) {
	CLocaleData locData(lang_tag);
	printf("%s => %s (valid=%s)\n", lang_tag, locData.langTag().toLocal8Bit().data(), (locData.isValid() ? "yes" : "no"));
}

void testMatch(const char* tag1, const char* tag2) {
	CLocaleData loc1(tag1);
	CLocaleData loc2(tag2);
	int match = loc1.calcMatch(loc2);
	printf("%s (%s) and\t%s (%s): match value=%d\n", tag1, loc1.langTag().toLocal8Bit().data(), tag2, loc2.langTag().toLocal8Bit().data(), match);
}
