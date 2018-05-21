import sys
import platform

debug = True
optimizate = False
test = False

args = sys.argv[1:]
# if len(args) == 0:
#     args.append('-h')

lib_path_option = False
inc_path_option = False

lib_path = ""
inc_path = ""

for arg in args:
    if arg == "--test-ok":
        test = True
    elif arg == "--debug-ok":
        debug = True
        optimizate = False
    elif arg == "--nodebug-ok":
        debug = False
    elif arg == "--release-ok":
        debug = False
        optimizate = True
    elif arg in ["--help", "-h"]:
        print(
"""\
options:
  --test-ok: enable testing, disabled by default
  --debug-ok: enable debugging, enabled by default
  --nodebug-ok: disable debugging
  --release-ok: no debug and enable optimizations, disabled by default
  --help or -h: print this message
be good\
        """)
        exit(0)
    elif arg == "--alleg-inc-path":
        inc_path_option = True
    elif arg == "--alleg-lib-path":
        lib_path_option = True
    elif inc_path_option == True:
        inc_path_option = False
        inc_path = arg
    elif lib_path_option == True:
        lib_path_option  = False
        lib_path = arg

print('generating makefile...')
mk = open("makefile", "w")
options = ""
if debug:
    options += "-g"
if optimizate:
    options += "-O2"
if options != "":
    mk.write("options = " + options + "\n")

os = ""
if platform.machine().endswith("64"):
    if platform.system() == "Linux":
        os = "linux"
        mk.write("os = linux\n")
        mk.write("obj_ext = .o\n")
        mk.write("ALLEG_LIB = `pkg-config --libs allegro-5 allegro_image-5 allegro_primitives-5`\n")
        mk.write("ALLEG_INC = `pkg-config --cflags allegro-5 allegro_image-5 allegro_primitives-5`\n")
        print('64 bit linux system detected')

    elif platform.system() == "Windows":
        os = "windows"
        mk.write("os = windows\n")
        mk.write("exe_ext = .exe\n")
        mk.write("obj_ext = .obj\n")
        mk.write("ALLEG_LIB = -LE:\\Documents\\coisas_uteis\\mingw64\\allegro\\lib -lallegro_monolith-debug-static -ljpeg -ldumb -lFLAC -lfreetype -lvorbisfile -lvorbis -logg -lphysfs -lpng16 -lzlib -ldsound -lgdiplus -luuid -lkernel32 -lwinmm -lpsapi -lopengl32 -lglu32 -luser32 -lcomdlg32 -lgdi32 -lshell32 -lole32 -ladvapi32 -lws2_32 -lshlwapi -static-libstdc++ -static-libgcc\n")
        mk.write("ALLEG_INC = -IE:\\Documents\\coisas_uteis\\mingw64\\allegro\\include\n")
        print('64 bit windows system detected')

    else:
        print("Sorry, unsupported system")
        mk.close()
        exit(1)

f = open("makefile.in", "r")
mk.write(f.read())

f.close()
mk.close()
print('makefile built')

pass
