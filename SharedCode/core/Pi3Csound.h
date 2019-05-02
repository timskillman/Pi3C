#pragma once
#include <SDL_mixer.h>

class Pi3Csound {
public:
	Pi3Csound(const char * soundfile) { if (!sound) sound = Mix_LoadWAV(soundfile); }
	~Pi3Csound() { if (sound) { Mix_FreeChunk(sound); sound = nullptr; } }

	void play(int channel = -1, int loop = 0) { Mix_PlayChannel(channel, sound, loop); }

	Mix_Chunk * sound;
};

class Pi3Cmusic {
public:
	Pi3Cmusic(const char * musicfile) { if (!music) music = Mix_LoadMUS(musicfile); }
	~Pi3Cmusic() { if (music) { Mix_FreeMusic(music); music = nullptr; } }

	void play(int loop = -1) { Mix_PlayMusic(music, loop); }

	Mix_Music * music;
};
