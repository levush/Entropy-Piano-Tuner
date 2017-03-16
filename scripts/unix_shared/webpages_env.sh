# setup server configuration

#export SERVER_USERNAME=u81468700
#export SERVER_ADDRESS=s582786137.online.de
#export _SERVER_ROOT_DIR=download

export SERVER_USERNAME=hinrichsen
export SERVER_ADDRESS=eptwebpages
export _SERVER_ROOT_DIR=public_html/ept

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

# set permissions
########################################
function fix_permissions {
	local remote_path=$1
	ssh $SERVER_USERNAME@$SERVER_ADDRESS "chmod 755 -R $_SERVER_ROOT_DIR/$remote_path"
}
# mkdir REMOTE_PATH
########################################

# using ssh
function ssh_mkdir {
	local remote_path=$1
	ssh $SERVER_USERNAME@$SERVER_ADDRESS "mkdir -p $_SERVER_ROOT_DIR/$remote_path"
}

# using sftp
function sftp_mkdir {
	local remote_path=$1
	echo "Creating remote directory at: $remote_path"
	sftp $SERVER_USERNAME@$SERVER_ADDRESS <<EOF
cd $_SERVER_ROOT_DIR
mkdir $remote_path
EOF
}


# push LOCAL_FILE REMOTE_PATH
########################################

# using rsync
function rsync_push {
	local local_file=$1
	local remote_path=$SERVER_USERNAME@$SERVER_ADDRESS:$_SERVER_ROOT_DIR/$2
	rsync -vh $local_file $remote_path
}

# using sftp
function sftp_push {
	local local_file=$1
	local remote_path=$2

	sftp $SERVER_USERNAME@$SERVER_ADDRESS <<EOF
cd $_SERVER_ROOT_DIR
cd $remote_path
put $local_file
EOF
}

# push_dir LOCAL_DIR REMOTE_PATH

# using rsync
function rsync_push_dir {
	local local_dir=$1
	local remote_path=$SERVER_USERNAME@$SERVER_ADDRESS:$_SERVER_ROOT_DIR/$2
	rsync -vh -r -p $local_dir $remote_path
	# ssh $SERVER_USERNAME@$SERVER_ADDRESS "chmod -R 755 $_SERVER_ROOT_DIR/$2"
}

# using sftp
function sftp_push_dir {
	local local_dir=$1
	local remote_path=$2
	
	sftp_mkdir $_SERVER_ROOT_DIR/$remote_path
	
	cd $local_dir
	for file in *
	do
		echo "File $file"
		[ -f $file ] && sftp_push $file $remote_path
		[ -d $file ] && sftp_push_dir $local_dir/$file $remote_path/$file
	done
	
	cd ..
}

# ln REMOTE_PATH_ORIGIN REMOTE_PATH_TARGET FILENAME
########################################
function ssh_ln {
	ssh $SERVER_USERNAME@$SERVER_ADDRESS "rm -f $_SERVER_ROOT_DIR/$2/$3"
	ssh $SERVER_USERNAME@$SERVER_ADDRESS "ln $_SERVER_ROOT_DIR/$1/$3 $_SERVER_ROOT_DIR/$2"
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
	#sftp_mkdir $1
	ssh_mkdir $1
}

function server_push {
	#sftp_push $1 $2
	rsync_push $1 $2
}

function server_push_dir {
	#sftp_push_dir $1 $2
	rsync_push_dir $1 $2
}

function server_ln {
	#sftp_ln $1 $2 $3
	ssh_ln $1 $2 $3
}

# create directory structure
#########################################
echo "Creating remote directories. Note that these commands may fail if the remote directories already exist (when using sftp)"

server_mkdir /
server_mkdir $_SERVER_DOWNLOADS_DIR
server_mkdir $_SERVER_DOWNLOADS_DIR/$versionString
server_mkdir $_SERVER_DOWNLOADS_DIR/$depsVersionString
server_mkdir $_SERVER_DOWNLOADS_DIR/dependencies
server_mkdir $_SERVER_DEPS_VERSION_DIR

echo "Done."
