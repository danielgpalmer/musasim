#!/bin/bash

set -x
set -e

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
GDBSRC="${SRCDIR}/gdb-7.3a";

TARGET="m68k-elf"

BINUTILSBUILD="${BUILDDIR}/${TARGET}-binutils"
GCCBUILD="${BUILDDIR}/${TARGET}-gcc"
NEWLIBBUILD="${BUILDDIR}/${TARGET}-newlib"
NEWLIBBUILD="${BUILDDIR}/${TARGET}-gdb"

PREFIX="${INSTDIR}/${TARGET}";

NCPUS="3"

INSTBIN="${PREFIX}/bin"

mkdir -p ${INSTBIN}

PATH="${INSTBIN}:${PATH}"


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


echo "*** BUILDING BINUTILS ***";


if [ ! -e ${BINUTILSTAR} ]; then
	wget -O ${BINUTILSTAR} ${BINUTILSURL};
fi

if [ ! -d ${BINUTILSSRC} ]; then
	cd ${SRCDIR} && tar xzf ${BINUTILSTAR};
fi 

if [ -d ${BINUTILSBUILD} ]; then
	rm -rf ${BINUTILSBUILD};
fi

#mkdir ${BINUTILSBUILD}
#cd ${BINUTILSBUILD}
#${BINUTILSSRC}/configure --target="${TARGET}" --prefix="${PREFIX}"
#make -j "${NCPUS}"
#make install

echo "*** BUILDING INITIAL GCC***"
if [ ! -e ${GCCTAR} ]; then
        wget -O ${GCCTAR} ${GCCURL};
fi
 
if [ ! -d ${GCCSRC} ]; then
        cd ${SRCDIR} && tar xzf ${GCCTAR};
fi 
 
#if [ -d ${GCCBUILD} ]; then
#        rm -rf ${GCCBUILD};
#fi

#mkdir ${GCCBUILD}
#cd ${GCCBUILD}
#${GCCSRC}/configure --target="${TARGET}" --enable-languages=c --with-gnu-as --with-gnu-ld --enable-languages=c --disable-libssp --prefix="${PREFIX}" --disable-shared --with-newlib=yes
#make -j "${NCPUS}"
#make install

echo "*** BUILDING NEWLIB ***"

stageprep $NEWLIBTAR $NEWLIBURL $NEWLIBSRC $NEWLIBBUILD;
cd ${NEWLIBBUILD}
${NEWLIBSRC}/configure --target="${TARGET}" --prefix="${PREFIX}" --disable-newlib-supplied-syscalls
make -j "${NCPUS}"
make install


echo "*** BUILDING FINAL GCC***"

echo "*** BUILDING GDB***"

stageprep $GDBTAR $GDBURL $GDBSRC $GDBBUILD;
