#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#       execute_tests.py
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
import time;
path="/home/gs/finalyrprj/project_integrated/";

def prepare_test_run():
	global path;
	print "Compiling backupfs.......";
	os.chdir(path);
	#rslt= 0;
	rslt = os.system("make");
	if rslt!=0:
		print "Error in make.. using old compilation";
		
	print "Starting all the systems";
	os.system("cp backupfs_copy/backupfs_copy /bin/");
		
	rslt = os.system("rmmod chardev/chardev.ko");
	rslt = os.system("insmod chardev/chardev.ko");
	
	rslt = os.system("rmmod backupfs/wrapfs.ko");
	rslt = os.system("insmod backupfs/wrapfs.ko");
	
	pid=os.fork();
	if(pid==0):
		os.execvp("gnome-terminal", ["gnome-terminal","-x",path+"userprog/userprog","backup","--title=","User Program"]);
	return pid;


def execute_tests(test_folder, code_dir):
	global path;
	path = code_dir+"/";
	print "Deleting old backups from the system";
	os.system("rm -rf /sls-data");
	
	pid_b = prepare_test_run();
		
	#parent_path = os.path.dirname(test_folder);
	parent_path = test_folder;
	print "Mounting wrapfs on "+parent_path;
	rslt = os.system("mount -t wrapfs "+parent_path+" "+parent_path);
		
	raw_input("Press any key to execute delete operation on "+test_folder);
	print "Executing Delete Operation on the test bed";	
	choice = raw_input("Remove test folder "+test_folder+" ?");
	if choice != 'no' and test_folder!="/":
		os.system("rm -rf "+test_folder);
	else:
		print "Abort!";
	
	#print "Sleeping for 10 secs";
	#time.sleep(10);
	
	raw_input("Press any key to start restore");
	
	print "unmounting wrapfs on "+parent_path;
	rslt = os.system("umount -t wrapfs "+parent_path);
	
	
	print path+"userprog/userprog";
	pid_r=os.fork();
	if(pid_r==0):
		os.execvp("gnome-terminal", ["gnome-terminal","-x",path+"userprog/userprog","restore","--title=","Restore"]);
	time.sleep(5);
	os.kill(pid_r,9);
	os.kill(pid_b,9);
	os.wait();
	return 0

if __name__ == '__main__':
	os.exit(-1);

