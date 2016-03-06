TEMPLATE = subdirs
SUBDIRS += scalar r2cf r2r r2cb


scalar.depends = r2cf r2r r2cb
