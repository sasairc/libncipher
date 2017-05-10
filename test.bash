#
# てすと？
#

function cleanup() {
	echo "*** cleanup directory ***"
	make clean && \
		rm -r "$1"

	return $?
}

function install_all() {
	echo "*** target install-all ***"
	make CC="$1" PREFIX="$2" install && \
		tree "$2"

	return $?
}

function run_static_bin() {
	echo "*** run ***"

	SEED128="1234567890qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNMあいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほまみむめもアイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマ"

	wget https://raw.githubusercontent.com/sasairc/yasuna/master/quotes/yasuna-quotes

	./command/sample --help
	./command/sample --version

	echo "*** default seed/delimiter ***"
	cat yasuna-quotes | ./command/sample -e | ./command/sample -d > dest.txt
	diff -c yasuna-quotes dest.txt
	echo "diff: $?"

	echo "*** seed128 ***"
	cat yasuna-quotes | ./command/sample -e -s $SEED128 | ./command/sample -d -s $SEED128 > dest.txt
	diff -c yasuna-quotes dest.txt
	echo "diff: $?"

	return $?
}
