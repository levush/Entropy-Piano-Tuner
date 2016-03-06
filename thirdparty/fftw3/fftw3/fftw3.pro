TEMPLATE = subdirs
SUBDIRS = api dft kernel rdft reodft

api.depends += dft kernel rdft reodft
