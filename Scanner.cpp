#include "Scanner.h"
#include <locale>
#include <sstream>
#include <map>
#include <string>
#include <cstdlib>
#include <cstdio>

Token Scanner::nextToken(Attribute& attr, int& lineno) {

  //
  // Return next token / attribute (track line number).
  //
  lineno++;
  //
  // Eat whitespace and comments
  //
 int c;
 top:
  do {
    c = in_.get();
  } while (std::isspace(c));

  if (c == '#') {
    do {
      c = in_.get();
      if (c == EOF) return Token::EOT;
    } while (c != '\n');
    goto top;
  }

  if (c == EOF) return Token::EOT;
  if (c == '+') return Token::PLUS;
  if (c == '-') return Token::MINUS;
  if (c == '*') return Token::MULT;
  if (c == '/') return Token::DIV;
  if (c == '(') return Token::LPAREN;
  if (c == ')') return Token::RPAREN;;

  if (c == ':') {  // assign :=
    c = in_.get();
    if (c != '=')
      throw std::runtime_error("Unknown Lexeme");
    return Token::ASSIGN;
  }
  
  //
  // REAL
  //
  if (std::isdigit(c)) {
    std::string buf = "";
    do {
      buf.push_back(c);
      c = in_.get();
    } while (std::isdigit(c));
    if (c == '.') {
      buf.push_back(c);
      c = in_.get();
      while (std::isdigit(c)) {
	buf.push_back(c);
	c = in_.get();
      }
    }
    in_.unget();
    // attribute.f = std::stod(buf);
    attr.f = std::strtod(buf.c_str(), nullptr);
    return Token::REAL;
  }

  //
  // IDENT or a reserved word
  //
  if (std::isalpha(c) || c == '_') {
    std::string buf = "";
    do {
      buf.push_back(c);
      c = in_.get();
    } while (std::isalnum(c) || c == '_');
    in_.unget();
    
    static const std::map<std::string,Token> reserved = {
      {"HOME", Token::HOME},
      {"PENUP", Token::PENUP},
      {"PENDOWN", Token::PENDOWN},
      {"FORWARD", Token::FORWARD},
      {"LEFT", Token::LEFT},
      {"RIGHT", Token::RIGHT},
      {"PUSHSTATE", Token::PUSHSTATE},
      {"POPSTATE", Token::POPSTATE},
      {"OR", Token::OR},
      {"AND", Token::AND},
      {"NOT", Token::NOT},
      {"WHILE", Token::WHILE},
      {"DO", Token::DO},
      {"OD", Token::OD},
      {"IF", Token::IF},
      {"FI", Token::FI},
      {"THEN", Token::THEN},
      {"ELSIF", Token::ELSIF},
      {"ELSE", Token::ELSE},
    };
    auto p = reserved.find(buf);
    if (p != reserved.end())
      return p->second;

    attr.s = buf;
    return Token::IDENT;
  }
  
  //
  //  NE, LE, GE, GT, LT, EQ
  //
  if(c == '<'){
    c = in_.get();
    if(c == '>')
      return Token::NE;
    else if(c == '=')
      return Token::LE;
    else{
      in_.unget();
      return Token::LT;
    }
  }
  else if(c == '>'){
    c = in_.get();
    if(c == '=')
      return Token::GE;
    else{
      in_.unget();
      return Token::GT;
    }
  }
  
  if(c == '=')
    return Token::EQ;

  throw std::runtime_error("Unknown Lexeme");
  return Token::UNKNOWN;
}

std::string tokenToString(Token token) {
  static std::map<Token,std::string> tokenMap = {
    {Token::UNKNOWN, "UNKNOWN"},
    {Token::EOT,"EOT"},
    {Token::IDENT, "IDENT"},
    {Token::REAL, "REAL"},
    {Token::ASSIGN, "ASSIGN"},
    {Token::EQ, "EQ"},
    {Token::NE, "NE"},
    {Token::LE, "LE"},
    {Token::LT, "LT"},
    {Token::GE, "GE"},
    {Token::GT,"GT"},
    {Token::LPAREN, "LPAREN"},
    {Token::RPAREN, "RPAREN"},
    {Token::OR, "OR"},
    {Token::AND, "AND"},
    {Token::NOT, "NOT"},
    {Token::PLUS, "PLUS"},
    {Token::MINUS, "MINUS"},
    {Token::MULT, "MULT"},
    {Token::DIV,"DIV"},
    {Token::IF, "IF"},
    {Token::THEN, "THEN"},
    {Token::ELSIF, "ELSIF"},
    {Token::ELSE, "ELSE"},
    {Token::FI,"FI"},
    {Token::WHILE, "WHILE"},
    {Token::DO, "DO"},
    {Token::OD,"OD"},
    {Token::HOME, "HOME"},
    {Token::PENUP, "PENUP"},
    {Token::PENDOWN, "PENDOWN"},
    {Token::FORWARD, "FORWARD"},
    {Token::LEFT, "LEFT"},
    {Token::RIGHT, "RIGHT"},
    {Token::PUSHSTATE, "PUSHSTATE"},
    {Token::POPSTATE,"POPSTATE"}
  };
  auto p = tokenMap.find(token);
  if (p != tokenMap.end())
    return p->second;
  return "?";
}