#include "declarations.h"
struct Header header;
struct Track track;




// char* location = "midis/aladdin.mid";   //midi file
char* location = "midis/miki.mid";   //midi file
// char* location = "miki.txt";   //NOT a midi file
unsigned char ch;
int count;


void analyze_header_chunk(unsigned char channel) {
    switch (count) {
    case 8: header.file_format1 = channel; break;
    case 9: header.file_format2 = channel; break;
    case 10: header.track_count1 = channel; break;
    case 11: header.track_count2 = channel; break;
    case 12: header.dt_count1 = channel; break;
    case 13: header.dt_count2 = channel; break;
    case 18: track.track_len1 = channel; break;
    case 19: track.track_len2 = channel; break;
    case 20: track.track_len3 = channel; break;
    case 21: track.track_len4 = channel; break;
    }
    return;
}

/* checks if channel is delta time or not
// 1バイトの時の時間データ：0x7F
// 2バイトの時の時間データ：0x81 0x7F
// 3バイトの時の時間データ：0x81 0x81 0x7F
// 4バイトの時の時間データ：0x81 0x81 0x810x7F */
void delta_time(unsigned char channel, FILE* open_file) {
    if (channel >= CMD_NOTE_OFF) {
        channel = fgetc(open_file);
        if (channel >= CMD_NOTE_OFF) {
            channel = fgetc(open_file);
            // delta time
            if (channel >= CMD_NOTE_OFF) {
                channel = fgetc(open_file);
                count++;
            }
            count++;
            return;
        }
        count++;
        return;
    }
    return;
}

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

void count_events(unsigned char channel, FILE* open_file) {
    /*
        1000nnnn //nnnn is midi channel number iranai
        1001nnnn
        ...
     */
     // status = channel & 0xf0;
    channel &= 0xf0; //1111 0000 nullifies the last 4 bits
    // key on : 1001 nnnn
    // 1111 0000

    switch (channel) {
    case CMD_NOTE_ON: //note on
        // printf("\ninside NOTE ON - %.2x\n", channel);
        channel = fgetc(open_file); //key
        // printf("\t%.2x\n", channel);
        count++;
        channel = fgetc(open_file); // velocity
        // printf("\t%.2x\n", channel);
        channel == 0x00 ? note_off++ : note_on++;
        count++;
        break;
    case CMD_NOTE_OFF: //note off
        // printf("\ninside NOTE OFF[%d] - %.2x\n", count, channel);
        channel = fgetc(open_file); //key
        // printf("\t%.2x\n", channel);
        channel = fgetc(open_file); //velocity
        count += 2;
        note_off++;
        // printf("\t%.2x %dnote off++\n", channel, note_off);
        break;
    case CMD_KEY_PRESSURE:
        // printf("\ninside KEY PRESSURE[%d] - %.2x\n", count, channel);
        channel = fgetc(open_file);
        channel = fgetc(open_file);
        count += 2;
        key_pressure++;
        break;
    case CMD_CONTROL_CHANGE:
        channel = fgetc(open_file);
        channel = fgetc(open_file);
        count += 2;
        control_change++;
        break;
    case CMD_PROGRAM_CHANGE:
        channel = fgetc(open_file);
        count++;
        program_change++;
        break;
    case 0xd0:
        channel = fgetc(open_file);
        count++;
        channel_pressure++;
        break;
    case CMD_PITCH_CHANGE:
        channel = fgetc(open_file);
        channel = fgetc(open_file);
        count += 2;
        pitch_change++;
        break;
    case CMD_SYSTEM_EXCLUSIVE:
        channel = fgetc(open_file);
        for (size_t i = 0; i < channel; i++, count++) {
            channel = fgetc(open_file);
        }
        count++;
        system_exclusive++;
        break;
    }
    return;
}

/* ・メタイベント
　FF aa bb cc dd ee
　最初に0xFFが来た場合、メタイベントとなる、
　次aaはイベントタイプで
　次のbbにデータの長さが入る。
　FF aa bb cc dd ee
　上記のようなデータだった場合はbbに「03」が入る */
void analyze_meta_event(unsigned char channel, FILE* open_file) {
    channel = fgetc(open_file);
    channel = fgetc(open_file); //bb 
    count += 2;
    for (int i = 0; i < channel; i++) {
        channel = fgetc(open_file);
        count++;
    }
    return;
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
    return;
}

void calculate_results() {
    file_format = (header.file_format1 << 8) + header.file_format2;
    track_count = (header.track_count1 << 8) + header.track_count2;
    dt_count = (header.dt_count1 << 8) + header.dt_count2;
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
    printf("\tPitch wheel change: %d\n", pitch_change);
    printf("\tSystem Exclusive: %d\n", system_exclusive);
    return;
}

int main() {
    if (is_midi(location)) {    //.midだったら
        FILE* file_pointer = fopen(location, "rb"); //opens the binary file "rb" read binary
        while (!feof(file_pointer) && count <= 21) {
            analyze_header_chunk(fgetc(file_pointer));   // header chunk analysis
            count++;
        }
        track_len = (track.track_len1 << 8) + track.track_len2 + (track.track_len3 << 8) + track.track_len4;


        for (count; count <= track_len + 21 && !feof(file_pointer); count++)         {
            delta_time(fgetc(file_pointer), file_pointer);
            ch = fgetc(file_pointer);
            count++;
            ch == CMD_COMMON_RESET ? analyze_meta_event(ch, file_pointer) : count_events(ch, file_pointer);
        }

        fclose(file_pointer); //closes the open file
        calculate_results();
        printf("\ncount: %d\n", count);

    } else { printf("\tThis is not a midi file.\n"); }

    // get_hex_data(location);
    return 0;
}

