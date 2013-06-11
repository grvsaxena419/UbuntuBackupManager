#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#       create_test_env.py
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

import os;
import sys;
import random;
import base64;
import string;

total_files=0;
total_subd=0;


def create_files(folder,num,size):
	global total_files;
	for i in range(0,num):		
		try:
			f_name=''.join(random.choice(string.letters + string.digits) for x in range(32))
			print "Creating file "+f_name;
			
			r=random.randint(0,1);
			if r==0:
				os.system("dd if=/dev/urandom of="+str(f_name)+" bs="+str(size)+" count=1024 >/dev/null 2>&1");
			else:
				fil=open(f_name,"w");
				strng="This is a test file! Created by backupfs\n";
				for i in range(0,num*size/len(strng)):
					print >>fil,strng;
				fil.close();
			total_files=total_files+1;
		except Exception, e:
			print "Error %s"%e.args;
				
def create_subfolders(folder,num,level, max_level):
	if level >= max_level:
		return;
	global total_subd;
	print "Creating files in ",folder;
	
	os.chdir(folder);

	create_files(folder,2*num,1024);
	
	print "Creating subdirectories in ",folder;
		
	for i in range(0,num):
		try:
			print "Creating folder "+folder+str(i);
			os.mkdir(folder+str(i));
			total_subd=total_subd+1;
			r=random.randint(0, 1);
			if r==1:
				n=random.randint(0,num);
				create_subfolders(folder+str(i),n,level+1,max_level);
		except Exception, e:
			print "Error %s %d"%e.message,e.args;
	os.chdir("../");
		
def create_test_env(folder_name):
	print "Removing older files of test bed";
	os.system("rm -rf "+folder_name);
	print "Preparing Test bed for tests";
	try:
		os.mkdir(folder_name);
		print "Created folder "+folder_name+" for testing";
	except Exception, e:
		print "Error %s:"%e.args;
	
	print "Creating files and subdirectories in"+folder_name;
	os.chdir(folder_name);
	os.chdir("../");
	rel_foldername=os.path.basename(folder_name);	
	create_subfolders(rel_foldername,7,0,2);
	print str(total_subd)+" Directories created "+str(total_files)+" files created";

	return 0

if __name__ == '__main__':
	sys.exit(-1);

