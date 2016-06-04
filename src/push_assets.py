import shutil
import os
#
# Push assets to android.
#

shutil.copyfile("./manifest.lua", "./android/assets/manifest.lua")
shutil.copyfile("./settings.lua", "./android/assets/settings.lua")
src_files = os.listdir("./test")
for file_name in src_files:
    full_file_name = os.path.join("./test", file_name)
    if (os.path.isfile(full_file_name)):
        shutil.copy(full_file_name, "./android/assets/test")
