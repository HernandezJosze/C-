//
// Created by Hernandez on 4/18/2021.
//

#ifndef SIMULADORWEBDECODIGOC_LEXER_H
#define SIMULADORWEBDECODIGOC_LEXER_H
#include <map>
#include <cctype>
#include <cstdlib>
#include <vector>
#include <string_view>
enum token{
   IF,
   ELSE,
   FOR,
   DO,
   WHILE,
   INT,
   FLOAT,
   BREAK,
   CONTINUE,
   MAS,
   MENOS,
   MULTIPLICACION,
   MODULO,
   DIVISION,
   MENOR,
   MAYOR,
   MENOR_IGUAL,
   MAYOR_IGUAL,
   DIFERENTE,
   IGUAL,
   ASIGNACION,
   CORCHETE_I,
   CORCHETE_D,
   LLAVE_I,
   LLAVE_D,
   PARENTESIS_I,
   PARENTESIS_D,
   COMA,
   PUNTO_Y_COMA,
   LITERAL_ENTERA,
   LITERAL_FLOTANTE,
   LITERAL_CADENA,
   IDENTIFICADOR,
   RETURN,
   END_FILE
};
struct token_anotada{
   token tipo;
   std::string_view location;
};

std::map<std::string_view, token> AlphaMp{
        {"if", IF},
        {"else", ELSE},
        {"for", FOR},
        {"do", DO},
        {"while", WHILE},
        {"int", INT},
        {"float", FLOAT},
        {"break", BREAK},
        {"continue", CONTINUE},
        {"return", RETURN}
};
std::map<std::string_view, token> SymbolsMp{
        {"+", MAS},
        {"-", MENOS},
        {"*", MULTIPLICACION},
        {"%", MODULO},
        {"/", DIVISION},
        {"<", MENOR},
        {">", MAYOR},
        {"<=", MENOR_IGUAL},
        {">=", MAYOR_IGUAL},
        {"!=", DIFERENTE},
        {"==", IGUAL},
        {"=", ASIGNACION},
        {"[", CORCHETE_I},
        {"]", CORCHETE_D},
        {"{", LLAVE_I},
        {"}", LLAVE_D},
        {"(", PARENTESIS_I},
        {")", PARENTESIS_D},
        {",", COMA},
        {";", PUNTO_Y_COMA},
};
bool isYetId(const char& c){
   return std::isalnum(c) || c == '_';
}
bool isAStartId(const char& c){
    return std::isalpha(c) || c == '_';
}
bool isSpace(const char c){
   return std::isblank(c) || c == 13 || c == 10;
}
bool isIntoAlphaMp(std::vector<token_anotada>& v, const char *ptr){
   std::string s;
   const char *ptr_end = ptr;
   while(std::isalpha(*ptr_end)) {
      s.push_back(*ptr_end++);
      auto fnd = AlphaMp.find(s);
      if (fnd != AlphaMp.end( )) {
         if (!isYetId(*(ptr_end))) {
            v.push_back({fnd->second, std::string_view(ptr, ptr_end)});
            return true;
         }
         return false;
      }
   }
   return false;
}
bool isIntoSymbolsMp(std::vector<token_anotada>& v, const char *ptr){
   std::string s;
   const char *ptr_end = ptr;
   while(!std::isalnum(*ptr_end)){
      s.push_back(*ptr_end++);
      auto fnd = SymbolsMp.find(s);
      if(fnd != SymbolsMp.end( ) && SymbolsMp.find(s + *ptr_end) == SymbolsMp.end( )){
         v.push_back({fnd->second, std::string_view(ptr, ptr_end)});
         return true;
      }
   }
   return false;
}
bool isLiteralCad(std::vector<token_anotada>& v, const char *ptr, char type) {
   const char* ptr_end = ++ptr;
   while(*ptr_end != type || (*(ptr_end - 1) == '\\' && *(ptr_end - 2) != '\\')) {
      if(*ptr_end == '\0'){
         return false;
      }
      ++ptr_end;
   }
   if(ptr_end - ptr >= 2 && type == '\'' && *ptr != '\\'){
      return false;
   }
   v.push_back({LITERAL_CADENA, std::string_view(ptr, ptr_end)});
   return true;
}
bool isNumeric(std::vector<token_anotada>& v, const char *ptr){
   auto ptr_endi = const_cast<char*>(ptr), ptr_endf = ptr_endi;
   auto resi = strtol(ptr, &ptr_endi, 0);
   auto resf = strtod(ptr, &ptr_endf);
   if (ptr_endi == ptr && ptr_endf == ptr) {
      return false;
   }
   v.push_back({(ptr_endf > ptr_endi ? LITERAL_FLOTANTE : LITERAL_ENTERA), std::string_view(ptr, std::max(ptr_endi, ptr_endf))});
   return true;
}
bool id(std::vector<token_anotada>& v,const char *ptr){
   const char *ptr_end = ptr;
   while(isYetId(*ptr_end)){
      ++ptr_end;      //std::cout << "id: " << *ptr_end << "\n";
   }
   std::string s = {*ptr_end};
   if(isSpace(*ptr_end) || (SymbolsMp.find(s) != SymbolsMp.end( ))){
      v.push_back({IDENTIFICADOR, std::string_view(ptr, ptr_end)});
      return true;
   }
   return false;
}

std::vector<token_anotada> lexer(const char* ptr){
   std::vector<token_anotada> vectorLexer;
   while(*ptr != '\0'){
      if(isSpace(*ptr)){
         ++ptr;
      }else if((*ptr == '\"' || *ptr == '\'') && isLiteralCad(vectorLexer, ptr, *ptr)) {
         std::advance(ptr, vectorLexer.back( ).location.size( ) + 2);

      }else if(isNumeric(vectorLexer, ptr)){
         std::advance(ptr, vectorLexer.back( ).location.size( ));

      }else if(isIntoAlphaMp(vectorLexer, ptr) || isIntoSymbolsMp(vectorLexer, ptr)){
         std::advance(ptr, vectorLexer.back( ).location.size( ));

      }else if(isAStartId(*ptr) && id(vectorLexer, ptr)){
         std::advance(ptr, vectorLexer.back().location.size( ));

      }else{
            throw std::runtime_error("ERROR LEXER");
      }
   }
   vectorLexer.push_back({END_FILE, std::string_view(ptr, ptr + 1)});
   return vectorLexer;
}

#endif //SIMULADORWEBDECODIGOC_LEXER_H
