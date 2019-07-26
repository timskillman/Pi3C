#pragma once
#include <SDL_mixer.h>

class Pi3Csound {
public:
	Pi3Csound(const char * soundfile) { if (!sound) sound = Mix_LoadWAV(soundfile); }
	~Pi3Csound() { if (sound) { Mix_FreeChunk(sound); sound = nullptr; } }

	void play(int channel, int loop = 0) { this->channel = channel; Mix_PlayChannel(channel, sound, loop); }
	void volume(int level) { Mix_Volume(channel, level); }

	Mix_Chunk * sound;
	int channel = -1;
};

class Pi3Cmusic {
public:
	Pi3Cmusic(const char * musicfile) { if (!music) music = Mix_LoadMUS(musicfile); }
	~Pi3Cmusic() { if (music) { Mix_FreeMusic(music); music = nullptr; } }

	void play(int loop = -1) { Mix_PlayMusic(music, loop); }
	void volume(int level) { Mix_VolumeMusic(level); }

	Mix_Music * music;
};
