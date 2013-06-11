#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#       untitled.py
#       
#       Copyright 2012 gs <gs@gs-laptop>
#       
#       This program is free software; you can redistribute it and/or modify
#       it under the terms of the GNU General Public License as published by
#       the Free Software Foundation; either version 2 of the License, or
#       (at your option) any later version.
#       
#       This program is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#       GNU General Public License for more details.
#       
#       You should have received a copy of the GNU General Public License
#       along with this program; if not, write to the Free Software
#       Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#       MA 02110-1301, USA.
#       
#       

import sys;
import os;
import create_test_env;
import execute_tests;


def main(folder_name, code_path):
	if(os.getuid()!=0):
		print "You need to run this test as root!";
		return -1;
	print "Running from "+code_path;
	create_test_env.create_test_env(folder_name);	
	raw_input("Press enter to start executing tests");
	
	execute_tests.execute_tests(folder_name , code_path);
	return 0;

if __name__ == '__main__':
	if(len(sys.argv)<2):
		print sys.argv[0]," <testfolder-name>";
		sys.exit(-1);
	path = os.path.abspath(sys.argv[0]);
	code_path = os.path.dirname(os.path.dirname(path));
	main(sys.argv[1],code_path);

