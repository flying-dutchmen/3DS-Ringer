#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include "sfx.h"
#include "filesystem.h"

SFX_s SFX[NUMSFX];

bool soundEnabled;

void initSound()
{
	int i;
	for(i=0;i<NUMSFX;i++)
	{
		SFX[i].used=false;
	}

	if(csndInit()==0)soundEnabled=true;
	else soundEnabled=false;
}

void exitSound(void)
{
	int i;
	for(i=0;i<NUMSFX;i++)
	{
		if(SFX[i].used)
		{
			if(SFX[i].ldata)
			{
				linearFree(SFX[i].ldata);
				SFX[i].ldata=NULL;
			}			
                        if ((SFX[i].stereo) && (SFX[i].rdata))
			{
				linearFree(SFX[i].rdata);
				SFX[i].rdata=NULL;
			}
			SFX[i].used=false;
		}
	}
	if(soundEnabled)csndExit();
}

void initSFX(SFX_s* s)
{
	if(!s)return;
	s->ldata=NULL;
	s->rdata=NULL;
	s->size=0;
	s->used=true;
	s->stereo=false;
}

//to be tested
void deinterlaceSFX(SFX_s* s, char* filename)
{
	if(!s)return;
        int i;
	u8* data = bufferizeFile(filename, &s->size, true, true); //read all data 
        s->ldata=(u8*)linearMemAlign(s->size >> 1, 0x80); //setup left
        s->rdata=(u8*)linearMemAlign(s->size >> 1, 0x80); //setup right

        // [left][right] ...
	if (s->format = SOUND_FORMAT_8BIT)
        for(i=0;i<s->size;i+=2)
	{
          s->ldata[i] = data[i]; 
          s->rdata[i] = data[i+1];
        } else //16bit 
        for(i=0;i<s->size;i+=4)
	{
          s->ldata[i] = data[i];
          s->rdata[i] = data[i+1];
          s->ldata[i+1] = data[i+2];
          s->rdata[i+1] = data[i+3];
        };

//reset size & free buffer
        s->size = s->size >> 1; // div 2
        linearFree(data);
}

void loadSFX(SFX_s* s, char* filename, u32 format, u32 Rate, bool inStereo)
{
	if(!s)return;

	initSFX(s);
	s->format=format;
	s->sampleRate=Rate;
	s->stereo=inStereo;
	(!s->stereo) ? s->ldata=bufferizeFile(filename, &s->size, true, true) : deinterlaceSFX(s, filename);
}

SFX_s* createSFX(char* filename, u32 format, u32 Rate, bool inStereo)
{
	int i;
	for(i=0;i<NUMSFX;i++)
	{
		if(!SFX[i].used)
		{
			loadSFX(&SFX[i], filename, format, Rate, inStereo);
			if(!SFX[i].ldata)return NULL;
			return &SFX[i];
		}
	}
	return NULL;
}

int channel;

void playSFX(SFX_s* s)
{
	if(!s || !s->used || !s->ldata || !soundEnabled)return;

	channel++;
	channel%=8;

          (!s->stereo) ? csndPlaySound(channel+8, s->format, s->sampleRate, 1.0, 0.0, (u32*)s->ldata, (u32*)s->ldata, s->size):
          csndPlaySound(channel+8, s->format, s->sampleRate, 1.0, 0.0, (u32*)s->ldata, (u32*)s->rdata, s->size);
}
