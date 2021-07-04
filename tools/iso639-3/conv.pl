#!/usr/bin/perl

$inp_filename = "iso-639-3.tab";
$out1_filename = "../../locale_data/iso-639-3_data.c";
$out2_filename = "../../locale_data/iso-639-3_data.h";

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
// ISO-639-3 Language registry.
// License: Public Domain.
// This file is autogenerated.
// Based on data from https://iso639-3.sil.org/code_tables/download_tables
// file: iso-639-3.tab
// url: https://iso639-3.sil.org/sites/iso639-3/files/downloads/iso-639-3.tab

#include "iso-639-3_data.h"

#include <stddef.h>

const struct iso639_3_rec iso639_3_data[] = {
EOF

$count = 0;
$/="\r\n";
foreach $line (@lines) {
	chomp($line);
	my ($id, $part2b, $part2t, $part1, $scope, $type, $ref_name, $comment) = split(/\t/, $line);
	if (length($id) == 3 && length($ref_name) > 0) {
		my $part2b_c = length($part2b) > 0 ? "\"$part2b\"" : "NULL";
		my $part2t_c = length($part2t) > 0 ? "\"$part2t\"" : "NULL";
		my $part1_c = length($part1) > 0 ? "\"$part1\"" : "NULL";
		print OUTF1 "\t{\"$id\", $part2b_c, $part2t_c, $part1_c, \"$ref_name\"},\n";
		$count++;
	}
}

print OUTF1 "};\n\n";
#print OUTF1 "unsigned int iso639_3_data_sz = $count;\n\n";
close(OUTF1);

print OUTF2 <<EOF;
// ISO-639-3 Language registry.
// License: Public Domain
// This file is autogenerated.
// Based on data from https://iso639-3.sil.org/code_tables/download_tables
// file: iso-639-3.tab
// url: https://iso639-3.sil.org/sites/iso639-3/files/downloads/iso-639-3.tab

#ifndef ISO639_3_DATA_H
#define ISO639_3_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#define ISO639_3_TAB_SZ	${count}

struct iso639_3_rec {
	/**
	 * The three-letter 639-3 identifier.
	 */
	const char* id;
	/**
	 * Equivalent 639-2 identifier of the bibliographic application code set, if there is one.
	 */
	const char* part2b;
	/**
	 * Equivalent 639-2 identifier of the terminology application code set, if there is one.
	 */
	const char* part2t;
	/**
	 * Equivalent 639-1 identifier, if there is one.
	 */
	const char* part1;
	/**
	 * Reference language name.
	 */
	const char* ref_name;
};

extern const struct iso639_3_rec iso639_3_data[];

#ifdef __cplusplus
}
#endif

#endif	// ISO639_3_DATA_H
EOF
close(OUTF2);
