#!/bin/sh

# we typically don't want the code-generated stuff in
# the git PR, so after fixing, restore it to before.
RESTORE_CODEGEN=1

# Interestingly there are differences in count for both;
# there are way more if we just narrow to misc-include-cleaner.
# misc-include-cleaner if this is the only check we run.
CLANG_TIDY_ARGS="--checks=-*,misc-include-cleaner,misc-header-include-cycle"
#CLANG_TIDY_ARGS=""

FIX_FILE=${1:-fix-headers-list.txt}

echo "Using fixing file ${FIX_FILE}"

bant dwyu ... -q > dwyu-before.txt

# Some generic textual fixes that are good to get
# out of the way before we let clang tidy on the code.

# Fix uses of geometry and stacktrace to included the one-level indirection.
rg '#include "boost/(geometry|stacktrace).hpp' -l src/ > boost-list.txt
if [ -s boost-list.txt ] ; then
  sed -i 's@#include "boost/\(geometry\|stacktrace\).hpp"@#include "boost/\1/\1.hpp"@' $(cat boost-list.txt)
fi

# Fix use of angle-bracketed boost
rg '#include <boost/' -l src/ -g "*.{h,hh,cc,hpp,cpp,hxx,cxx}" > boost-list.txt
if [ -s boost-list.txt ] ; then
  sed -i 's@#include <boost/\(.*\)>@#include "boost/\1"@' $(cat boost-list.txt)
fi

# Vendored headers should be iquoted
sed -i 's@#include <tcl.h>@#include "tcl.h"@' $(rg 'include.*<tcl.h>' -l src/ include/ test/)
sed -i 's@#include <yaml-cpp/yaml.h>@#include "yaml-cpp/yaml.h"@' $(rg 'include.*<yaml-cpp/yaml.h>' -l src/ include/ test/)
sed -i 's@#include <gtest/gtest.h>@#include "gtest/gtest.h"@' $(rg 'include.*<gtest/' -l src/ include/ test/)
sed -i 's@#include <gmock/gmock.h>@#include "gmock/gmock.h"@' $(rg 'include.*<gmock/' -l src/ include/ test/)
sed -i 's@#include <Eigen/\(.*\)>@#include "Eigen/\1"@' $(rg 'include.*<Eigen/' -l src/ include/ test/)
sed -i 's@#include <lemon/\(.*\)>@#include "lemon/\1"@' $(rg 'include.*<lemon/' -l src/ include/ test/)

# ... also observed some local project headers
sed -i 's@#include <odb/\(.*\)>@#include "odb/\1"@' $(rg 'include.*<odb/' -l src/ include/ test/)
sed -i 's@#include <dst/\(.*\)>@#include "dst/\1"@' $(rg 'include.*<dst/' -l src/ include/ test/)

# After first textual fixes, run clang tidy so that ...
etc/run-clang-tidy.sh $CLANG_TIDY_ARGS
grep misc-include-cleaner OpenROAD_clang-tidy.summary > count-before.txt
awk '/not used directly.*misc-include-cleaner/ { split($1, parts, ":"); print parts[1] " " $5 }' OpenROAD_clang-tidy.out | sort > not-used-before.txt

# ... we can fix missing includes
. <(../dev-tools/header-fixer.sh OpenROAD_clang-tidy.out "${FIX_FILE}" "User Code Begin Includes") > header-explanation.txt

# For now, only remove headers that are (a) not in headers (because they might
# provide accidental dependencies further down) (b) not end with .h of sorts, so
# only c++ system headers at this point.
# We can let this loose in full force once we have all missing headers fixed.
# (also, CAVE, clang-tidy sometimes suggests removals that are clearly needed)
#. <(../dev-tools/remove-superfluous-headers.sh OpenROAD_clang-tidy.out | grep -v "\.h")
#. <(../dev-tools/remove-superfluous-headers.sh OpenROAD_clang-tidy.out)

if [ ${RESTORE_CODEGEN} -eq 1 ]; then
  # Re-run code generator as we don't want to mess up these files.
  (cd src/odb/src/codeGenerator ; ./generate > /dev/null)
fi

# Don't accidentally mess up sta: it is a separate repo
(cd src/sta/ ; git stash)

# Now formatting to sort the headers into the right place
git status | awk '/modified: .*\.(h|hh|cc|hpp|cpp|hxx|cxx)/ {print $2}' | grep -v BUILD | grep -v etc/ | grep -v src/sta > fmt-list.txt
clang-format-18 -i -style=file:.clang-format $(cat fmt-list.txt)

bant dwyu ... -q > dwyu-after.txt

# Final run of clang-tidy.
etc/run-clang-tidy.sh $CLANG_TIDY_ARGS
grep misc-include-cleaner OpenROAD_clang-tidy.summary > count-after.txt
awk '/not used directly.*misc-include-cleaner/ { split($1, parts, ":"); print parts[1] " " $5 }' OpenROAD_clang-tidy.out | sort > not-used-after.txt

# Not all files are actually in git as we ran the code generator. So filter
# by these (this is N^2, but not a big deal. Unfortunately, `join` does not
# preserve output
. <(git status | awk '/modified: / {print "grep \"^"$2":\" header-explanation.txt"}') | sort | uniq > submit-header-explanation.txt

echo
echo "Counts misc-include-cleaner"
awk '{ if (FIRST) { printf("before - after: %d - %d = %d\n", FIRST, $1, FIRST - $1); } else { FIRST=$1;} }' count-before.txt count-after.txt
echo
wc --total=never -l not-used-before.txt not-used-after.txt

cat submit-header-explanation.txt
echo
echo "^ Also submit-header-explanation.txt"

echo
echo "-- dwyu diff (fyi, no actual change done) --"
diff dwyu-before.txt dwyu-after.txt
