//for use with helix
//found --> https://translate.google.ca/translate?hl=en&sl=zh-CN&u=http://bbs.eeworld.com.cn/forum.php%3Fmod%3Dviewthread%26action%3Dprintable%26tid%3D452352&prev=search
//revised Kenny.D Lee

#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include "helix/mp3dec.h"

static void mp3_player (const char * filename)
{
int err, i, outputSamps, current_sample_rate = 0;

FILE *fres;
short *outBuf[8196];
u32 bufflag;
int read_offset = 0; /* read offset pointer */
int bytes_left = 0; /* number of remaining bytes */
unsigned long Frames = 0; /* mp3 frame count */
unsigned char * buffer;
unsigned char * read_ptr = buffer; /* buffer pointer */
HMP3Decoder Mp3Decoder; /* mp3 decoder pointer */
MP3FrameInfo FrameInfo;

// Open an audio file
fres = fopen(filename, "rb+");
// Open failure
//if (fres != FR_OK)
//{
//printf ("read file% s error open another file \ r \ n!", filename);
//fres = f_close (& file);
//}

// Open success
// Initialize the MP3 decoder
Mp3Decoder = MP3InitDecoder();

// Get the input data stream, call the helix library decoded output PCM data
while (1)
{
// Read MP3 files
read_ptr = buffer; // point to the MP3 input stream
bytes_left = fread(buffer, 1, sizeof(buffer), fres); // read the actual size of the input stream

while (1)
{
read_offset = MP3FindSyncWord (read_ptr, bytes_left); // looking for frame synchronization, return to the sync word
if (read_offset <0) // did not find the frame synchronization
{
break; // out of the loop //
}

read_ptr += read_offset; // shift to the position of the synchronization word
bytes_left -= read_offset; // size of the data after the synchronization word
if (bytes_left <1024) // supplemental data
{
/* Use DMA to read, filled 4-byte alignment */
i = (u32)(bytes_left) & 3; // determine excess bytes
if (i) i = 4-i; // Need to add byte
memcpy (buffer + i, read_ptr, bytes_left); // copy from its starting position
read_ptr = buffer + i; // pointing to its location data
bytes_left += fread(buffer + bytes_left + i, 1, sizeof (buffer) -bytes_left-i, fres); // valid data stream size
}
err = MP3Decode (Mp3Decoder, &read_ptr, &bytes_left, outBuf[bufflag], 0); // start decoding
Frames ++;

if (err != ERR_MP3_NONE) // error handling
{
switch (err)
{
case ERR_MP3_INDATA_UNDERFLOW:
printf ("ERR_MP3_INDATA_UNDERFLOW \ r \ n");
read_ptr = buffer;
bytes_left = fread(read_ptr, 1, sizeof(buffer), fres);
break;

case ERR_MP3_MAINDATA_UNDERFLOW:
/* Do nothing - next call to decode will provide more mainData */
printf ("ERR_MP3_MAINDATA_UNDERFLOW \ r \ n");
break;

        default:
printf ("UNKNOWN ERROR:% d \ r \ n", err);
                       
// Skip this frame
if (bytes_left> 0)
{
bytes_left-- ;
read_ptr++ ;
}
break; // program should be in here to Hard fault
}
}
else // decoding error, output PCM
{
MP3GetLastFrameInfo (Mp3Decoder, &FrameInfo); // Get the decoded information

/* Set the sample rate based on the decoded information */
if (FrameInfo.samprate != current_sample_rate) // sample rate
{
current_sample_rate = FrameInfo.samprate;

printf ("\ r \ n Bitrate% dKbps", FrameInfo.bitrate / 1000);
printf ("\ r \ n Samprate% dHz", current_sample_rate);
printf ("\ r \ n BitsPerSample% db", FrameInfo.bitsPerSample);
printf ("\ r \ n nChans% d", FrameInfo.nChans);
printf ("\ r \ n Layer% d", FrameInfo.layer);
printf ("\ r \ n Version% d", FrameInfo.version);
printf ("\ r \ n OutputSamps% d", FrameInfo.outputSamps);

//if (current_sample_rate> = I2S_AudioFreq_Default)
{
//CODEC_I2S_Configuration (current_sample_rate); // modify I2S rate according to the sampling rate

}
}

/* Output dac */
outputSamps = FrameInfo.outputSamps; // PCM number of data

if (outputSamps> 0)
{
if (FrameInfo.nChans == 1) // Mono
{
// Copy the data to a single channel to another channel
for (i = outputSamps - 1; i >= 0; i--)
{
outBuf[bufflag] [i * 2] = outBuf[bufflag];
outBuf[bufflag] [i * 2 + 1] = outBuf[bufflag];
}
outputSamps *= 2;
}

//Audio_MAL_Play ((uint32_t) outBuf [bufflag], outputSamps);
//while (XferCplt == 0) /* wait for DMA transfert complete */
{} /* You can write some code for operate on UDA1380 */
//XferCplt = 0;
//bufflag = 1 -bufflag;
}
}


//if (file.fptr == file.fsize) // file pointer to the end of the file, the data read
//{
//printf (" \ r \ n");
//break;
//}

}

}
printf ("\ r \ n Close decoder");
MP3FreeDecoder (Mp3Decoder);

fclose(fres); // close the file
}
