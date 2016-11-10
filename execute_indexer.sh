OPTION=$1
STEP_OPTION=$2

rm ./a.out > /dev/null 2>&1
rm ~/Dropbox/ir/result.txt > /dev/null 2>&1

if [ "$OPTION" == "-r" ]
then
	g++ -std=c++11 -O3 *.cpp
	if [ "$STEP_OPTION" == "-s1" ]
	then
		if ./a.out input/ index/ result/ -s1; then
			echo "Start execution"
		fi
	elif [ "$STEP_OPTION" == "-s2" ]
	then
		if ./a.out input/ index/ result/ -s2; then
			echo "Start execution"
		fi
	else
		if ./a.out input/ index/ result/; then
			echo "Start execution"
		fi
	fi
elif [ "$OPTION" == "-d" ]
then
	if g++ -std=c++11 -O3 *.cpp && ./a.out -d; then
		echo "Start execution"
	fi
elif [ "$OPTION" == "-c" ]
then
	g++ -std=c++11 *.cpp;
else
	if g++ -std=c++11 -O3 *.cpp && ./a.out; then
		echo "Start execution"
	fi
fi
