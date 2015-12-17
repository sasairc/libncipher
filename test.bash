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

	local A="おいしいおうどん"
	local B=`echo $A | ./sample/sample -e | ./sample/sample -d`

	test $A = $B
	
	return $?
}
