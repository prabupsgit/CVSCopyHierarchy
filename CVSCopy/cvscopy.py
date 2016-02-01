import sys
import os
import distutils
import shutil
from distutils import dir_util
from distutils import file_util
from ctypes import *



def create_tree(dst):
    if not os.path.exists(dst):
        os.makedirs(dst)

def copy_target(src, dst):

    if(os.path.isdir(src)):
        distutils.dir_util.copy_tree(src, dst)
    else:
        distutils.file_util.copy_file(src, dst)

try:
    FILE_ATTRIBUTE_HIDDEN = 0x02
    source_arr = sys.argv[2:]
    #source_arr = [source[0:-1] for source in source_arr]
    #print 'Last is space: ' + str(source[-1].isspace())
    #print sys.argv
    for source in source_arr:
        source_tokens = [x for x in source.split('\\')]
        tempsource = ''
        dest = sys.argv[1]
        tempsource = source_tokens[0] + '\\' + source_tokens[1] + '\\' + source_tokens[2] + '\\' + source_tokens[3]
        end = len(source_tokens)
        if os.path.isfile(source):
            end = -1

        for tokens in source_tokens[4: end]:
            dest = os.path.join(dest, tokens)
            create_tree(dest)

            if tempsource[-1] == ':':
                tempsource += '\\'

            tempsource = os.path.join(tempsource, tokens)
            tempsrccvs = os.path.join(tempsource, "CVS")


            #print tempsource
            #print tempsrccvs
            #raw_input()

            if(os.path.exists(tempsrccvs) and os.path.isdir(tempsrccvs)):
                destcvs = os.path.join(dest, "CVS")
                if(os.path.exists(destcvs)):
                    shutil.rmtree(destcvs)
                    create_tree(destcvs)
                #hide cvs folder
                distutils.dir_util.copy_tree(tempsrccvs, destcvs)
                windll.kernel32.SetFileAttributesW(unicode(destcvs), FILE_ATTRIBUTE_HIDDEN)

        copy_target(source, dest)

except OSError as e:
    print '!!!Failed!!!\n\n'
    print e.message
    raw_input()

except Exception as e:
    print '!!!Failed!!!\n\n'
    print e.message
    raw_input()
