#!/usr/bin/perl

$inp_filename = "country codes (iso.org).csv";
$out1_filename = "../../locale_data/iso-3166-1_data.c";
$out2_filename = "../../locale_data/iso-3166-1_data.h";

if (!open(INF, "< $inp_filename")) {
	die "Unable to open $inp_filename";
}
if (!open(OUTF1, "> $out1_filename")) {
	close(INF);
	die "Unable to open $out1_filename";
}
if (!open(OUTF2, "> $out2_filename")) {
	close(OUTF1);
	close(INF);
	die "Unable to open $out2_filename";
}
@lines = <INF>;
close(INF);

print OUTF1 <<EOF;
// ISO 3166-1 Country Codes.
// License: Public Domain.
// This file is autogenerated.
// Based on data from: https://www.iso.org/iso-3166-country-codes.html

#include "iso-3166-1_data.h"

#include <stddef.h>

const struct iso3166_1_rec iso3166_1_data[] = {
EOF

$count = 0;
$/="\n";
foreach $line (@lines) {
	chomp($line);
	my ($name_eng, $name_fr, $alpha2, $alpha3, $num) = split(/\|/, $line);
	if (length($alpha2) == 2 && length($alpha3) == 3 && length($name_eng) > 0 && scalar($num) > 0) {
		print OUTF1 "\t{\"$alpha3\", \"$alpha2\", \"$name_eng\", $num},\n";
		$count++;
	}
}

print OUTF1 "};\n\n";
close(OUTF1);

print OUTF2 <<EOF;
// ISO 3166-1 Country Codes.
// License: Public Domain.
// This file is autogenerated.
// Based on data from: https://www.iso.org/iso-3166-country-codes.html

#ifndef ISO3166_1_DATA_H
#define ISO3166_1_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#define ISO3166_1_DATA_SZ	${count}

struct iso3166_1_rec {
	/**
	 * English short name.
	 */
	const char* name;
	/**
	 * Alpha-2 code.
	 */
	const char* alpha2;
	/**
	 * Alpha-3 code.
	 */
	const char* alpha3;
	/**
	 * Numeric value.
	 */
	unsigned int num;
};

extern const struct iso3166_1_rec iso3166_1_data[];

#ifdef __cplusplus
}
#endif

#endif // ISO3166_1_DATA_H
EOF
close(OUTF2);
