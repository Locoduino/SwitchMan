#!/usr/bin/python
import sys, os
sys.path.append('../../../python-makefile')
import makefile

#--- Change dir to script absolute path
scriptDir = os.path.dirname (os.path.abspath (sys.argv[0]))
os.chdir (scriptDir)
#--- Get goal as first argument
goal = "all"
if len (sys.argv) > 1 :
  goal = sys.argv [1]
#--- Get max parallel jobs as second argument
maxParallelJobs = 0 # 0 means use host processor count
if len (sys.argv) > 2 :
  maxParallelJobs = int (sys.argv [2])
#--- Build python makefile
make = makefile.Make (goal, maxParallelJobs == 1) # Display executable if sequential build
# make.mMacTextEditor = "Atom"
sourceList = [
    "dom.cpp",
    "../../src/TrackSet.cpp",
    "../../src/Track.cpp",
    "../../src/PathSet.cpp",
    "../../src/HeadedTrackSet.cpp",
    "../../unix/Arduino.cpp",
    "../../unix/HardwareSerial.cpp"
]
objectList = []
for source in sourceList:
#--- Add compile rules
  src = os.path.basename(os.path.dirname(source)) + "/" + os.path.basename(source)
  object = "objects/" + src + ".o"
  depObject = object + ".dep"
  objectList.append (object)
  rule = makefile.Rule ([object], "Compiling " + source) # Release 2
  rule.deleteTargetDirectoryOnClean ()
  rule.mDependences.append (source)
  rule.mCommand.append ("g++")
#  rule.mCommand += ["-std=c++11"]
  rule.mCommand += ["-I../../src"]
  rule.mCommand += ["-I../../unix"]
  rule.mCommand += ["-I../../examples/dom"]
  rule.mCommand += ["-c", source]
  rule.mCommand += ["-o", object]
  rule.mCommand += ["-MD", "-MP", "-MF", depObject]
  rule.enterSecondaryDependanceFile (depObject, make)
  rule.mPriority = os.path.getsize (scriptDir + "/" + source)
#  rule.mOpenSourceOnError = True
  make.addRule (rule)
#--- Add linker rule
product = "dom"
mapFile = product + ".map"
rule = makefile.Rule ([product, mapFile], "Linking " + product) # Release 2
rule.mDeleteTargetOnError = True
rule.deleteTargetFileOnClean ()
rule.mDependences += objectList
rule.mCommand += ["g++"]
rule.mCommand += objectList
rule.mCommand += ["-o", product]
rule.mCommand += ["-Wl,-map," + mapFile]
postCommand = makefile.PostCommand ("Stripping " + product)
postCommand.mCommand += ["strip", "-A", "-n", "-r", "-u", product]
rule.mPostCommands.append (postCommand)
make.addRule (rule)
#--- Print rules
# make.printRules ()
# make.writeRuleDependancesInDotFile ("make-deps.dot")
make.checkRules ()
#--- Add goals
make.addGoal ("all", [product, mapFile], "Building all")
make.addGoal ("compile", objectList, "Compile C files")
#make.simulateClean ()
#make.printGoals ()
#make.doNotShowProgressString ()
make.runGoal (maxParallelJobs, maxParallelJobs == 1)
#--- Build Ok ?
make.printErrorCountAndExitOnError ()
