# load webpages environment
. ./webpages_env.sh

cd $TUNER_BASE_DIR

# create dependencies.tar.bz2 and .zip
tar -cvjSf $TUNER_BASE_DIR/publish/dependencies.tar.bz2 dependencies
zip -r     $TUNER_BASE_DIR/publish/dependencies.zip dependencies

server_push $TUNER_BASE_DIR/publish/dependencies.tar.bz2 $_SERVER_DEPS_VERSION_DIR
server_push $TUNER_BASE_DIR/publish/dependencies.zip $_SERVER_DEPS_VERSION_DIR
