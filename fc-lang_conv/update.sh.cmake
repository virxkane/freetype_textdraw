#!/bin/sh

die()
{
	echo $*
	exit 1
}

fc_lang_dir="@CMAKE_SOURCE_DIR@/fc-lang"

orth_files=`ls ${fc_lang_dir}/*.orth`
cd "@CMAKE_CURRENT_SOURCE_DIR@/files/" || die
for f in ${orth_files}
do
	#echo $f
	@CMAKE_CURRENT_BINARY_DIR@/fc-lang_conv "${f}"
done
