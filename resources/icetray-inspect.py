#!/usr/bin/env python
# 
#  $Id$
#  
#  Copyright (C) 2011
#  Jakob van Santen <vansanten@wisc.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#

from optparse import OptionParser
from icecube import icetray, dataclasses
from icecube.icetray import inspect
import re, glob, sys
from os.path import splitext, basename

parser = OptionParser("usage: %prog [options] project1 project2 ...")
parser.add_option('-a', '--all', dest='all',
	help='Examine all projects/libraries in DIRECTORY', metavar='DIRECTORY',
	default=None)
parser.add_option('--no-params', dest='names_only', action='store_true',
	help='Print the names of modules only, not their parameters',
	default=False)
parser.add_option('--no-modules', dest='no_modules', action='store_true',
	help='Skip I3Modules',
	default=False)
parser.add_option('--no-services', dest='no_services', action='store_true',
	help='Skip ServiceFactories',
	default=False)

opts, args = parser.parse_args()
if len(args) == 0 and opts.all is None:
	parser.print_help()
	parser.exit(1)

if opts.all:
	args += [splitext(basename(fname))[0][3:] for fname in glob.glob('%s/lib*' % opts.all)]

bad_services = ['I3TrayInfoServiceFactory'] # Services that crash on instantiation
bad_libs = ['corsikaXX', 'xppc', 'ppc'] # Libraries that segfault or rudely call exit()

modcount = 0
servicecount = 0

# keep our output clean.
icetray.disable_logging() 

def print_config(config):
	desc = config.descriptions
	if len(config.keys()) > 0:
		for k in config.keys():
			print '    %s' % k
			print '      Description : %s' % desc[k]
			print '      Default     : %s' % config[k]
			print ''
	else:
		print '    (No parameters)'
	print '  ' + '-'*77

def display_config(mod, category):
		print '  %s (%s)' % (mod, category)
		
		try:
			config = inspect.get_configuration(mod)
		except RuntimeError as e:
			sys.stderr.write("Error constructing '%s': %s" % (mod, e))
			return False
			
		if not opts.names_only:
			print_config(config)

		return True

for project in args:
	
	if project in bad_libs:
		continue
	
	print '*** %s ***' % project
	
	try:
		modname = project.replace('-','_')
		module = __import__('icecube.%s' % modname, fromlist=[modname])
		py_modules = icetray.inspect.harvest_subclasses(module)
	except ImportError:
		try:
			icetray.load(project)
			py_modules = []
		except RuntimeError as e:
			sys.stderr.write("Ignoring '%s': %s" % (project, e))
			continue
		
	cxx_modules = icetray.modules(project)
	cxx_services = icetray.services(project)

	print '-'*79

	if not opts.no_modules:
		for mod in cxx_modules:
			if display_config(mod, 'C++ I3Module'):
				modcount += 1 
		for mod in py_modules:
			if display_config(mod, 'Python I3Module'):
				modcount += 1
	if not opts.no_services:
		for mod in cxx_services:
			if mod in bad_services:
				continue
			if display_config(mod, 'C++ ServiceFactory'):
				servicecount += 1
	
print ''
print '%d module and %d service configurations reported.' % (modcount, servicecount)