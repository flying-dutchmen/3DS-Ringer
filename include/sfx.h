#ifndef SFX_H
#define SFX_H

#define NUMSFX (32)

typedef struct
{
	u8* rdata;
	u8* ldata;
	u32 size;
	u32 format;
	u32 sampleRate; //44100, 22050, 11025
	bool stereo;
	bool used;
}SFX_s;

void initSound(void);
void exitSound(void);
void initSFX(SFX_s* s);
void loadSFX(SFX_s* s, char* filename, u32 format, u32 Rate, bool InStereo);
SFX_s* createSFX(char* filename, u32 format, u32 Rate, bool InStereo);
void playSFX(SFX_s* s);

#endif
