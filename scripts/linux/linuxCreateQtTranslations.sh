set -e
cd ${0%/*}
# load environment
. ./linux_env.user.sh

translationsdir="$TUNER_BASE_DIR/translations"

function convertLang {
	if [ -f $QT_LANG_DIR/qtbase_$1.qm ];
	then
		$QTDIR/bin/lconvert -o $translationsdir/qt_$1.qm $QT_LANG_DIR/qtbase_$1.qm $QT_LANG_DIR/qt_$1.qm
	else
		echo "Warning: Language $1 not translated by qt"
		$QTDIR/bin/lconvert -o $translationsdir/qt_$1.qm $QT_LANG_DIR/qt_$1.qm
	fi
}

convertLang "de"
convertLang "pl"
convertLang "es"
convertLang "ru"
convertLang "pt"
convertLang "ko"
