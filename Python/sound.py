###############################################################################
# utf-8
# python 3.5.2
# 2018/04/17
#
# (sample command)
# files in directory AAA to zip in BBB directory
# python sound.py AAA BBB
###############################################################################
import zipfile
import datetime
import os
import sys

FILES_NUM_IN_ZIP = 3

def removeFileList(filedir, filelist):

    for i in range(FILES_NUM_IN_ZIP):
      os.remove(filedir + '/' + filelist[i])

    print("success to delete files no longer needed")

def fileListInDirToZip(filedir, zipdir):

      filelist = os.listdir(filedir + '/')
      wavlist = []
      
      # only wav file
      for file in filelist:
        base, ext = os.path.splitext(file)
        if ext == '.wav':
            wavlist.append(file)

      if len(wavlist) < FILES_NUM_IN_ZIP:
          print('number of files is smaller than ' + str(FILES_NUM_IN_ZIP))
          sys.exit();

      now = datetime.datetime.now()
      zippath = zipdir + '/' + str(now) + '.zip'
      with zipfile.ZipFile(zippath, 'w', compression=zipfile.ZIP_DEFLATED) as new_zip:
          for i in range(FILES_NUM_IN_ZIP):
            new_zip.write(filedir + '/' + wavlist[i])

      print("success to create zip")

      removeFileList(filedir, wavlist)

def fileToZip(filedir, zipdir):

    fileListInDirToZip(filedir, zipdir)

if __name__=='__main__':

    print("--start")

    args = sys.argv
    
    if len(args) != 3:
        print("args error")

    fileToZip(args[1], args[2])

    print("--end")

