#/lusr/java5/bin/java -Xmx900m weka.classifiers.rules.JRip -t $1 -T $2 -O 10
#/lusr/java5/bin/java -Xmx900m weka.classifiers.rules.JRip -t $1 -N 100 -O 10
#/lusr/java5/bin/java -Xmx900m weka.classifiers.rules.JRip -t $1

# -Xmx900m allocates 900MB of memory
nice java -Xmx900m weka.classifiers.rules.JRip -t $1 -O 10

#/lusr/java5/bin/java -Xmx900m weka.classifiers.rules.JRip -t $1 -N 100

