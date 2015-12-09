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
	rm CC="$1" PREFIX="$2" install && \
		tree "$2"

	return $?
}

function run_static_bin() {
	echo "*** run ***"

	make sample

	local A="おうどん"
	local B=`echo $A | ./src/sample -e | ./src/sample -d`

	test $A = $B
	
	return $?
}
