#!python

#=============================================================================
# T-PoT - Total Commander file system plug-in for iPod and iPhone devices
#-----------------------------------------------------------------------------
# File:			publish.py
# Purpose:		Compiles the T-PoT plug-in and creates a distribution package.
# Limitations:	Supports Visual Studio 2003 or 2005 (easy to add 2008 but has
#				not been tested)
# Platform:		Win32
#-----------------------------------------------------------------------------
# Usage: python publish.py -h 
#
# Example: compiling and creating the distribution packages with version 1.1:
#
#   python publish.py -b -p -v 1.1
#
# => Creates the "T-PoT.1.1.2003.zip" and "T-PoT.1.1.2005.zip" plug-ins.
#
# If one of the Visual Studio version is not installed, the corresponding 
# step will be skipped.
#-----------------------------------------------------------------------------
# Copyright (c) 2007-2009, Scythal
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions are met:
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# * Neither the name of the software nor the names of its contributors may be 
#   used to endorse or promote products derived from this software without 
#   specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY ITS AUTHOR ``AS IS'' AND ANY EXPRESS OR 
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN 
# NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================

import os, re, zipfile, sys, optparse

def buildProjects(target, dir, solution, projects, logFile = None):
	'''
	Compiles a list of projects of a solution.
	- target is the tool version (2003, 2005 or 2008)
	- dir is the directory of the solution file
	- projects is the list of the projects to compile
	- logFile is an optional output log, this file must be open for writing
	OSError() Exceptions are raised in case of problem.
	Uses compile.bat to launch the compiler with the proper environment.
	'''
	p = os.popen(r"compile.bat -vs %s %s %s %s" % (target, dir, solution, " ".join(projects)))
	regLog = re.compile("(Build:|Clean:)")
	for logline in p:
		if logFile:
			logFile.write(logline)
		result = regLog.search(logline)
		if result != None:
			print(logline.strip())
	if p.close():
		raise OSError("Fatal error in compilation\nCheck the %s file\n" % logFilename)

def compileBinaries(version, target):
	'''Compiles the T-PoT plug-ins.'''
	logFile = open("compile.%s.%s.log" % (version, target), "wt")
	try:
		ext = "" if target == "2005" else "_2003"
		buildProjects(target, "..", "T-PoT%s" % ext, ["T-PoT"], logFile)
		print("Compiled version %s on target %s" % (version, target))
	finally:
		logFile.close()

def makePackage(version, target):
	'''Creates the plug-in packages for Total Commander.'''
	zipFilename = "T-PoT.%s.%s.zip" % (version, target)
	z = zipfile.ZipFile(zipFilename, "w", compression=zipfile.ZIP_DEFLATED)
	z.write("pluginst.inf")
	z.write("README.txt")
	z.write(target + "/T-PoT.wfx", "T-PoT.wfx")
	z.close()
	print("Created package " + zipFilename)

if __name__ == "__main__":
	# Fix missin COMSPEC on some Windows installations:
	if (sys.platform == "win32") and not os.environ.get('COMSPEC', None):
		os.environ['COMSPEC'] = path.join(os.environ['SYSTEMROOT'], "System32", "cmd.exe")
		
	# Parses the arguments:
	opt = optparse.OptionParser(
		usage = "%prog [<options>]\n\n"
				"Builds the binaries and the distribution package for the T-PoT.\n")
	opt.add_option("--build", "-b", action="store_true", default=False, help="builds the binaries")
	opt.add_option("--package", "-p", action="store_true", default=False, help="builds the package")
	opt.add_option("-v", default="0.x", help="version")
	options, args = opt.parse_args()

	# Default targets:
	targets = ["2003", "2005"]
	
	for target in targets:
		print("=" * 80 + "\nTarget: %s\n" % target)
		try:
			if options.build:
				# Compiles the sources:
				compileBinaries(options.v, target)
			if options.package:
				# Creates the .zip plug-in package:
				makePackage(options.v, target)
		except:
			print("Target %s failed." % target)
		print ("-" * 80)
	print("Done.")
