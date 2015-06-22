# setup server configuration
export SERVER_USERNAME=hinrichsen
export SERVER_ADDRESS=webpages.physik.uni-wuerzburg.de
export _SERVER_ROOT_DIR=public_html/ept
export _SERVER_DOWNLOADS_DIR=Resources/Public/Downloads

# load version
. ./loadVersion.sh

# define further usefull paths
export SERVER_DOWNLOADS_DIR=$SERVER_ADDRESS:$_SERVER_ROOT_DIR/$_SERVER_DOWNLOADS_DIR

export US_DOWNLOADS=$SERVER_USERNAME@$SERVER_DOWNLOADS_DIR
export US_REPOSITORY=$SERVER_USERNAME@$SERVER_DOWNLOADS_DIR/Repository

export US_BINARY=$US_DOWNLOADS/$versionString
export US_DEPENDENCIES=$US_DOWNLOADS/dependencies/$depsVersionString

# create directories if missing
ssh $SERVER_USERNAME@$SERVER_ADDRESS "mkdir -p $_SERVER_ROOT_DIR/$_SERVER_DOWNLOADS_DIR/$versionString"
ssh $SERVER_USERNAME@$SERVER_ADDRESS "mkdir -p $_SERVER_ROOT_DIR/$_SERVER_DOWNLOADS_DIR/dependencies/$depsVersionString"