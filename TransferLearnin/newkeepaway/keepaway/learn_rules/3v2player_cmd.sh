#!/bin/bash

if [ -e "a.tmp" ]
then
	rm a.tmp
fi

grep "=>" $1 > a.tmp
cat a.tmp | awk '{ gsub("and", "\\&\\&"); print }' > b.tmp
cat b.tmp | awk '{ gsub("^\\(", "else if(("); print }' > c.tmp
cat c.tmp | awk '{ gsub("=>", ")"); print }' > d.tmp
cat d.tmp | awk '{ gsub("^ )", "else"); print }' > e.tmp
cat e.tmp | awk '{ gsub("Action=0", "Action=0;"); print}' > f.tmp
cat f.tmp | awk '{ gsub("Action=1", "Action=1;"); print}' > g.tmp
cat g.tmp | awk '{ gsub("Action=2", "Action=2;"); print}' > h.tmp
cat h.tmp | awk '{ gsub("\\([.0-9]+\\/[.0-9]+\\)", ""); print}' > i.tmp
#features: dist taker, angJump1, distJump1, distJump1Taker, angJump2, distJump2, distJump2Taker
#cat i.tmp | awk '{ gsub("s1", "state[3]"); print}' > j.tmp
#cat j.tmp | awk '{ gsub("s2", "state[11]"); print}' > j1.tmp
#cat j1.tmp | awk '{ gsub("s3", "state[1]"); print}' > j2.tmp
#cat j2.tmp | awk '{ gsub("s4", "state[9]"); print}' > j3.tmp
#cat j3.tmp | awk '{ gsub("s5", "state[12]"); print}' > j4.tmp
#cat j4.tmp | awk '{ gsub("s6", "state[2]"); print}' > j5.tmp
#cat j5.tmp | awk '{ gsub("s7", "state[10]"); print}' > j6.tmp
cat header.txt i.tmp footer.txt > $1.h

rm *.tmp

#mkdir ~/projects/rcssjava/keepaway-0.6/player_RR_$1
#mv $1.h ~/projects/rcssjava/keepaway-0.6/player_RR_$1/Rules.h
#cp ~/projects/ringworld2/player_RULES/* ~/projects/rcssjava/keepaway-0.6/player_RR_$1/
#cd ~/projects/rcssjava/keepaway-0.6/player_RR_$1
#make
#cd -
