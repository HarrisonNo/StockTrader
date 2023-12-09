#include "directory_file_saving.h"
#include <filesystem>
/*
https://en.cppreference.com/w/cpp/filesystem
https://cplusplus.com/doc/tutorial/files/
https://en.cppreference.com/w/cpp/filesystem
*/

bool check_and_create_dirs(std::string path) {
    std::error_code ec;
    std::filesystem::path current_path = std::filesystem::current_path(ec);

    //ec will have some value on fail, zero on success
    if (ec.value()) {
        return false;
    }

    current_path.append(path);

    if (std::filesystem::exists(current_path)) {
        return true;
    }

    std::filesystem::create_directories(current_path, ec);

    if (ec.value()) {
        return false;
    }

    return true;
}