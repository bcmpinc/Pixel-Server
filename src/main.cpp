#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

using namespace std;

#define PORT 9999
#define TILE_SIZE 256

bool quit = false;

void pollevent() {
	SDL_Event event;
	/* Check for events */
	while (SDL_PollEvent (&event)) {
		if (event.type==SDL_QUIT) 
			quit = true;
	}
}

int main(int argc, char *argv[]) {
	if (SDL_Init (SDL_INIT_VIDEO) < 0) {
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit (SDL_Quit);
	
	// create a listening TCP socket on port PORT (server)
	IPaddress ip;
	if(SDLNet_ResolveHost(&ip,NULL,PORT)==-1) {
		printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		exit(1);
	}
	
	TCPsocket server_tcpsock = SDLNet_TCP_Open(&ip);
	if(!server_tcpsock) {
		printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		exit(1);
	}
	
	while (!quit) {
		SDL_Delay(10); 
		pollevent();
		
		// accept a connection coming in on server_tcpsock
		TCPsocket new_tcpsock;

		new_tcpsock=SDLNet_TCP_Accept(server_tcpsock);
		if(!new_tcpsock) {
			printf("SDLNet_TCP_Accept: %s\n", SDLNet_GetError());
		}
		else {
			// communicate over new_tcpsock
			printf("Got connection\n");
			SDLNet_TCP_Close(new_tcpsock);
		}
	}
	
	printf("Shutting down.\n");
	SDLNet_TCP_Close(server_tcpsock);
	
	return 0;
}
