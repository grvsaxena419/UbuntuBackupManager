main:
	cd chardev; make
	cp chardev/Module.symvers backupfs/
	cd backupfs; make		
	cd userprog; make
	cd backupfs_copy; make
run: main
	cd chardev; make insert
	cd backupfs; make insert
	cd userprog; make run
	sudo cp backupfs_copy/backupfs_copy /bin/
rerun:
	cd backupfs; make remov
	cd chardev; make reinsert
	cd backupfs; make insert
	pkill userprog
	cd userprog; make run

clean:
	cd backupfs; make clean
	cd chardev; make clean
	cd userprog; make clean
	cd backupfs_copy; make clean
