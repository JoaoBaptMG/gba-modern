#!/bin/sh
#
# Converts names from template to project-specific stuff, based on the 
# directory name. Changes minor things in Makefile and PN2 project files.
# 
# Usage : 
# - Copy template folder.
# - Rename folder.
# - Run script. This file will also remove itself once you're done.
#
# 'Pre-usage' considerations:
# - Set the library paths to point to the right folders on your system.

# ---------------------------------------------------------------------
# Variables
# ---------------------------------------------------------------------

files="template.pnproj template.ppg Makefile"
projdir=$(dirname $0)
proj=$(basename $projdir)


# Full path of the library itself, in platform-specific format.
# Yeah, it's ugly, but apparently the only way to get it right.
libtonc=e:/dev/gba/proj/tonc/code/tonclib

# Replacement list, space separated.
sedlist="PROJNAME=$proj LIBTONC=$libtonc"

# ---------------------------------------------------------------------
# Actual script
# ---------------------------------------------------------------------

# Construct sed-script for conversions.
for item in $sedlist ; do
	sedcmd="$sedcmd -e s#\[\[${item/=/\]\]#}#g"
done

# Apply conversions for appropriate files. 
# NOTE: if I don't do this by using an intermediate file, things go squirrily.
for file in $files
do
	if [ -e $file ] ; then
		echo Converting: $file
		dst=${file/template/$proj}			# Filename conversion.
		sed $sedcmd $file > $dst.t			# File content conversion.
		rm -f $file
		mv	$dst.t $dst
	fi
done

# Remove this script if not in the template directory.
if [ "$proj" != "template" ] && [ "$proj" != "nds-forge" ] ; then
	echo "This file will self-destruct in 3 ... 2 ... 1 ..."
	rm -f $0
fi
