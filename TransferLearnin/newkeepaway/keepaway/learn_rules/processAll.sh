#!/bin/bash

for rawFile in $@
do

echo starting $rawFile

cut -d ' ' -f -14 $rawFile > c.tmp_processAll

cat header_rules.arff c.tmp_processAll > d.tmp_processAll.arff

./wRunParam.sh d.tmp_processAll.arff >& e.tmp_processAll

./3v2player_cmd.sh e.tmp_processAll
mv e.tmp_processAll $rawFile.results
mv e.tmp_processAll.h $rawFile.h

rm *.tmp_processAll

done

