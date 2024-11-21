#!/bin/bash

if [ -z $KARABO ]; then
  echo "\$KARABO is not defined. Make sure you have sourced the activate script for the Karabo Framework you would like to use."
  exit 1
else
    KARABOVERSION=$(cat $KARABO/VERSION)
fi
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
# "karabo install" expects the installation directory here
BUILD_DIR=$SCRIPTPATH/build
DISTDIR=$SCRIPTPATH/localdist
INSTALL_PREFIX=$KARABO/extern

MACHINE=$(uname -m)
OS=$(uname -s)
source "$KARABO/bin/.set_lsb_release_info.sh"
if [ "$OS" = "Linux" ]; then
    DISTRO_ID=( $LSB_RELEASE_DIST )
    DISTRO_RELEASE=$(echo $LSB_RELEASE_VERSION | sed -r "s/^([0-9]+).*/\1/")
fi

DEPVERSION=$(git rev-parse --short HEAD)

DEPNAME=`basename $SCRIPTPATH`
PACKAGENAME=$DEPNAME-$DEPVERSION-$KARABOVERSION
EXTRACT_SCRIPT=${SCRIPTPATH}/.extract.sh
INSTALLSCRIPT=${PACKAGENAME}-${DISTRO_ID}-${DISTRO_RELEASE}-${MACHINE}.sh
PACKAGEDIR=${SCRIPTPATH}/package

# Always clean the bundle
rm -rf $DISTDIR
rm -rf $PACKAGEDIR
# Start fresh
mkdir -p $DISTDIR/lib
mkdir -p $DISTDIR/include/image_source
mkdir -p $BUILD_DIR
mkdir -p $PACKAGEDIR

# handle the make -j option from the caller.
BUILD_OPT="--build ."
unset MAKEFLAGS
if [[ "${2}" != "" ]]; then
  BUILD_OPT="--build . -j ${2}"
# or use all but 4 procs on system
# (minimum nproc is always 1)
elif command -v nproc &> /dev/null; then
  NPROC=$(nproc --all --ignore=4)
  BUILD_OPT="--build . -j${NPROC}"
fi

cmake \
    -DCMAKE_BUILD_TYPE=$1 \
    -DBUILD_TESTS=0 \
    -DBoost_NO_BOOST_CMAKE=ON \
    -DBoost_NO_SYSTEM_PATHS=ON \
    -DCMAKE_PREFIX_PATH=$KARABO/extern \
    -DCMAKE_INSTALL_PREFIX=$DISTDIR \
    -B $BUILD_DIR .
cd $BUILD_DIR
cmake $BUILD_OPT
$(cp $BUILD_DIR/imageSource/lib*.so $DISTDIR/lib/. | true)
$(cp $SCRIPTPATH/src/*.hh  $DISTDIR/include/image_source/. | true)
$(patchelf --force-rpath --set-rpath '$ORIGIN/../../lib:$ORIGIN/../lib64:$ORIGIN' $DISTDIR/lib/libimageSource.so)

############# packaging and installing of dependency to karabo ################

# Packaging
cd ${PACKAGEDIR}
tar -zcf ${PACKAGENAME}.tar.gz -C ${DISTDIR} .

# Create installation script
echo -e '#!/bin/bash\n'"VERSION=$DEPVERSION\nDEPNAME=$DEPNAME\nKARABOVERSION=$KARABOVERSION" | cat - $EXTRACT_SCRIPT ${PACKAGENAME}.tar.gz > $INSTALLSCRIPT
chmod a+x $INSTALLSCRIPT
echo
echo "Created package: ${PACKAGEDIR}/$INSTALLSCRIPT"
echo

# Use installation script to install dependency in Karabo
echo -e "\n### Installing $DEPNAME in $INSTALL_PREFIX"
./$INSTALLSCRIPT --prefix=$INSTALL_PREFIX
echo -e "\n\n**** Installed $DEPNAME to $INSTALL_PREFIX"

exit 0
