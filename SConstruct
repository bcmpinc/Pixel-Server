env = Environment()

env.ParseConfig('sdl-config --cflags')
env.ParseConfig('sdl-config --libs')


env.Append(CPPFLAGS = Split("-g -Wall -O2"))
env.Append(LIBS = Split('SDL_net'))

env.Program('pixel-server', Glob('src/*.cpp'))

