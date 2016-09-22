PUBLISH_ALGORITHMS += entropyminimizer pitchraise resettorecording
TEST_ALGORITHMS += examplealgorithm

ALGORITHM_NAMES += $$PUBLISH_ALGORITHMS

CONFIG(debug, debug|release) {
    ALGORITHM_NAMES += $$TEST_ALGORITHMS
}
