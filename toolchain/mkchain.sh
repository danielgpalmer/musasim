#!/bin/bash

set -x
set -e
set -u 

TARGET="$1"

if [ "$TARGET" = "" ]; then
	TARGET="m68000-softfloat-elf";
fi

ROOTDIR=`pwd`
SRCDIR="${ROOTDIR}/src"
TARDIR="${ROOTDIR}/tarballs"
BUILDDIR="${ROOTDIR}/build"
INSTDIR="${ROOTDIR}/inst"

BINUTILSURL="http://ftp.gnu.org/gnu/binutils/binutils-2.22.tar.gz"
GCCURL="http://ftp.gnu.org/gnu/gcc/gcc-4.6.2/gcc-core-4.6.2.tar.gz"
NEWLIBURL="ftp://sources.redhat.com/pub/newlib/newlib-1.20.0.tar.gz"
GDBURL="http://ftp.gnu.org/gnu/gdb/gdb-7.3a.tar.gz"

BINUTILSTAR="${TARDIR}/binutils-2.22.tar.gz"
GCCTAR="${TARDIR}/gcc-core-4.6.2.tar.gz"
NEWLIBTAR="${TARDIR}/newlib-1.20.0.tar.gz"
GDBTAR="${TARDIR}/gdb-7.3a.tar.gz";

BINUTILSSRC="${SRCDIR}/binutils-2.22"
GCCSRC="${SRCDIR}/gcc-4.6.2"
NEWLIBSRC="${SRCDIR}/newlib-1.20.0";
GDBSRC="${SRCDIR}/gdb-7.3";


BINUTILSBUILD="${BUILDDIR}/${TARGET}-binutils"
GCCBUILD="${BUILDDIR}/${TARGET}-gcc"
NEWLIBBUILD="${BUILDDIR}/${TARGET}-newlib"
GDBBUILD="${BUILDDIR}/${TARGET}-gdb"

PREFIX="${INSTDIR}/${TARGET}";

NCPUS="3"
#if [ -x /usr/bin/distcc ]; then
#	NCPUS=`distcc -j`
#	if [ "${NCPUS}" -gt "0" ]; then
#		NCPUS="3"
#	fi
#fi

INSTBIN="${PREFIX}/bin"

mkdir -p ${INSTBIN}

PATH="${INSTBIN}:${PATH}"
TOOLCHAINTAR="${ROOTDIR}/toolchain-${TARGET}.tar.gz"

if [ -e $TOOLCHAINTAR ]; then
	TOOLCHAINSTAMP=`stat -c %Z ${TOOLCHAINTAR}`
	SCRIPTSTAMP=`stat -c %Z ${ROOTDIR}/mkchain.sh`

	if [ "$TOOLCHAINSTAMP" -gt "$SCRIPTSTAMP" ]; then
		echo "Toolchain tar is up to date";
		exit 0;
	fi 
fi;


function stageprep {

	TAR=$1;
	URL=$2;
	SRC=$3;
	BUILD=$4;

	if [ ! -e ${TAR} ]; then 
       		wget -O ${TAR} ${URL};
	fi

	if [ ! -d ${SRC} ]; then 
        	cd ${SRCDIR} && tar xzf ${TAR};
	fi 

	if [ -d ${BUILD} ]; then
        	rm -rf ${BUILD};
	fi

	mkdir ${BUILD}
}

if [ -d $PREFIX ]; then
	rm -r $PREFIX;
fi


REQUIREDPKGS="build-essential libgmp-dev libmpc-dev libmpfr-dev"


for PKG in $REQUIREDPKGS; do
	dpkg -s  $PKG 2>/dev/null | grep Status > /dev/null
	if [ "$?" -ne "0" ]; then
		echo "Build dep $PKG is missing, install it!";
		exit 1;
	fi
done


GCCCONFOPTS="--target=${TARGET} --enable-languages=c --with-gnu-as --with-gnu-ld --enable-languages=c --disable-libssp --prefix=${PREFIX} --disable-shared --with-newlib=yes"

echo "*** BUILDING BINUTILS ***";
stageprep ${BINUTILSTAR} ${BINUTILSURL} ${BINUTILSSRC} ${BINUTILSBUILD}
cd ${BINUTILSBUILD}
${BINUTILSSRC}/configure --target="${TARGET}" --prefix="${PREFIX}"
make -j "${NCPUS}"
make install

echo "*** BUILDING INITIAL GCC***"
stageprep ${GCCTAR} ${GCCURL} ${GCCSRC} ${GCCBUILD}
cd ${GCCBUILD}
${GCCSRC}/configure ${GCCCONFOPTS}
make -j "${NCPUS}"
make -k install

echo "*** BUILDING NEWLIB ***"
stageprep $NEWLIBTAR $NEWLIBURL $NEWLIBSRC $NEWLIBBUILD
cd ${NEWLIBBUILD}
${NEWLIBSRC}/configure --target="${TARGET}" --prefix="${PREFIX}" --disable-newlib-supplied-syscalls
make -j "${NCPUS}"
make install

echo "*** BUILDING FINAL GCC***"
stageprep ${GCCTAR} ${GCCURL} ${GCCSRC} ${GCCBUILD}
cd ${GCCBUILD}
${GCCSRC}/configure ${GCCCONFOPTS}
make -j "${NCPUS}"
make install


echo "*** BUILDING GDB***"
stageprep $GDBTAR $GDBURL $GDBSRC $GDBBUILD
cd ${GDBSRC}
${GDBSRC}/configure --target="${TARGET}" --prefix="${PREFIX}"
make -j "${NCPUS}"
make install

cd $ROOTDIR
tar cpzvf $TOOLCHAINTAR inst
echo "*** ALL DONE! ***";
