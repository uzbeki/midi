#include "declarations.h"
struct Header header;
struct Track track;
unsigned char ch;
int count;

// char* location = "midis/aladdin.mid";   //midi file
char* location = "midis/miki.mid";   //midi file

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
}

void delta_time(unsigned char channel, FILE* open_file) {
    if (channel >= CMD_NOTE_OFF && channel != CMD_COMMON_RESET) {
        channel = fgetc(open_file);
        if (channel >= CMD_NOTE_OFF) {
            channel = fgetc(open_file);
            if (channel >= CMD_NOTE_OFF) {
                channel = fgetc(open_file);
                count++;
            }
            count++;
        }
        count++;
    }

}

bool is_midi(char* file_location) {
    unsigned char aim[16] = { 0x4d, 0x54, 0x68, 0x64 };
    FILE* file_pointer = fopen(file_location, "rb"); //opens the binary file "rb" read binary
    for (size_t count = 0; count < 4 && !feof(file_pointer); count++) {
        ch = fgetc(file_pointer);
        if (ch != *(aim + count)) {
            fclose(file_pointer); //closes the open file
            return false;
        }
    }
}
unsigned char move_twice(unsigned char chan, FILE* file) {
    chan = fgetc(file);
    count += 2;
    return fgetc(file);
}
void analyze_meta_event(unsigned char channel, FILE* open_file) {
    channel = move_twice(channel, open_file);
    for (int i = 0; i < channel; i++) {
        channel = fgetc(open_file);
        count++;
    }
    meta_events++;
}

void analyze_sys_ex(unsigned char channel, FILE* open_file) {
    channel = fgetc(open_file); //next length
    count++;
    for (size_t i = 0; i < channel; i++, count++) {
        channel = fgetc(open_file);
    }
    system_exclusive++;
}

void count_events(unsigned char channel, FILE* open_file) {
    channel &= 0xf0; //1111 0000 nullifies the last 4 bits
    if (channel == CMD_SYSTEM_EXCLUSIVE) {
        analyze_sys_ex(channel, open_file);
    } else if (channel == CMD_NOTE_ON || channel == CMD_NOTE_OFF) {
        move_twice(channel, open_file) == 0x00 ? note_off++ : note_on++;
    } else if (channel == CMD_KEY_PRESSURE || channel == CMD_CONTROL_CHANGE) {
        channel == CMD_KEY_PRESSURE ? key_pressure++ : control_change++;
        move_twice(channel, open_file);
    } else if (channel == CMD_PROGRAM_CHANGE || channel == CMD_CHANNEL_PRESSURE) {
        channel == CMD_PROGRAM_CHANGE ? program_change++ : channel_pressure++;
        channel = fgetc(open_file);
        count++;
    } else if (channel == CMD_PITCH_CHANGE) {
        move_twice(channel, open_file);
        pitch_change++;
    }
}

void get_hex_data(char* loc) {
    FILE* ptr = fopen(loc, "rb");
    FILE* writing_file = fopen("binary.txt", "w");
    while (!feof(ptr)) { fprintf(writing_file, "%.2x ", fgetc(ptr)); }
    fclose(writing_file); //closes the open file
    fclose(ptr);
}

void calculate_results() {
    file_format = (header.file_format1 << 8) + header.file_format2;
    track_count = (header.track_count1 << 8) + header.track_count2;
    dt_count = (header.dt_count1 << 8) + header.dt_count2;
    printf("\nHeader Chunk:\n\tType「チャンクタイプ」: MThd\n\tLength「データ」: 6\n\tFormat「フォーマット」: %d\n\tTrack count「トラック数」: %d\n\tDelta-time tick count「時間単位」: %d\n", file_format, track_count, dt_count);
    printf("\nTrack Chunk:\n\tType「チャンクタイプ」: MTrk\n");
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
    printf("\tMeta-events: %d\n", meta_events);
}

int main() {
    if (is_midi(location)) {    //.midだったら
        FILE* file_pointer = fopen(location, "rb"); //opens the binary file "rb" read binary
        for (count = 0; !feof(file_pointer) && count <= 21; count++) { analyze_header_chunk(fgetc(file_pointer)); }
        track_len = (track.track_len1 << 8) + track.track_len2 + (track.track_len3 << 8) + track.track_len4;
        for (count; count <= track_len + 21 && !feof(file_pointer); count++) {
            ch = fgetc(file_pointer);
            if (ch == CMD_COMMON_RESET) { analyze_meta_event(ch, file_pointer); }
            if (ch == CMD_SYSTEM_EXCLUSIVE) { analyze_sys_ex(ch, file_pointer); }
            delta_time(ch, file_pointer);
            ch = fgetc(file_pointer);
            count++;
            ch == CMD_COMMON_RESET ? analyze_meta_event(ch, file_pointer) : count_events(ch, file_pointer);
        }
        fclose(file_pointer); //closes the open file
        calculate_results();
    } else { printf("\tThis is not a midi file.\n"); }
    get_hex_data(location);
    return 0;
}