usage()
{
  cat <<EOF
Usage: $0 [options]
Options:
  --help       print this message
EOF
  exit 1
}

echo_exit()
{
  echo $1
  exit 1
}

for a in $@; do
  if echo $a | grep "^--help" > /dev/null 2> /dev/null; then
    usage
  fi
done

if [ -z $KARABO ]; then
    echo "\$KARABO is not defined. Make sure you have sourced the activate script for the Karabo Framework which you would like to use."
    exit 1
else
    LOCALKARABOVERSION=$(cat $KARABO/VERSION)
    if [ "$LOCALKARABOVERSION" == "$KARABOVERSION" ]; then
       installDir=$KARABO/extern
    else
       echo "Plugin was compiled with different karaboFramework version"
       echo "than installed one: $KARABOVERSION vs. $LOCALKARABOVERSION"
       echo " "
    fi

fi
if [ ! -d $installDir ]; then
    mkdir -p $installDir || echo_exit "Cannot create directory $installDir"
fi

echo "This is a self-extracting archive."
echo -n " Extracting files, please wait..."
# searches for the line number where finish the script and start the tar.gz
SKIP=`awk '/^__TARFILE_FOLLOWS__/ { print NR + 1; exit 0; }' $0`
tail -n +$SKIP $0 | (cd  $installDir && tar xzf -) || echo_exit "Problem unpacking the file $0"
echo  " unpacking finished successfully"
# Any script here will happen after the tar file extract.
echo
echo -n "Running post install script..."
echo " done."

# Patch aravis' pkgconfig
for fn in `find ${installDir} -name aravis-*.pc`; do
    sed -i "s|^prefix=.*$|prefix=${installDir}|" ${fn}
done

echo
echo " Package was successfully installed to: $installDir"
echo
echo
exit 0
# NOTE: Don't place any newline characters after the last line below.
__TARFILE_FOLLOWS__
