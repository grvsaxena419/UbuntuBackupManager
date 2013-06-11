#cd <GOTO folder you want to make the directory in> #example == /home/prerna
mkdir testFolder
cd testFolder

#start BLOCK : you can make the text files here
echo file > testFile.txt
#end BLOCK

#start BLOCK : you can make the folders u need here and the subfiles
mkdir testSubfolder
cd testSubfolder
echo Subfile > testSubfile1.txt
echo Subfile > testSubfile2.txt
#end BlOCK

#start BLOCK : move around the directory tree and deleting whatever you want
\\cd testSubfolder
rm -f testSubfile2.txt
#end BLOCK
