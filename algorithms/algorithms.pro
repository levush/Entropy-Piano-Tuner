include(algorithms_func.pri)

TEMPLATE = subdirs

SUBDIRS += entropyminimizer pitchraise resettorecording

contains(EPT_CONFIG, include_example_algorithm) {
    SUBDIRS += examplealgorithm
}
