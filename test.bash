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

	SEED="ほげふがおうどんqwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"

	wget https://raw.githubusercontent.com/sasairc/yasuna/master/quotes/yasuna-quotes

	./sample/sample --help
	./sample/sample --version

	cat yasuna-quotes | ./sample/sample -e | ./sample/sample -d > dest.txt
	diff -c yasuna-quotes dest.txt

	cat yasuna-quotes | ./sample/sample -e "$SEED" | ./sample/sample -d -s "$SEED" > dest.txt
	diff -c yasuna-quotes dest.txt

	return $?
}
