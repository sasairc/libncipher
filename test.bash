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

	./command/sample --help
	./command/sample --version

	cat yasuna-quotes | ./command/sample -e | ./command/sample -d > dest.txt
	diff -c yasuna-quotes dest.txt

	cat yasuna-quotes | ./command/sample -e -s "$SEED" | ./command/sample -d -s "$SEED" > dest.txt
	diff -c yasuna-quotes dest.txt

	return $?
}
