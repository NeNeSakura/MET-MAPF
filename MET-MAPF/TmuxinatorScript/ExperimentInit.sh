#!/bin/sh
#file ExperimentInit.sh

session="ART-MAPF"

if ! command -v tmux 2> /dev/null && ! command -v tmuxinator 2> /dev/null; then
  echo Hey, tmux and tmuxinator are required to run the script.
  echo Call sudo apt install tmux tmuxinator.
  echo Exit.
  exit
fi

if [ $# != 3 ]; then
  echo Seems that the formation of parameters are wrong.
  echo "Usage: $0 <workspace_path> <tested_program> <tmux session name>"
  echo For example, "$0 ../cmake-build-release/ARTTesting ./ARTTesting ART"
  exit
fi

#require tmuxinator
if  [ ! -d "$1" ]  ||  [ ! -n "$2" ] ; then
  echo Seems that the formation of parameters are wrong.
  echo "Usage: $0 <workspace_path> <tested_program> <tmux session name>"
  echo For example, "$0 ../cmake-build-release/ARTTesting ./ARTTesting ART"
  exit
fi


echo Ready to run the script
echo tmuxinator start ART-MAPF -n $3 workspace="$1" program="$2"


tmuxinator start ART-MAPF -n $3 workspace="$1" program="$2"
