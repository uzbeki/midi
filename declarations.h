#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
/* events */
#define CMD_NOTE_OFF               0x80
#define CMD_NOTE_ON                0x90
#define CMD_KEY_PRESSURE           0xa0
#define CMD_CONTROL_CHANGE         0xb0
#define CMD_PROGRAM_CHANGE         0xc0
#define CMD_CHANNEL_PRESSURE       0xd0
#define CMD_PITCH_CHANGE           0xe0
// common events
#define CMD_SYSTEM_EXCLUSIVE       0xf0

#define CMD_COMMON_MTC_QUARTER     0xf1
#define CMD_COMMON_SONG_POS        0xf2
#define CMD_COMMON_SONG_SELECT     0xf3
#define CMD_COMMON_TUNE_REQUEST    0xf6
#define CMD_COMMON_SYSEX_END       0xf7
#define CMD_COMMON_CLOCK           0xf8
#define CMD_COMMON_START           0xfa
#define CMD_COMMON_CONTINUE        0xfb
#define CMD_COMMON_STOP            0xfc
#define CMD_COMMON_SENSING         0xfe
#define CMD_COMMON_RESET           0xff


unsigned short file_format;
unsigned short track_count;
unsigned short dt_count;
unsigned long track_len;
unsigned int note_on, note_off, key_pressure, control_change, program_change, channel_pressure, pitch_change, system_exclusive, meta_events;
unsigned char status;
struct Header {
    unsigned char file_format1;     //8
    unsigned char file_format2;     //9

    unsigned char track_count1;     //10
    unsigned char track_count2;     //11

    unsigned char dt_count1;        //12
    unsigned char dt_count2;        //13

};


struct Track {
    unsigned char track_len1;     //18
    unsigned char track_len2;     //19
    unsigned char track_len3;     //20
    unsigned char track_len4;     //21
};