#ifndef AST_H
#define AST_H

#include <string>
#include <iostream>
#include <vector>
#include <iomanip>
#include "Env.h"

//
// Abstract base class for all expressions.
//
class Expr {
public:
  virtual ~Expr() {}
  virtual float eval(Env& env) const = 0;
};

//
// Abstract base class for all statements.
//
class Stmt {
public:
  virtual ~Stmt() {};
  virtual void execute(Env& env) = 0;
};

//
// AST's expressions and statements go here.
//

class AssignStmt : public Stmt {
protected:
  const std::string _name;  //l-value
  Expr *_expr; // r-value
public:
  AssignStmt(const std::string& n, Expr *e) : _name{n}, _expr{e} {}
  virtual void execute(Env& env) {
    env.put(_name, _expr->eval(env));
  }
};

class HomeStmt : public Stmt {
public:
  virtual void execute(Env& env) {
    std::cout << "H" << std::endl;
  }
};

class PenUpStmt : public Stmt {
public:
  virtual void execute(Env& env) {
    std::cout << "U" << std::endl;
  }
};

class PenDownStmt : public Stmt {
public:
  virtual void execute(Env& env) {
    std::cout << "D" << std::endl;
  }
};

class PushStateStmt : public Stmt {
public:
  virtual void execute(Env& env) {
    std::cout << "[" << std::endl;
  }
};

class PopStateStmt : public Stmt {
public:
  virtual void execute(Env& env) {
    std::cout << "]" << std::endl;
  }
};

class ForwardStmt : public Stmt {
protected:
  Expr *_dist;
public:
  ForwardStmt(Expr *e) : _dist{e} {}
  virtual void execute(Env& env) {
    const float d = _dist->eval(env);
    std::cout << "M " << d << std::endl;
  }
};

class RightStmt : public Stmt {
protected:
  Expr *_angle;
public:
  RightStmt(Expr *e) : _angle{e} {}
  virtual void execute(Env& env) {
    const float a = _angle->eval(env);
    std::cout << "R " << -a << std::endl;
  }
};

class LeftStmt : public Stmt {
protected:
  Expr *_angle;
public:
  LeftStmt(Expr *e) : _angle{e} {}
  virtual void execute(Env& env) {
    const float a = _angle->eval(env);
    std::cout << "R " << a << std::endl;
  }
};

class VarExpr : public Expr {
protected:
  const std::string _name;
public:
  VarExpr(const std::string& n) : _name{n} {}
  virtual float eval(Env& env) const {
    return env.get(_name);
  }
};

class ConstExpr : public Expr {
protected:
  const float _val;
public:
  ConstExpr(float v) : _val{v} {}
  virtual float eval(Env& env) const {
    return _val;
  }
};

class UnaryExpr : public Expr {
protected:
  Expr *_expr;
public:
  UnaryExpr(Expr *e) : _expr{e} {}
};

class NegExpr : public UnaryExpr {
public:
  NegExpr(Expr *e) : UnaryExpr(e) {}
  virtual float eval(Env& env) const {
    return -_expr->eval(env);
  }
};

class BinaryExpr : public Expr {
protected:
  Expr *_left, *_right;
public:
  BinaryExpr(Expr *l, Expr *r) : _left{l}, _right{r} {}
};

class AddExpr : public BinaryExpr {
public:
  AddExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  virtual float eval(Env& env) const {
    return _left->eval(env) + _right->eval(env);
  }
};

class SubExpr : public BinaryExpr {
public:
  SubExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  virtual float eval(Env& env) const {
    return _left->eval(env) - _right->eval(env);
  }
};

class MulExpr : public BinaryExpr {
public:
  MulExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  virtual float eval(Env& env) const {
    return _left->eval(env) * _right->eval(env);
  }
};

class DivExpr : public BinaryExpr {
public:
  DivExpr(Expr *l, Expr *r) : BinaryExpr(l,r) {}
  virtual float eval(Env& env) const {
    return _left->eval(env) / _right->eval(env);
  }
};

/************************************************/
/************************************************/

class BlockStmt : public Stmt{
protected:  
  std::vector<Stmt*> _stmts;
public:
  BlockStmt(const std::vector<Stmt*>&s): _stmts{s}{}
  virtual void execute(Env& env){
    for(int i = 0; i < (int)_stmts.size(); i++)
      _stmts[i]->execute(env);
  }
};

class WhileStmt : public Stmt{
protected:
  Expr *cond_;
  Stmt *body_;
public:
  WhileStmt(Expr *c, Stmt *b) : cond_{c}, body_{b} {}
  void execute(Env& env){
    while (cond_->eval(env) != 0)
      body_->execute(env);
  }
  ~WhileStmt() {delete cond_; delete body_;}
};

class IfStmt : public Stmt{
protected:
  Expr *cond_;
  Stmt *body_;
  Stmt *else_;
public:
  IfStmt(Expr *c, Stmt *b, Stmt *e) : cond_{c}, body_{b}, else_{e} {}
  void execute(Env& env){
    if (cond_->eval(env) != 0)
      body_->execute(env);
    else if(else_ != nullptr)
      else_->execute(env);
  }
  ~IfStmt() {delete cond_; delete body_; delete else_;}
};

class AndBool : public BinaryExpr{
  public:
    AndBool(Expr *l, Expr *r) : BinaryExpr(l,r) {}
    virtual float eval(Env& env) const{
      return _left->eval(env) && _right->eval(env);
    }
};

class OrBool : public BinaryExpr{
  public:
    OrBool(Expr *l, Expr *r) : BinaryExpr(l,r) {}
    virtual float eval(Env& env) const{
    return _left->eval(env) || _right->eval(env);
    }
};

class NotExpr: public UnaryExpr{
public:
 NotExpr(Expr *e):UnaryExpr(e){}
 virtual float eval(Env& env) const{
   return(_expr->eval(env)); 
 }
};

class e_q : public BinaryExpr{
  public:
    e_q(Expr *l, Expr *r) : BinaryExpr(l,r) {}
    virtual float eval(Env& env) const{
      return _left->eval(env) == _right->eval(env);
    }
};

class n_e : public BinaryExpr{
  public:
    n_e(Expr *l, Expr *r) : BinaryExpr(l,r) {}
    virtual float eval(Env& env) const{
      return _left->eval(env) != _right->eval(env);
    }
};

class l_t : public BinaryExpr{
  public:
    l_t(Expr *l, Expr *r) : BinaryExpr(l,r) {}
    virtual float eval(Env& env) const{
      return _left->eval(env) < _right->eval(env);
    }
};

class g_t : public BinaryExpr{
  public:
    g_t(Expr *l, Expr *r) : BinaryExpr(l,r) {}
    virtual float eval(Env& env) const{
      return _left->eval(env) > _right->eval(env);
    }
};

class g_e : public BinaryExpr{
  public:
    g_e(Expr *l, Expr *r) : BinaryExpr(l,r) {}
    virtual float eval(Env& env) const{
      return _left->eval(env) >= _right->eval(env);
    }
};

class l_e : public BinaryExpr{
  public:
    l_e(Expr *l, Expr *r) : BinaryExpr(l,r) {}
    virtual float eval(Env& env) const{
      return _left->eval(env) <= _right->eval(env);
    }
};

#endif // AST_H