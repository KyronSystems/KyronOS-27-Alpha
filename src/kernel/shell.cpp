#include "kernel/shell.hpp"
#include "kernel/console.hpp"
#include "kernel/keyboard.hpp"
#include <stdint.h>

namespace {
constexpr uint32_t max_entries = 32;
constexpr uint32_t name_size = 32;
constexpr uint32_t data_size = 256;
constexpr uint32_t max_users = 8;
struct Entry { char name[name_size]; char data[data_size]; uint32_t parent; bool directory; bool used; };
struct User { char name[name_size]; uint32_t home; bool used; };
Entry entries[max_entries];
User users[max_users];
uint32_t home_directory = 0;
uint32_t current_user = 0;
uint32_t current_directory = 0;
uint32_t previous_directory = 0;
char cwd[64] = "/";
const char hostname[] = "kyron";

uint32_t length(const char* text) { uint32_t size = 0; while (text[size]) ++size; return size; }
bool equals(const char* left, const char* right) {
    uint32_t index = 0; while (left[index] || right[index]) { if (left[index] != right[index]) return false; ++index; } return true;
}
bool starts_with(const char* text, const char* prefix) {
    uint32_t index = 0; while (prefix[index]) { if (text[index] != prefix[index]) return false; ++index; } return true;
}
void copy(char* destination, const char* source, uint32_t capacity) {
    uint32_t index = 0; while (index + 1 < capacity && source[index]) { destination[index] = source[index]; ++index; } destination[index] = 0;
}
void build_path(uint32_t directory, char* destination, uint32_t capacity) {
    if (directory == 0) { copy(destination, "/", capacity); return; }
    uint32_t stack[8]; uint32_t depth = 0; uint32_t current = directory;
    while (current != 0 && depth < 8) { stack[depth++] = current; current = entries[current].parent; }
    destination[0] = 0;
    for (uint32_t index = depth; index > 0; --index) {
        uint32_t offset = length(destination);
        if (offset + 1 < capacity) destination[offset++] = '/';
        copy(destination + offset, entries[stack[index - 1]].name, capacity - offset);
    }
}
void refresh_path() { build_path(current_directory, cwd, sizeof(cwd)); }
const char* argument(char* line) { while (*line && *line != ' ') ++line; while (*line == ' ') ++line; return line; }
Entry* find(const char* name, uint32_t parent) { for (uint32_t i = 0; i < max_entries; ++i) if (entries[i].used && entries[i].parent == parent && equals(entries[i].name, name)) return &entries[i]; return nullptr; }
Entry* create(const char* name, bool directory, uint32_t parent) {
    if (!*name || length(name) >= name_size || find(name, parent)) return nullptr;
    for (uint32_t i = 0; i < max_entries; ++i) if (!entries[i].used) { entries[i].used = true; entries[i].directory = directory; entries[i].parent = parent; copy(entries[i].name, name, name_size); return &entries[i]; }
    return nullptr;
}
void add_user(const char* name) {
    if (!*name || length(name) >= name_size) { console::write_line("Invalid username.", 0x0C); return; }
    for (uint32_t i = 0; i < max_users; ++i) if (users[i].used && equals(users[i].name, name)) { console::write_line("User already exists.", 0x0C); return; }
    for (uint32_t i = 0; i < max_users; ++i) if (!users[i].used) {
        Entry* home = create(name, true, home_directory);
        if (!home) { console::write_line("Unable to create home folder.", 0x0C); return; }
        users[i].used = true; copy(users[i].name, name, name_size); users[i].home = static_cast<uint32_t>(home - entries);
        console::write_line("User added.", 0x0B); return;
    }
    console::write_line("User limit reached.", 0x0C);
}
void switch_user(const char* name) {
    for (uint32_t i = 0; i < max_users; ++i) if (users[i].used && equals(users[i].name, name)) {
        previous_directory = current_directory; current_user = i; current_directory = users[i].home; refresh_path(); return;
    }
    console::write_line("User not found.", 0x0C);
}
void write_prompt_path() {
    char home_path[64]; build_path(users[current_user].home, home_path, sizeof(home_path));
    uint32_t home_length = length(home_path); uint32_t path_length = length(cwd);
    bool inside_home = equals(cwd, home_path) || (path_length > home_length && cwd[home_length] == '/' && starts_with(cwd, home_path));
    if (inside_home) { console::write("~", 0x0D); if (path_length > home_length) console::write(cwd + home_length, 0x0D); }
    else console::write(cwd, 0x0D);
}
void help() {
    console::write_line("Commands:", 0x0D);
    console::write_line("  help clear version about echo pwd ls cd", 0x0B);
    console::write_line("  add user <name>   switch user <name>", 0x0B);
    console::write_line("  go back           go home           go root", 0x0B);
    console::write_line("  touch mkdir rmdir write append cat rm", 0x0B);
    console::write_line("  devices mem reboot shutdown", 0x0B);
}
void list() {
    for (uint32_t i = 0; i < max_entries; ++i) if (entries[i].used && entries[i].parent == current_directory) { console::write(entries[i].name); console::write_line(entries[i].directory ? "/" : ""); }
}
void change_directory(const char* target) {
    uint32_t old_directory = current_directory;
    if (equals(target, "/")) current_directory = 0;
    else if (equals(target, "..")) current_directory = entries[current_directory].parent;
    else {
        uint32_t parent = current_directory;
        if (*target == '/') { parent = 0; ++target; }
        Entry* entry = find(target, parent);
        if (!entry || !entry->directory) { console::write_line("Directory not found.", 0x0C); return; }
        current_directory = static_cast<uint32_t>(entry - entries);
    }
    previous_directory = old_directory;
    refresh_path();
}
void go_back() { uint32_t swap = current_directory; current_directory = previous_directory; previous_directory = swap; refresh_path(); }
void go_home() { previous_directory = current_directory; current_directory = users[current_user].home; refresh_path(); }
void go_root() { previous_directory = current_directory; current_directory = 0; refresh_path(); }
void remove_entry(const char* name) { Entry* entry = find(name, current_directory); if (!entry) { console::write_line("File not found.", 0x0C); return; } entry->used = false; }
void write_file(const char* command, bool append) {
    const char* name = command; while (*name == ' ') ++name;
    char file_name[name_size]; uint32_t index = 0; while (name[index] && name[index] != ' ' && index + 1 < name_size) { file_name[index] = name[index]; ++index; } file_name[index] = 0;
    const char* text = name + index; while (*text == ' ') ++text;
    Entry* entry = find(file_name, current_directory); if (!entry && !append) entry = create(file_name, false, current_directory);
    if (!entry || entry->directory) { console::write_line("Unable to write file.", 0x0C); return; }
    uint32_t offset = append ? length(entry->data) : 0; uint32_t available = data_size - offset;
    for (uint32_t i = 0; i + 1 < available && text[i]; ++i) entry->data[offset + i] = text[i];
    entry->data[data_size - 1] = 0; console::write_line("OK", 0x0B);
}
uint32_t complete(char* buffer, uint32_t input_length, uint32_t capacity) {
    uint32_t start = input_length;
    while (start > 0 && buffer[start - 1] != ' ') --start;
    uint32_t prefix_length = input_length - start;
    if (prefix_length == 0) return input_length;
    char prefix[name_size];
    for (uint32_t i = 0; i < prefix_length && i + 1 < name_size; ++i) prefix[i] = buffer[start + i];
    prefix[prefix_length < name_size ? prefix_length : name_size - 1] = 0;
    char common[name_size]; uint32_t common_length = 0; bool found = false;
    for (uint32_t i = 0; i < max_entries; ++i) if (entries[i].used && entries[i].parent == current_directory) {
        bool matches = true; for (uint32_t j = 0; j < prefix_length; ++j) if (entries[i].name[j] != prefix[j]) matches = false;
        if (!matches) continue;
        if (!found) { copy(common, entries[i].name, name_size); common_length = length(common); found = true; }
        else { uint32_t j = 0; while (j < common_length && entries[i].name[j] == common[j]) ++j; common_length = j; common[j] = 0; }
    }
    if (!found || common_length <= prefix_length) return input_length;
    uint32_t new_length = input_length;
    while (new_length < start + common_length && new_length + 1 < capacity) {
        buffer[new_length] = common[new_length - start];
        ++new_length;
    }
    return new_length;
}
void command(char* line) {
    if (!*line) return;
    if (equals(line, "help")) help();
    else if (equals(line, "clear")) console::clear();
    else if (equals(line, "version") || equals(line, "about")) console::write_line("KyronOS 27 Alpha 1", 0x0D);
    else if (equals(line, "go back")) go_back();
    else if (equals(line, "go home")) go_home();
    else if (equals(line, "go root")) go_root();
    else if (line[0] == 'a' && line[1] == 'd' && line[2] == 'd' && line[3] == ' ' && line[4] == 'u' && line[5] == 's' && line[6] == 'e' && line[7] == 'r' && line[8] == ' ') add_user(line + 9);
    else if (line[0] == 's' && line[1] == 'w' && line[2] == 'i' && line[3] == 't' && line[4] == 'c' && line[5] == 'h' && line[6] == ' ' && line[7] == 'u' && line[8] == 's' && line[9] == 'e' && line[10] == 'r' && line[11] == ' ') switch_user(line + 12);
    else if (line[0] == 'c' && line[1] == 'd' && line[2] == ' ') change_directory(argument(line));
    else if (equals(line, "pwd")) console::write_line(cwd, 0x0B);
    else if (equals(line, "ls")) list();
    else if (equals(line, "devices")) console::write_line("CPU: x86 | Keyboard: PS/2 | Storage: RAM KSFS", 0x0B);
    else if (equals(line, "mem")) console::write_line("Memory information unavailable in Alpha 1 bootstrap.", 0x0D);
    else if (equals(line, "reboot")) { asm volatile("cli; hlt"); }
    else if (equals(line, "shutdown")) { asm volatile("cli; hlt"); }
    else if (line[0] == 'e' && line[1] == 'c' && line[2] == 'h' && line[3] == 'o' && line[4] == ' ') console::write_line(argument(line));
    else if (line[0] == 'c' && line[1] == 'a' && line[2] == 't' && line[3] == ' ') { Entry* entry = find(argument(line), current_directory); if (entry && !entry->directory) console::write_line(entry->data); else console::write_line("File not found.", 0x0C); }
    else if (line[0] == 'l' && line[1] == 's' && line[2] == ' ') list();
    else if (line[0] == 't' && line[1] == 'o' && line[2] == 'u' && line[3] == 'c' && line[4] == 'h' && line[5] == ' ') console::write_line(create(argument(line), false, current_directory) ? "OK" : "Unable to create file.", 0x0B);
    else if (line[0] == 'm' && line[1] == 'k' && line[2] == 'd' && line[3] == 'i' && line[4] == 'r' && line[5] == ' ') console::write_line(create(argument(line), true, current_directory) ? "OK" : "Unable to create directory.", 0x0B);
    else if (line[0] == 'r' && line[1] == 'm' && line[2] == ' ') remove_entry(argument(line));
    else if (line[0] == 'r' && line[1] == 'm' && line[2] == 'd' && line[3] == 'i' && line[4] == 'r' && line[5] == ' ') remove_entry(argument(line));
    else if (line[0] == 'w' && line[1] == 'r' && line[2] == 'i' && line[3] == 't' && line[4] == 'e' && line[5] == ' ') write_file(argument(line), false);
    else if (line[0] == 'a' && line[1] == 'p' && line[2] == 'p' && line[3] == 'e' && line[4] == 'n' && line[5] == 'd' && line[6] == ' ') write_file(argument(line), true);
    else console::write_line("Command not found.", 0x0C);
}
}

namespace shell {
[[noreturn]] void run() {
    entries[0].used = true; entries[0].directory = true; entries[0].parent = 0; copy(entries[0].name, "/", name_size);
    create("kyron", true, 0);
    create("boot", true, 0);
    create("system", true, 0);
    create("etc", true, 0);
    Entry* home = create("home", true, 0);
    home_directory = static_cast<uint32_t>(home - entries);
    Entry* kyron_home = create("kyron", true, home_directory);
    users[0].used = true; users[0].home = static_cast<uint32_t>(kyron_home - entries); copy(users[0].name, "kyron", name_size);
    create("tmp", true, 0);
    create("dev", true, 0);
    create("bin", true, 0);
    refresh_path();
    char line[128];
    console::write_line("Welcome to KyronOS 27 Alpha 1.", 0x0F);
    console::write_line("Type 'help' for available commands.", 0x0B);
    while (true) {
        console::write("[", 0x0D);
        console::write(hostname, 0x0D);
        console::write("]:", 0x0D);
        write_prompt_path();
        console::write("# ", 0x0D);
        keyboard::read_line(line, sizeof(line), complete);
        command(line);
    }
}
}
