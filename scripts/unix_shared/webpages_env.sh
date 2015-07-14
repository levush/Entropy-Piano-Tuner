# setup server configuration

export SERVER_USERNAME=u81468700-download
export SERVER_ADDRESS=s582786137.online.de
export _SERVER_ROOT_DIR=

#export SERVER_USERNAME=hinrichsen
#export SERVER_ADDRESS=webpages.physik.uni-wuerzburg.de
#export _SERVER_ROOT_DIR=public_html/ept

# load version
. ./loadVersion.sh

# remote dirs as locations for uploading files
export _SERVER_DOWNLOADS_DIR=Resources/Public/Downloads
export _SERVER_REPOSITORY_DIR=$_SERVER_DOWNLOADS_DIR/Repository
export _SERVER_VERSION_DIR=$_SERVER_DOWNLOADS_DIR/$versionString
export _SERVER_DEPS_VERSION_DIR=$_SERVER_DOWNLOADS_DIR/dependencies/$depsVersionString

# define further usefull paths
export SERVER_DOWNLOADS_DIR=$SERVER_ADDRESS:$_SERVER_ROOT_DIR/$_SERVER_DOWNLOADS_DIR

export US_DOWNLOADS=$SERVER_USERNAME@$SERVER_DOWNLOADS_DIR
export US_REPOSITORY=$SERVER_USERNAME@$SERVER_DOWNLOADS_DIR/Repository

export US_BINARY=$US_DOWNLOADS/$versionString
export US_DEPENDENCIES=$US_DOWNLOADS/dependencies/$depsVersionString

# define functions to create a directory and uploading a file

# mkdir REMOTE_PATH
########################################

# using ssh
function ssh_mkdir {
	remote_path=$1
	ssh $SERVER_USERNAME@$SERVER_ADDRESS "mkdir -p $_SERVER_ROOT_DIR$remote_path"
}

# using sftp
function sftp_mkdir {
	remote_path=$1
	sftp $SERVER_USERNAME@$SERVER_ADDRESS <<EOF
mkdir $remote_path
EOF
}


# push LOCAL_FILE REMOTE_PATH
########################################

# using rsync
function rsync_push {
	local_file=$1
	remote_path=$2
	rsync -vh $local_file $remote_path
}

# using sftp
function sftp_push {
	local_file=$1
	remote_path=$2

	sftp $SERVER_USERNAME@$SERVER_ADDRESS <<EOF
cd $_SERVER_ROOT_DIR
cd $remote_path
put $local_file
EOF
}

# push_dir LOCAL_DIR REMOTE_PATH

# using rsync
function rsync_push_dir {
	local_dir=$1
	remote_path=$2
	rsync -vh -r $local_dir $remote_path
}

# using sftp
function sftp_push_dir {
	local_dir=$1
	remote_path=$2

	sftp $SERVER_USERNAME@$SERVER_ADDRESS <<EOF
mkdir $remote_path
cd $_SERVER_ROOT_DIR
cd $remote_path
put -r $local_dir
EOF
}

# ln REMOTE_PATH_ORIGIN REMOTE_PATH_TARGET FILENAME
########################################
function ssh_ln {
	ssh $SERVER_USERNAME@$SERVER_ADDRESS "rm -f $2/$3"
	ssh $SERVER_USERNAME@$SERVER_ADDRESS "ln $1/$3 $2"
}

function sftp_ln {
	# remove old link is important
	sftp $SERVER_USERNAME@$SERVER_ADDRESS <<EOF
cd $_SERVER_ROOT_DIR/$2
rm $3
EOF

	# create ln
	sftp $SERVER_USERNAME@$SERVER_ADDRESS <<EOF
cd $_SERVER_ROOT_DIR
ln $1/$3 $2/$3
EOF
}

# choose the function to use
#########################################
function server_mkdir {
	sftp_mkdir $1
}

function server_push {
	sftp_push $1 $2
}

function server_push_dir {
	sftp_push_dir $1 $2
}

function server_ln {
	sftp_ln $1 $2 $3
}

# create directory structure
#########################################
echo "Creating remote directories. Note that these commands may fail if the remote directories already exist (when using sftp)"

server_mkdir $_SERVER_ROOT_DIR
server_mkdir $_SERVER_ROOT_DIR/$_SERVER_DOWNLOADS_DIR
server_mkdir $_SERVER_ROOT_DIR/$_SERVER_DOWNLOADS_DIR/$versionString
server_mkdir $_SERVER_ROOT_DIR/$_SERVER_DOWNLOADS_DIR/$depsVersionString

echo "Done."
