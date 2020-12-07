#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>



unsigned short file_format;
unsigned short track_count;
unsigned short dt_count;
unsigned long track_len;
unsigned int note_on, note_off, key_pressure, control_change, program_change, channel_pressure, pitch_wheel_change, system_exclusive, meta_events;

bool is_midi(char* file_location) {
    char ch;
    unsigned char aim[16] = { 0x4d, 0x54, 0x68, 0x64 };
    FILE* file_pointer = fopen(file_location, "rb"); //opens the binary file "rb" read binary
    for (size_t count = 0; count < 4 && !feof(file_pointer); count++) {
        ch = fgetc(file_pointer);
        // printf("ch is %x\n", ch);
        if (ch != *(aim + count)) {
            fclose(file_pointer); //closes the open file
            return false;
        }
    }
}

void get_hex_data(char* loc) {
    unsigned char ch;
    FILE* ptr = fopen(loc, "rb");
    FILE* writing_file = fopen("binary.txt", "w");
    while (!feof(ptr)) {
        ch = fgetc(ptr);
        fprintf(writing_file, "%.2x ", ch);
    }
    fclose(writing_file); //closes the open file
    fclose(ptr);
}



int main() {
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

    struct Header header;
    struct Track track;




    // char* location = "midis/aladdin.mid";   //midi file
    char* location = "midis/miki.mid";   //midi file
    // char* location = "miki.txt";   //NOT a midi file
    unsigned char ch;
    int count;
    if (is_midi(location)) {
        FILE* file_pointer = fopen(location, "rb"); //opens the binary file "rb" read binary
        while (!feof(file_pointer)) {
            ch = fgetc(file_pointer);

            // header chunk analysis
            switch (count) {
            case 8:
                header.file_format1 = ch;
                break;
            case 9:
                header.file_format2 = ch;
                break;
            case 10:
                header.track_count1 = ch;
                break;
            case 11:
                header.track_count2 = ch;
                break;
            case 12:
                header.dt_count1 = ch;
                break;
            case 13:
                header.dt_count2 = ch;
                break;
            case 18:
                track.track_len1 = ch;
                break;
            case 19:
                track.track_len2 = ch;
                break;
            case 20:
                track.track_len3 = ch;
                break;
            case 21:
                track.track_len4 = ch;
                break;
            }
            switch (ch) {
            case 0x9:
                note_on++;
                break;
            case 0x8:
                note_off++;
                break;
            case 0xa:
                key_pressure++;
                break;
            case 0xb:
                control_change++;
                break;
            case 0xc:
                program_change++;
                break;
            case 0xd:
                channel_pressure++;
                break;
            case 0xe:
                pitch_wheel_change++;
                break;
            case 0xf:
                system_exclusive++;
                break;
            }
            count++;
        }
        fclose(file_pointer); //closes the open file

        file_format = (header.file_format1 << 8) + header.file_format2;
        track_count = (header.track_count1 << 8) + header.track_count2;
        dt_count = (header.dt_count1 << 8) + header.dt_count2;
        track_len = (track.track_len1 << 8) + track.track_len2 + (track.track_len3 << 8) + track.track_len4;
        printf("\nHeader Chunk:\n");
        printf("\tType「チャンクタイプ」: MThd\n");
        printf("\tLength「データ」: 6\n");
        printf("\tFormat「フォーマット」: %d\n", file_format);
        printf("\tTrack count「トラック数」: %d\n", track_count);
        printf("\tDelta-time tick count「時間単位」: %d\n", dt_count);

        printf("\nTrack Chunk:\n");
        printf("\tType「チャンクタイプ」: MTrk\n");
        printf("\tTrack length「T データ長」: %d\n", track_len);

        printf("\nEvents count:\n");
        printf("\tNote on: %d\n", note_on);
        printf("\tNote off: %d\n", note_off);
        printf("\tKey pressure: %d\n", key_pressure);
        printf("\tControl Change: %d\n", control_change);
        printf("\tProgram change: %d\n", program_change);
        printf("\tChannel pressure: %d\n", channel_pressure);
        printf("\tPitch wheel change: %d\n", pitch_wheel_change);
        printf("\tSystem Exclusive: %d\n", system_exclusive);

    } else {
        printf("\t違う。.midファイル頂戴\n");
    }

    // get_hex_data(location);
    return 0;
}

