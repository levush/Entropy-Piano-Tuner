# run this file to automatically build a rpm package (fedora)
# I refer to https://fedoraproject.org/wiki/How_to_create_an_RPM_package
# for a first setup.
# Follow the instructions to build the desired structure tree (rpmdev-setuptree)

set -e

RPMBUILD_DIR=~/rpmbuild
SPECS_DIR=$RPMBUILD_DIR/SPECS
SOURCE_DIR=$RPMBUILD_DIR/SOURCES
RPMS_DIR=$RPMBUILD_DIR/RPMS

if [ ! -d "$RPMBUILD_DIR" ]; then
    echo "rpm build dir could not be located at $RPMBUILD_DIR."
    exit -1
fi

if [ ! -d "$SPECS_DIR" ]; then
    echo "specs dir could not be located at $SPECS_DIR."
    exit -1
fi

if [ ! -d "$SOURCE_DIR" ]; then
    echo "source dir could not be located at $SOURCE_DIR."
    exit -1
fi


BINARY_NAME=entropypianotuner
SPEC_FILENAME=$BINARY_NAME.spec

# get current version
cd unix_shared
. ./loadVersion.sh

# the git archive has to match the one from the specs file!
ARCHIVE_NAME=archive.tar.gz
GIT_ARCHIVE=$ARCHIVE_NAME?ref=v${versionString}
GIT_OWNER=entropytuner
GIT_PROJECT=Entropy-Piano-Tuner
GIT_SOURCE=https://gitlab.com/$GIT_OWNER/$GIT_PROJECT/repository/$GIT_ARCHIVE

# setup required files (spec file and fetch source)
cp $TUNER_BASE_DIR/scripts/fedora/$SPEC_FILENAME $SPECS_DIR

cd $SOURCE_DIR
wget $GIT_SOURCE
mv $GIT_ARCHIVE $ARCHIVE_NAME


# build
cd $SPECS_DIR
rpmbuild -ba $SPEC_FILENAME
echo "Running rpmlint, there may not occur errors or warnings!"
rpmlint $SPEC_FILENAME ../*/$BINARY_NAME*.rpm ../SRPMS/$BINARY_NAME*.rpm
echo "Runnung rpmls, there may not occur error or warnings! (checking permissings)"
cd $RPMS_DIR
rpmls *.rpm

echo Done.
