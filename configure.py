import sys
import platform
import subprocess
import locale

debug = True
optimizate = False
test = False
codeblocks = True

args = sys.argv[1:]
# if len(args) == 0:
#     args.append('-h')

lib_path_option = False
inc_path_option = False

options_str = ["--gcc-path=", "--alleg-inc-path=", "--alleg-lib-path="]
options_v = ["", "", ""]

##lib_path = ""
##inc_path = ""

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
    elif arg == "--no-codeblocks":
        codeblocks = False
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
    else:
        for opt in options_str:
            if arg.startswith(opt):
                options_v[options_str.index(opt)] = arg[arg.index('=')+1:]
                if options_v[options_str.index(opt)][-1] != "\\":
                    options_v[options_str.index(opt)] += "\\"
##                print(options_v)
  

print('generating makefile...')
mk = open("makefile", "w")
options = ""
if debug:
    options += "DEBUG"
if optimizate:
    options += "OPTIMIZATE"
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
##        print(options_v[0])
        if options_v[0] == "":
            out = subprocess.run(["where", "gcc"], stdout=subprocess.PIPE,
encoding=locale.getpreferredencoding(False)).stdout.splitlines()[0].split("\\")
            options_v[0] = "\\".join(out[:-2]) + "\\"
        if options_v[1] == "":
            options_v[1] = options_v[0] + "allegro\\include"
        if options_v[2] == "":
            options_v[2] = options_v[0] + "allegro\\lib"
        mk.write("NULL_STR =\n")
        mk.write("GCC_PATH = " + options_v[0] + "bin\\$(NULL_STR)\n")
        mk.write("ALLEG_INC = -I" + options_v[1] + "\n")
        mk.write("ALLEG_LIB = -L" + options_v[2] + " -lallegro_monolith-debug-static -ljpeg -ldumb -lFLAC -lfreetype -lvorbisfile -lvorbis -logg -lphysfs -lpng16 -lzlib -ldsound -lgdiplus -luuid -lkernel32 -lwinmm -lpsapi -lopengl32 -lglu32 -luser32 -lcomdlg32 -lgdi32 -lshell32 -lole32 -ladvapi32 -lws2_32 -lshlwapi -static-libstdc++ -static-libgcc\n")
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



if os == "windows":
        import subprocess
        subprocess.call(["del", "/S", "/F", "/Q", "codeblocks", ">nul", "2>&1"], shell=True)
        subprocess.call(["rmdir", "/S", "/Q", "codeblocks", ">nul", "2>&1"], shell=True)
        if not codeblocks:
            exit()
        subprocess.call(["mkdir", "codeblocks", "2>nul"], shell=True)
        subprocess.call(["mkdir", "codeblocks\\art", "2>nul"], shell=True)
        import glob
        files_c = glob.glob("*.c")
        files_h = glob.glob("*.h")

        files_proj = ""

        for fl in files_c:
            files_proj += """\
		<Unit filename="{0}">
			<Option compilerVar="CC" />
		</Unit>
""".format(fl)

        for fl in files_h:
            files_proj += """\
		<Unit filename="{0}" />
""".format(fl)
	
        gen = open("codeblocks\\chess.cbp", "w")
	
        str = """\
<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
<CodeBlocks_project_file>
	<FileVersion major="1" minor="6" />
	<Project>
		<Option title="chess" />
		<Option pch_mode="2" />
		<Option compiler="gcc" />
		<Build>
			<Target title="Debug">
				<Option output="bin/Debug/new_test" prefix_auto="1" extension_auto="1" />
				<Option object_output="obj/Debug/" />
				<Option type="1" />
				<Option compiler="gcc" />
				<Compiler>
					<Add option="-g" />
				</Compiler>
			</Target>
			<Target title="Release">
				<Option output="bin/Release/new_test" prefix_auto="1" extension_auto="1" />
				<Option object_output="obj/Release/" />
				<Option type="1" />
				<Option compiler="gcc" />
				<Compiler>
					<Add option="-O2" />
				</Compiler>
				<Linker>
					<Add option="-s" />
				</Linker>
			</Target>
		</Build>
		<Compiler>
			<Add option="-Wall" />
			<Add directory="{0}" />
		</Compiler>
		<Linker>
			<Add option="-lallegro_monolith-debug-static" />
			<Add option="-ljpeg" />
			<Add option="-ldumb" />
			<Add option="-lFLAC" />
			<Add option="-lfreetype" />
			<Add option="-lvorbisfile" />
			<Add option="-lvorbis" />
			<Add option="-logg" />
			<Add option="-lphysfs" />
			<Add option="-lpng16" />
			<Add option="-lzlib" />
			<Add option="-ldsound" />
			<Add option="-lgdiplus" />
			<Add option="-luuid" />
			<Add option="-lkernel32" />
			<Add option="-lwinmm" />
			<Add option="-lpsapi" />
			<Add option="-lopengl32" />
			<Add option="-lglu32" />
			<Add option="-luser32" />
			<Add option="-lcomdlg32" />
			<Add option="-lgdi32" />
			<Add option="-lshell32" />
			<Add option="-lole32" />
			<Add option="-ladvapi32" />
			<Add option="-lws2_32" />
			<Add option="-lshlwapi" />
			<Add directory="{1}" />
		</Linker>
{2}
		<Extensions>
			<code_completion />
			<envvars />
			<debugger />
			<lib_finder disable_auto="1" />
		</Extensions>
	</Project>
</CodeBlocks_project_file>""".format(options_v[1], options_v[2], files_proj)
        gen.write(str)
        gen.close()
##	copyd = open("codeblocks\\chess.c", "w")
##	copys = open("chess.c", "r")
##	tt = copys.read()
##	copyd.write(tt)
##	copyd.close()
##	copys.close()
        subprocess.call(["copy", "*.c", "codeblocks\\*.c", "/Y", "/V", ">nul"], shell=True)
        subprocess.call(["copy", "*.h", "codeblocks\\*.h", "/Y", "/V", ">nul"], shell=True)
        subprocess.call(["copy", "art\\*.*", "codeblocks\\art\\*.*", "/Y", "/V", ">nul"], shell=True)
        subprocess.call(["attrib", "+r", "codeblocks\\*.*"], shell=True)
        print("codeblocks project built")
        pass
