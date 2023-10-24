import os
import sys
import shutil

from shutil import copy2
from shutil import copyfile
from shutil import copytree

def collceion_program_files(type, force_update, publish):
    base_path = ""
    if type == "debug":
        base_path = "./cmake-build-debug/"
    elif type == "release":
        base_path = "./cmake-build-release/"
    elif type == "rel-debug":
        base_path = "./cmake-build-relwithdebinfo/"
    else:
        print("don't known the mode : {}, must debug/release/rel-debug".format(sys.argv[1]))
        return

    print("the path is : {}".format(base_path))

    files_with_ref_path = []
    files = os.listdir(base_path)
    for file in files:
        file_path = base_path + "/" + file
        if ".dll" in file:
            files_with_ref_path.append(file_path)
        if ".exe" in file:
            files_with_ref_path.append(file_path)

    resources_file_path = []

    folders_path = []
    folders_path.append(base_path + "iconengines")
    folders_path.append(base_path + "imageformats")
    folders_path.append(base_path + "bearer")
    folders_path.append(base_path + "audio")
    folders_path.append(base_path + "mediaservice")
    folders_path.append(base_path + "platforms")
    folders_path.append(base_path + "plugins")
    folders_path.append(base_path + "styles")

    target_path = base_path + "publish"

    if force_update and os.path.exists(target_path):
        shutil.rmtree(target_path)

    if not os.path.exists(target_path):
        os.makedirs(target_path)

    for file in files_with_ref_path:
        file_name = file.split("/")[-1]
        print("copy file {} to {}".format(file_name, target_path + "/" + file_name))
        copyfile(file, target_path + "/" + file_name)

    for folder in folders_path:
        file_name = folder.split("/")[-1]
        print("copy folder {} to {}".format(file_name, folder))
        try:
            copytree(folder, target_path + "/" + file_name)
        except:
            print("3rd libs folder already exists, use : force-update if you want to update them.")

# python install.py release/debug/rel-debug
# gen publish folder at : out/build/{x64-debug/x64-release/x64-rel-debug}/publish
if __name__ == "__main__":
    # param 1. release / debug
    # 
    print("arg : {}".format(sys.argv))
    force_update = False
    publish = False
    if len(sys.argv) >= 3:
        if sys.argv[2] == "force-update":
            force_update = True
        if sys.argv[2] == "publish":
            force_update = True
            publish = True

    collceion_program_files(sys.argv[1], force_update, publish)