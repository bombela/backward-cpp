#!/bin/bash

COMPILERS_CXX98=`cat<<EOF
gcc-4.4
gcc-4.5
gcc-4.6
gcc-4.8
clang
EOF`

COMPILERS_CXX11=`cat<<EOF
gcc-4.8
clang
EOF`

function mkbuild() {
	local compiler=$1
	local lang=$2
	local buildtype=$3
	local builddir="build_$1_$2"
	export CC=$compiler
	export CXX=`echo $compiler | sed -e 's/clang/clang++/' -e 's/gcc/g++/'`
	export CXXFLAGS="-std=$lang"
	echo "creating $builddir"
	mkdir $builddir 2>/dev/null
	(
		cd "$builddir"
		cmake -DCMAKE_BUILD_TYPE=$buildtype ..
	)
}

function build() {
	local builddir=$1
	shift
	make -C "$builddir" $@
}

function dotest() {
	local builddir=$1
	shift
	make -C "$builddir" test $@
	return 0
}

action=$1
shift

if [[ $action == "cmake" ]]; then
	buildtype=$1
	shift
	for compiler in $COMPILERS_CXX98; do
		mkbuild $compiler c++98 $buildtype
		[[ $? != 0 ]] && exit
	done
	for compiler in $COMPILERS_CXX11; do
		mkbuild $compiler c++11 $buildtype
		[[ $? != 0 ]] && exit
	done
elif [[ $action == "make" ]]; then
	for compiler in $COMPILERS_CXX98; do
		build "build_${compiler}_c++98" $@
		[[ $? != 0 ]] && exit
	done
	for compiler in $COMPILERS_CXX11; do
		build "build_${compiler}_c++11" $@
		[[ $? != 0 ]] && exit
	done
elif [[ $action == "test" ]]; then
	for compiler in $COMPILERS_CXX98; do
		dotest "build_${compiler}_c++98" $@
		[[ $? != 0 ]] && exit
	done
	for compiler in $COMPILERS_CXX11; do
		dotest "build_${compiler}_c++11" $@
		[[ $? != 0 ]] && exit
	done
elif [[ $action == "clean" ]]; then
	for compiler in $COMPILERS_CXX98; do
		rm -r "build_${compiler}_c++98"
	done
	for compiler in $COMPILERS_CXX11; do
		rm -r "build_${compiler}_c++11"
	done
fi
