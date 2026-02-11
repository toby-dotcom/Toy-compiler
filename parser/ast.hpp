#pragma once
#include <string>
#include <memory>
#include <iostream>
namespace ast {

struct Node { virtual ~Node()=default; virtual void print(int indent=0) const=0; };

struct Number : Node {
    double value; Number(double v): value(v){}
    void print(int indent=0) const override { std::cout<<std::string(indent,' ')<<"Number("<<value<<")\n"; }
};

struct Identifier : Node {
    std::string name; Identifier(const std::string& n): name(n){}
    void print(int indent=0) const override { std::cout<<std::string(indent,' ')<<"Identifier("<<name<<")\n"; }
};

struct BinaryOp : Node {
    std::string op; std::unique_ptr<Node> left,right;
    BinaryOp(std::unique_ptr<Node> l,const std::string& o,std::unique_ptr<Node> r): left(std::move(l)),op(o),right(std::move(r)){}
    void print(int indent=0) const override { 
        std::cout<<std::string(indent,' ')<<"BinaryOp("<<op<<")\n";
        left->print(indent+2);
        right->print(indent+2);
    }
};

struct Assignment : Node {
    std::string name; std::unique_ptr<Node> expr;
    Assignment(const std::string& n,std::unique_ptr<Node> e): name(n),expr(std::move(e)){}
    void print(int indent=0) const override {
        std::cout<<std::string(indent,' ')<<"Assignment("<<name<<")\n";
        expr->print(indent+2);
    }
};

} // namespace ast

