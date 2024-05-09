#!/bin/sh
#file FastCall.sh


echo call ExperimentInit.sh
echo e.g. ./FastCall.sh ../cmake-build-release


#./ExperimentInit.sh ../cmake-build-release/RandomTesting ./RandomTesting "RT"
#./ExperimentInit.sh ../cmake-build-release/AdaptiveRandomTesting ./AdaptiveRandomTesting "ART"
#./ExperimentInit.sh ../cmake-build-release/MetamorphicTesting ./MetamorphicTesting "MT"
./ExperimentInit.sh "$1"/RandomTesting ./RandomTesting "RT" &
./ExperimentInit.sh "$1"/AdaptiveRandomTesting ./AdaptiveRandomTesting "ART" &
./ExperimentInit.sh "$1"/MetamorphicTesting ./MetamorphicTesting "MT" &

echo Expeirments are executed, please use tmux attach -t <session-name> to check the execution process
echo Press Enter to get back.
