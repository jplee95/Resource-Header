// Copyright 2023 Jordan Paladino
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this
// software and associated documentation files (the "Software"), to deal in the Software
// without restriction, including without limitation the rights to use, copy, modify,
// merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be included in all copies
// or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
// CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>

inline constexpr const char*
cstrend(const char* str) {
    while(*str != 0) str++;
    return str;
}
#define CSTR_BEGIN_END(__cstr) __cstr, cstrend(__cstr)

int
main(int argc, const char** args) {
    if(argc == 1) {
        std::cout << "Copyright (c) Jordan Paladino\n" << "resourceheader <file>... [- <out>]" << std::endl;
        return EXIT_FAILURE;
    }
    
    bool is_output = false;
    std::vector<std::string> files;
    std::string out;
    for(int i = 1; i < argc; i++) {
        if(std::equal(CSTR_BEGIN_END(args[i]), CSTR_BEGIN_END("-"))) {
            is_output = true;
            continue;
        }
        if(is_output) {
            if(!out.empty()) {
                std::cout << "Too many args for output" << std::endl;
                return EXIT_FAILURE;
            }
            out = args[i];
            continue;
        }
        files.emplace_back(args[i]);
    }
    
    if(!std::filesystem::exists(out)) {
        std::filesystem::create_directories(out);
    } else if(!std::filesystem::is_directory(out)) {
        std::cout << "Given out path is not a directory" << std::endl;
        return EXIT_FAILURE;
    }
    
    for(const auto& file: files) {
        if(!std::filesystem::exists(file)) {
            std::cout << "File does not exist" << std::endl;
            return EXIT_FAILURE;
        }
    
        std::stringstream stream;
        std::ifstream input(file, std::ios::in | std::ios::binary);
        if(!input.is_open( )) {
            std::cout << "File could not be opened" << std::endl;
            return EXIT_FAILURE;
        }
    
        std::string file_name = std::filesystem::path(file).filename( ).string( );
        auto found = file_name.find('.');
        if(found != std::string::npos)
            file_name = file_name.substr(0, found);
    
        std::string item_name = file_name;
        for(auto& item: file_name) item = (char) std::toupper(item);
    
        stream << "#ifndef " << file_name << "_H\n";
        stream << "inline constexpr unsigned char " << item_name << "[] {";
    
        static constexpr int width = 16;
        int len = width;
        while(!input.eof( )) {
            auto c = input.get( );
            if(c == -1) break;
        
            if(len == width) {
                len = 0;
                stream << "\n" << std::setfill(' ') << std::setw(3) << " " << std::setfill('0');
            }
            len++;
            stream << " 0x" << std::hex << std::setw(2) << c << ",";
        }
        input.close( );
    
        stream << "\n};\n";
        stream << "#endif" << "\n";
    
        std::ofstream output(std::filesystem::path(file).parent_path( ).append(item_name + ".h"));
        if(!output.is_open( )) {
            std::cout << "Unable to write file to directory" << std::endl;
            return EXIT_FAILURE;
        }
        output << stream.str( );
        output.close( );
    }
    
    return EXIT_SUCCESS;
}
