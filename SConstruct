###library list
libraries = []
pathToLibraries=['./lib']

##Reference: https://stackoverflow.com/questions/26342109/scons-build-all-sources-files-in-all-directories

def AllSources(node='.', pattern='*'):
    result = [AllSources(dir, pattern)
              for dir in Glob(str(node)+'/*')
              if dir.isdir()]
    result += [source
               for source in Glob(str(node)+'/'+pattern)
               if source.isfile()]
    return result

env = Environment(
    CPPPATH=['./src/']
)

#env.Replace(CXX = 'mingw-w64-gcc')

optimize = ARGUMENTS.get('optimize',0)

if int(optimize):
    env.Append(CCFLAGS='-O3 -std=c++20')
else:
    env.Append(CCFLAGS='-g -std=c++20 -Wall -Wpedantic -Wconversion')

asan = ARGUMENTS.get('clang-sanitize',0)

clang = ARGUMENTS.get('clang',0)

if int(clang):
    env.Replace(CC= "clang", CXX="clang++")
    #libraries.append('stdc++fs')

if int(asan):
    env.Replace(CC= "clang", CXX="clang++")
    env.Replace(CCFLAGS='-O3 -std=c++20 ')
    env.Append(CCFLAGS='-fsanitize=address', LINKFLAGS='-fsanitize=address -fno-omit-frame-pointer')

env.Append(LINKFLAGS='-static -static-libgcc -static-libstdc++')

app = env.Program(target= 'convexHull',source = AllSources('./src', '*.cpp*'),LIBS = libraries, LIBPATH=pathToLibraries )
#Library(target= 'randysEngine',source = src_files)
