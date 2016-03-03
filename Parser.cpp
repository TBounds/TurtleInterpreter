#include "Parser.h"
#include <sstream>
#include <stdexcept>

void Parser::match(Token tok) {
  if (tok != lookahead_) {
    std::stringstream ss;
    ss << "Unexpected token '" << tokenToString(lookahead_) << "', ";
    ss << "Expecting '" << tokenToString(tok) << "'";
    throw std::runtime_error(ss.str());
  }
  lookahead_ = scanner_.nextToken(attribute_, lineno_);
}


void Parser::parse() {
  lookahead_ = scanner_.nextToken(attribute_, lineno_);
  try {
    prog();
  } catch(const std::exception& error) {
    std::stringstream ss;
    ss << lineno_ << ": " << error.what();
    throw std::runtime_error(ss.str());
  }
}


void Parser::prog() {
  stmt_seq();
  match(Token::EOT);
}



void Parser::stmt_seq() {
  while (lookahead_ != Token::EOT) {
    Stmt *s = block();
    AST_.push_back(s);
  }
}


Stmt *Parser::stmt() {
  // Call the appropiate statement function based on the lookahead token.
  switch(lookahead_){
    case Token::IDENT:  match(Token::IDENT); return assign();
    case Token::WHILE:  match(Token::WHILE); return while_stmt();
    case Token::IF:     match(Token::IF);    return if_stmt();
    default:                                 return action();                 
  }
}


Stmt *Parser::assign() {
  std::string name = attribute_.s;
  match(Token::ASSIGN);
  Expr *e = expr();
  return new AssignStmt(name, e);
}


Stmt *Parser::block() {
  
  std::vector<Stmt*> stmtTree;
  do{
    Stmt *s = stmt();
    stmtTree.push_back(s);
  }
  while( lookahead_ == Token::WHILE ||
         lookahead_ == Token::IF ||
         lookahead_ == Token::IDENT ||
         lookahead_ == Token::HOME ||
         lookahead_ == Token::PENUP ||
         lookahead_ == Token::PENDOWN ||
         lookahead_ == Token::FORWARD ||
         lookahead_ == Token::LEFT ||
         lookahead_ == Token::RIGHT ||
         lookahead_ == Token::PUSHSTATE ||
         lookahead_ == Token::POPSTATE);
  return new BlockStmt(stmtTree);
}


Stmt *Parser::while_stmt() {
  Expr *cond = _bool();
  match(Token::DO);
  Stmt *body = block();
  match(Token::OD);
  return new WhileStmt(cond, body);
}


Stmt *Parser::else_part() {
  switch(lookahead_){
    case Token::ELSIF:{
      match(Token::ELSIF);  
      Expr *cond = _bool();
      match(Token::THEN);
      Stmt *body = block();
      Stmt *elsePart = else_part();
      return new IfStmt(cond, body, elsePart); 
    }
    case Token::ELSE:{
      match(Token::ELSE);
      Stmt *body = block();
      match(Token::FI);
      return body; 
    }
    case Token::FI:{
      match(Token::FI);
      return nullptr;
    }
    default:
      throw std::runtime_error("Expecting turtle else_part statement!");
  }
}


Stmt *Parser::if_stmt() {
  Expr *cond = _bool();
  match(Token::THEN);
  Stmt *body = block();
  Stmt *elsePart = else_part();
  return new IfStmt(cond, body, elsePart);
}


Stmt *Parser::action() {
  switch(lookahead_) {
    case Token::HOME:    match(Token::HOME);    return new HomeStmt();
    case Token::PENUP:   match(Token::PENUP);   return new PenUpStmt();
    case Token::PENDOWN: match(Token::PENDOWN); return new PenDownStmt();
    case Token::FORWARD: match(Token::FORWARD); return new ForwardStmt(expr());
    case Token::LEFT:    match(Token::LEFT);    return new LeftStmt(expr());
    case Token::RIGHT:   match(Token::RIGHT);   return new RightStmt(expr());
    case Token::PUSHSTATE: 
      match(Token::PUSHSTATE); return new PushStateStmt();
    case Token::POPSTATE:
      match(Token::POPSTATE); return new PopStateStmt();
    default:
      throw std::runtime_error("Expecting turtle action statement!");
  }
}


Expr *Parser::expr() {
  Expr *e = term();
  while (lookahead_ == Token::PLUS ||
	 lookahead_ == Token::MINUS) {
    const auto op = lookahead_;
    match(lookahead_);
    Expr *t = term();
    if (op == Token::PLUS)
      e = new AddExpr(e, t);
    else
      e = new SubExpr(e, t);
  }
  return e;
}


Expr *Parser::term() {
  
  Expr *e = factor();
  while (lookahead_ == Token::MULT || lookahead_ == Token::DIV){
    Token op = lookahead_;
    match(lookahead_);
    Expr *t = factor();
    if (op == Token::MULT)
      e = new MulExpr(e, t);
    else
      e = new DivExpr(e, t);
  }
  return e;
}


Expr *Parser::factor() {
  
  switch(lookahead_) {
    case Token::PLUS:   match(Token::PLUS); return factor();
    case Token::MINUS:  match(Token::MINUS); return new NegExpr(factor());
    case Token::LPAREN:
      {
        match(Token::LPAREN);
        Expr *e = expr();
        match(Token::RPAREN);
        return e;
      }
    case Token::IDENT:
      {
        const std::string name = attribute_.s;
        match(Token::IDENT);
        return new VarExpr(name);
      }
    case Token::REAL:
      {
        const float val = attribute_.f;
        match(Token::REAL);
        return new ConstExpr(val);
      }
    default:
      throw std::runtime_error("Expecting factor!");
  }
}


Expr *Parser::_bool() {
  
  Expr *e = bool_term();
  while (lookahead_ == Token::OR){
    match(Token::OR);
    Expr *t = bool_term();
  e = new OrBool(e, t);
  }
  
  return e;
}


Expr *Parser::bool_term() {
  Expr *e = bool_factor();
  while(lookahead_ == Token::AND){
    Expr *t = bool_factor();
    e = new AndBool(e, t);
  }
  
  return e;
}


Expr *Parser::bool_factor() {
  switch(lookahead_){
    case Token::NOT: {match(Token::NOT); return new NotExpr(bool_factor());}
    case Token::LPAREN:{
      match(Token::LPAREN);
      Expr *e = _bool();
      match(Token::RPAREN);
      return e;
    }
    default: {
      Expr *e = cmp();
      return e;
    }
  }
}


Expr *Parser::cmp() {
  
  Expr *e = expr();
  switch(lookahead_){
    case Token::EQ:  {match(Token::EQ); Expr *s = expr(); return new e_q(e, s);}
    case Token::NE:  {match(Token::NE); Expr *s = expr(); return new n_e(e, s);}
    case Token::LT:  {match(Token::LT); Expr *s = expr(); return new l_t(e, s);}
    case Token::GT:  {match(Token::GT); Expr *s = expr(); return new g_t(e, s);}
    case Token::GE:  {match(Token::GE); Expr *s = expr(); return new g_e(e, s);}
    case Token::LE:  {match(Token::LE); Expr *s = expr(); return new l_e(e, s);}
    default:{
      throw std::runtime_error("Error cmp().");
    }
  }
}