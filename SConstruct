env = Environment()

env.Append(CPPFLAGS = Split("-g -Wall -O2"))
# env.Append(LIBS = Split(''))

env.Program('pixel-server', Glob('src/*.cpp'))

