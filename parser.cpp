#include "lupc.h"

ASTExprNode *parse_unary_expr(TokenNode **next, Error &err) {
}
ASTExprNode *parse_logical_or_expr(TokenNode **next, Error &err) {
}
ASTExprNode *parse_assign_expr(TokenNode **next, Error &err) {
  ASTExprNode *left = parse_logical_or_expr(next, err);
  // parse error
  if (!left) return NULL;
  if (!left->is_unary_expr()) return left;
  TokenNode *t = consume_token_with_str(next, ":");
  if (!t) return left;
  ASTAssignExprNode *ret;
  
  ret = new ASTAssignExprNode(t);
  ret->left = left;
  ret->right = parse_assign_expr(next, err);
  if (!!ret->right && )
  
  ret->expr = parse_expr(next, err);
  if (ret->expr == NULL) {
    delete ret;
    return NULL;
  }
  return ret;
}

ASTExprNode *parse_expr(TokenNode **next, Error &err) {
  return parse_assign_expr(next, err);
}

ASTExprStmtNode *parse_expr_stmt(TokenNode **next, Error &err) {
  // parse expr
  ASTExprNode *expr = parse_expr(next, err);
  // check parse error
  if (expr == NULL) return NULL;
  // token LF should be here
  TokenNode *t = expect_token_with_str(next, err, "\n");
  if (t == NULL) return NULL;
  ASTExprStmtNode *ret = new ASTExprStmtNode(t);
  ret->expr = expr;
  return ret;
}

ASTReturnStmtNode *parse_return_stmt(TokenNode **next, Error &err) {
  // token KwReturn should be here
  TokenNode *t = expect_token_with_type(next, err, KwReturn);
  if (t == NULL) return NULL;
  ASTReturnStmtNode *ret = new ASTReturnStmtNode(t);
  // parse expr
  ret->expr = parse_expr(next, err);
  // check parse error
  if (ret->expr == NULL) {
    delete ret;
    return NULL;
  }
  if (expect_token_with_str(next, err, "\n") == NULL) {
    delete ret;
    return NULL;
  }
  return ret;
}

ASTDeclaratorNode *parse_declarator(TokenNode **next, Error &err) {
  // token Ident should be here
  TokenNode *t = expect_token_with_type(next, err, Ident);
  if (t == NULL) return NULL;
  return new ASTDeclaratorNode(t);
}

ASTDeclarationNode *parse_declaration(TokenNode **next, Error &err) {
  // NULL check
  if ((*next) == NULL) {
    err = Error("expected declaration, found EOF");
    return NULL;
  }
  // parse type-specifer
  TokenNode *t;
  if (
    (t = consume_token_with_type(next, KwNum)) == NULL &&
    (t = expect_token_with_type(next, err, KwStr)) == NULL
  ) return NULL;
  ASTDeclarationNode *ret = new ASTDeclarationNode(t);
  ret->declarators = vector<ASTDeclaratorNode *>();
  while (1) {
    // parse declarator
    ASTDeclaratorNode *declarator = parse_declarator(next, err);
    // check parse error
    if (declarator == NULL) {
      for (ASTDeclaratorNode *p: ret->declarators) delete p;
      delete ret;
      return NULL;
    }
    ret->declarators.push_back(declarator);
    // declaration end
    if (consume_token_with_str(next, "\n") != NULL) break;
    // token "," should be here
    if (expect_token_with_str(next, err, ",") == NULL) {
      for (ASTDeclaratorNode *p: ret->declarators) delete p;
      delete ret;
      return NULL;
    }
  }
  return ret;
}

ASTDeclarationNode *parse_single_declaration(TokenNode **next, Error &err) {
  // NULL check
  if ((*next) == NULL) {
    err = Error("expected single-declaration, found EOF");
    return NULL;
  }
  // parse type-specifer
  TokenNode *t;
  if (
    (t = consume_token_with_type(next, KwNum)) == NULL &&
    (t = expect_token_with_type(next, err, KwStr)) == NULL
  ) return NULL;
  ASTDeclarationNode *ret = new ASTDeclarationNode(t);
  ret->declarators = vector<ASTDeclaratorNode *>();
  // parse declarator
  ASTDeclaratorNode *declarator = parse_declarator(next, err);
  // check parse error
  if (declarator == NULL) {
    delete ret;
    return NULL;
  }
  ret->declarators.push_back(declarator);
  return ret;
}

ASTCompoundStmtNode *parse_comp_stmt(TokenNode **next, Error &err, int indents);
ASTOtherStmtNode *parse_other_stmt(TokenNode **next, Error &err) {
  // NULL check
  if ((*next) == NULL) {
    err = Error("expected statement, found EOF");
    return NULL;
  }
  ASTOtherStmtNode *ret;
  switch ((*next)->type)
  {
  case KwBreak:
    ret = new ASTBreakStmtNode(consume_token_with_type(next, KwBreak));
    if (expect_token_with_str(next, err, "\n") == NULL) {
      delete ret;
      return NULL;
    }
    break;
  case KwContinue:
    ret = new ASTContinueStmtNode(consume_token_with_type(next, KwContinue));
    if (expect_token_with_str(next, err, "\n") == NULL) {
      delete ret;
      return NULL;
    }
    break;
  case KwReturn:
    ret = parse_return_stmt(next, err);
    break;
  // case KwIf:
  //   ret = parse_selection_stmt(next, err);
  //   break;
  // case KwLoop:
  //   ret = parse_loop_stmt(next, err);
  //   break;
  default:
    ret = parse_expr_stmt(next, err);
    break;
  }
  return ret;
}

ASTCompoundStmtNode *parse_comp_stmt(TokenNode **next, Error &err, int indents) {
  // NULL check
  if (expect_token_with_type(next, err, Punctuator) == NULL) return NULL;
  ASTCompoundStmtNode *ret = new ASTCompoundStmtNode(*next);
  while (1) {
    // token indent should be here
    if (*((*next)->begin) != ' ') {
      err = Error("expected indent, found ????");
      for (ASTNode *p: ret->items) delete p;
      delete ret;
      return NULL;
    }
    if (consume_token_with_indents(next, indents) == NULL) {
      if ((*next)->length > indents) {
      // inner compound-stmt
        // parse compound-stmt
        ASTCompoundStmtNode *comp_stmt = parse_comp_stmt(next, err, indents + 2);
        // check parse error
        if (comp_stmt == NULL) {
          for (ASTNode *p: ret->items) delete p;
          delete ret;
          return NULL;
        }
        ret->items.push_back(comp_stmt);
      } else {
      // compound-stmt end
        break;
      }
    } else if ((*next)->type == KwNum || (*next)->type == KwStr) {
    // declaration
      // parse declaration
      ASTDeclarationNode *declaration = parse_declaration(next, err);
      // check parse error
      if (declaration == NULL) {
        for (ASTNode *p: ret->items) delete p;
        delete ret;
        return NULL;
      }
      ret->items.push_back(declaration);
    } else {
    // other-stmt
      // parse other-stmt
      ASTOtherStmtNode *other_stmt = parse_other_stmt(next, err);
      // check parse error
      if (other_stmt == NULL) {
        for (ASTNode *p: ret->items) delete p;
        delete ret;
        return NULL;
      }
      ret->items.push_back(other_stmt);
    }
  }
  return ret;
}

ASTFuncDeclaratorNode *parse_func_declarator(TokenNode **next, Error &err) {
// ident(type ident, ...)
  // token Ident should be here
  TokenNode *t = expect_token_with_type(next, err, Ident);
  if (t == NULL) return NULL;
  ASTFuncDeclaratorNode *ret = new ASTFuncDeclaratorNode(t);
  // token "(" should be here
  if (expect_token_with_str(next, err, "(") == NULL) {
    delete ret;
    return NULL;
  }
  while (!(*next)->is_equal_with_str(")")) {
    // parse single-declaration
    ASTDeclarationNode *declaration = parse_single_declaration(next, err);
    // check parse error
    if (declaration == NULL) {
      for (ASTDeclarationNode *p: ret->args) delete p;
      delete ret;
      return NULL;
    }
    ret->args.push_back(declaration);
    // args end
    if (consume_token_with_str(next, ",") == NULL) break;
  }
  // token ")" should be here
  if (expect_token_with_str(next, err, ")") == NULL) {
    for (ASTDeclarationNode *p: ret->args) delete p;
    delete ret;
    return NULL;
  }
  return ret;
}

ASTFuncDeclarationNode *parse_func_declaration(TokenNode **next, Error &err) {
// func-declarator -> type
  // parse function-declarator
  ASTFuncDeclaratorNode *declarator = parse_func_declarator(next, err);
  // check parse error
  if (declarator == NULL) return NULL;
  // token "->" should be here
  if (expect_token_with_str(next, err, "->") == NULL) return NULL;
  // parse type-specifer
  TokenNode *t;
  if (
    (t = consume_token_with_type(next, KwNum)) == NULL &&
    (t = consume_token_with_type(next, KwStr)) == NULL &&
    (t = expect_token_with_type(next, err, KwVoid)) == NULL
  ) {
    delete declarator;
    return NULL;
  }
  // token LF should be here
  if (expect_token_with_str(next, err, "\n") == NULL) {
    delete declarator;
    return NULL;
  }
  ASTFuncDeclarationNode *ret = new ASTFuncDeclarationNode(t);
  ret->declarator = declarator;
  return ret;
}

ASTFuncDefNode *parse_func_def(TokenNode **next, Error &err) {
// func-declaration compound-stmt
  // token KwFunc should be here
  TokenNode *t = expect_token_with_type(next, err, KwFunc);
  if (t == NULL) return NULL;
  ASTFuncDefNode *ret = new ASTFuncDefNode(t);
  // parse function-declaration
  ret->declaration = parse_func_declaration(next, err);
  // check parse error
  if (ret->declaration == NULL) {
    delete ret;
    return NULL;
  }
  // parse compound-stmt
  ret->body = parse_comp_stmt(next, err, 2);
  // check parse error
  if (ret->body == NULL) {
    delete ret;
    delete ret->declaration;
    return NULL;
  }
  return ret;
}

ASTNode *parse_external_declaration(TokenNode **next, Error &err) {
  if (
    (*next) != NULL &&
    (*next)->is_equal_with_str("func")
  ) return parse_func_def(next, err);
  else return parse_declaration(next, err);
}

void init_parser(TokenNode **head_token) {
  // *head_token can be NULL
  TokenNode *next = *head_token;
  TokenNode *t, *bef;
  while (next != NULL) {
    // if next token is Delimiter, remove it and continue
    t = consume_token_with_type(&next, Delimiter);    
    if (t == NULL) break;
    delete t;
  }
  // set new head_token
  *head_token = next;
  // remove first LF punctuator
  if (*head_token != NULL) {
    t = consume_token_with_str(head_token, "\n");
    if (t != NULL) delete t;
  }
  // *head_token can be NULL
  next = *head_token;
  while (next != NULL) {
    bef = next;
    assert(bef->type != Delimiter);
    next = bef->next;
    while (next != NULL) {
      // if next token is Delimiter, remove it and continue
      t = consume_token_with_type(&next, Delimiter);
      if (t == NULL) break;
      delete t;
    }
    bef->next = next;
  }
}

ASTNode * parse(TokenNode **head_token, Error &err) {
  init_parser(head_token);
  TokenNode *next = *head_token;
  // head AST node
  ASTNode *head = parse_external_declaration(&next, err);
  for (
    ASTNode *external_declaration = head;;
    external_declaration = parse_external_declaration(&next, err)
  ) {
    // check parse error
    if (external_declaration == NULL) return NULL;
    // if it was last node, finish
    if (next == NULL) break;
  }
  return head;
}