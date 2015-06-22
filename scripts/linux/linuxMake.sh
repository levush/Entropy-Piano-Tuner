###########################################################
# this file will make the program
###########################################################

# automatic fail on errors
set -e

cd ${0%/*}

# load the user environment
. ./linux_env.user.sh

# if -c is provided, we clear the build first
# if -q is provided, we create qt translations
while getopts ":cq" opt; do
  case $opt in
    c)
      echo "Clearing build at $BUILD_DIR."
      rm -rf $BUILD_DIR
      ;;
    q)
      echo "Creating qt translations"
      $SCRIPT_DIR/linuxCreateQtTranslations.sh
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit -1
      ;;
  esac
done

# preparation
###########################################################

# create the output dir
mkdir -p $PUBLISH_DIR

# remove the old run file
rm -f ${RUN_FILE}

echo "Creating temporary build directory"
mkdir -p $BUILD_DIR

echo "lrelease translations"
cd $TUNER_BASE_DIR/translations
for f in *.ts; do lrelease $f; done

cd $BUILD_DIR

# Build
###########################################################
qmake $TUNER_BASE_DIR/entropytuner.pro -r -spec linux-g++ DEFINES+="CONFIG_ENABLE_UPDATE_TOOL"
make $MAKE_ARGS

echo "Build finished"
