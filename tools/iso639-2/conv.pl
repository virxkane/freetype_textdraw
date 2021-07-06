#!/usr/bin/perl

$inp_filename = "ISO-639-2_utf-8.txt";
$out1_filename = "../../locale_data/iso-639-2_data.c";
$out2_filename = "../../locale_data/iso-639-2_data.h";

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
// ISO-639-2 Language registry.
// License: Public Domain.
// This file is autogenerated.
// Based on data from: https://www.loc.gov/standards/iso639-2/ISO-639-2_utf-8.txt
// file: ISO-639-2_utf-8.txt
// url: https://www.loc.gov/standards/iso639-2/ISO-639-2_utf-8.txt

#include "iso-639-2_data.h"

#include <stddef.h>

const struct iso639_2_rec iso639_2_data[] = {
EOF

$count = 0;
$/="\n";
foreach $line (@lines) {
	chomp($line);
	my ($id, $part2t, $part1, $ref_name_eng, $ref_name_fr) = split(/\|/, $line);
	if (length($id) == 3 && length($ref_name_eng) > 0) {
		my $part2t_c = length($part2t) > 0 ? "\"$part2t\"" : "NULL";
		my $part1_c = length($part1) > 0 ? "\"$part1\"" : "NULL";
		print OUTF1 "\t{\"$id\", $part2t_c, $part1_c, \"$ref_name_eng\"},\n";
		$count++;
	}
}

print OUTF1 "};\n\n";
#print OUTF1 "unsigned int iso639_3_tab_data_sz = $count;\n\n";
close(OUTF1);

print OUTF2 <<EOF;
// ISO-639-2 Language registry.
// License: Public Domain.
// This file is autogenerated.
// Based on data from: https://www.loc.gov/standards/iso639-2/ISO-639-2_utf-8.txt
// file: ISO-639-2_utf-8.txt
// url: https://www.loc.gov/standards/iso639-2/ISO-639-2_utf-8.txt

#ifndef ISO639_2_DATA_H
#define ISO639_2_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#define ISO639_2_DATA_SZ	${count}

struct iso639_2_rec {
	/**
	 * The three-letter 639-2 identifier. This is bibliographic application code.
	 * alpha-3 language code.
	 */
	const char* id;
	/**
	 * Equivalent 639-2 identifier of the terminology application code set, if there is one.
	 */
	const char* part2t;
	/**
	 * Equivalent 639-1 identifier, if there is one.
	 * alpha-2 language code.
	 */
	const char* part1;
	/**
	 * Reference language name.
	 */
	const char* ref_name;
};

extern const struct iso639_2_rec iso639_2_data[];

#ifdef __cplusplus
}
#endif

#endif	// ISO639_2_DATA_H
EOF
close(OUTF2);