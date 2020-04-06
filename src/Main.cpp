#include "Lexer.h"
#include "Terminal.h"
#include "external/CLI11.hpp"
#include "external/linenoise.hpp"
#include <cstdio>

int main(int argc, char** argv)
{
    term_init();
    
    CLI::App app { "Sen programming language" };
    
    bool print_tokens = false;
    app.add_flag("-t,--tokens", print_tokens, "Print generated tokens");
    
    bool print_ast = false;
    app.add_flag("-a,--ast", print_ast, "Print Abstract Syntax Tree");
    
    std::string filename = "";
    app.add_option("file", filename, "Source path")->check(CLI::ExistingFile);
    
    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        if (e.get_exit_code() != 0) {
            term_color_stderr(Color::FG_BR_RED);
        }
        return app.exit(e);
    }
    
    if (filename == "") {
        Lexer lexer;
        while (true) {
            std::string line;
            bool quit = linenoise::Readline("> ", line);
            if (quit) {
                break;
            }
            
            lexer.init();
            lexer.set_input_from_string(line);
            lexer.lex();
            
            if (print_tokens) {
                lexer.print_info();
            }
            
            lexer.free();
        }
    } else {
        FILE* file = fopen(filename.c_str(), "rb");
        if (file == nullptr) {
            term_error("unable to open %s\n", filename.c_str());
            return 1;
        } else {
            Lexer lexer;
            lexer.init();
            lexer.set_input_from_file(file);
            fclose(file);
            
            lexer.lex();
            
            term_info("Lexed %d lines in %.2fms (%.f kloc/s)\n", lexer.processed_lines, lexer.elapsed,
                      (double)lexer.processed_lines / (lexer.elapsed / 1000.0) / 1000.0);
            
            if (print_tokens) {
                lexer.print_info();
            }
            
            lexer.free();
        }
        
        return 0;
    }
}
