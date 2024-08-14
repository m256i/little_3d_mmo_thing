import glob
all_files = glob.glob("./*.cpp") + glob.glob("./**/*.cpp", recursive=True) + \
    glob.glob("./*.cc") + glob.glob("./**/*.cc", recursive=True) + \
    glob.glob("./*.cxx") + glob.glob("./**/*.cxx", recursive=True) + \
    glob.glob("./*.c") + glob.glob("./**/*.c", recursive=True)
    
filtered_list = [
    i for i in all_files if "subprojects" not in i if "build" not in i]
filtered_list = list(dict.fromkeys(filtered_list))
for i in filtered_list:
    print(i)
