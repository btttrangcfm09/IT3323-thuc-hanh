/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <string.h>
#include "semantics.h"
#include "error.h"

extern SymTab* symtab;
extern Token* currentToken;

Object* lookupObject(char *name) {
  // Search from current scope outward
  Scope* scope = symtab->currentScope;
  Object* obj;
  
  while (scope != NULL) {
    obj = findObject(scope->objList, name);
    if (obj != NULL) return obj;
    scope = scope->outer;
  }
  
  // Search in global object list
  obj = findObject(symtab->globalObjectList, name);
  return obj;
}

void checkFreshIdent(char *name) {
  // Check if the name is already declared in the current scope
  Object* obj = findObject(symtab->currentScope->objList, name);
  if (obj != NULL) {
    error(ERR_DUPLICATE_IDENT, currentToken->lineNo, currentToken->colNo);
  }
}

Object* checkDeclaredIdent(char* name) {
  // Check if the identifier is declared in any accessible scope
  Object* obj = lookupObject(name);
  if (obj == NULL) {
    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
  }
  return obj;
}

Object* checkDeclaredConstant(char* name) {
  // Check if the constant is declared
  Object* obj = lookupObject(name);
  if (obj == NULL) {
    error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);
  } else if (obj->kind != OBJ_CONSTANT) {
    error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);
  }
  return obj;
}

Object* checkDeclaredType(char* name) {
  // Check if the type is declared
  Object* obj = lookupObject(name);
  if (obj == NULL) {
    error(ERR_UNDECLARED_TYPE, currentToken->lineNo, currentToken->colNo);
  } else if (obj->kind != OBJ_TYPE) {
    error(ERR_UNDECLARED_TYPE, currentToken->lineNo, currentToken->colNo);
  }
  return obj;
}

Object* checkDeclaredVariable(char* name) {
  // Check if the variable is declared
  Object* obj = lookupObject(name);
  if (obj == NULL) {
    error(ERR_UNDECLARED_VARIABLE, currentToken->lineNo, currentToken->colNo);
  } else if (obj->kind != OBJ_VARIABLE) {
    error(ERR_UNDECLARED_VARIABLE, currentToken->lineNo, currentToken->colNo);
  }
  return obj;
}

Object* checkDeclaredFunction(char* name) {
  // Check if the function is declared
  Object* obj = lookupObject(name);
  if (obj == NULL) {
    error(ERR_UNDECLARED_FUNCTION, currentToken->lineNo, currentToken->colNo);
  } else if (obj->kind != OBJ_FUNCTION) {
    error(ERR_UNDECLARED_FUNCTION, currentToken->lineNo, currentToken->colNo);
  }
  return obj;
}

Object* checkDeclaredProcedure(char* name) {
  // Check if the procedure is declared
  Object* obj = lookupObject(name);
  if (obj == NULL) {
    error(ERR_UNDECLARED_PROCEDURE, currentToken->lineNo, currentToken->colNo);
  } else if (obj->kind != OBJ_PROCEDURE) {
    error(ERR_UNDECLARED_PROCEDURE, currentToken->lineNo, currentToken->colNo);
  }
  return obj;
}

Object* checkDeclaredLValueIdent(char* name) {
  // Check if the lvalue identifier is declared (can be variable, parameter, or function name)
  Object* obj = lookupObject(name);
  if (obj == NULL) {
    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
  } else {
    switch (obj->kind) {
      case OBJ_VARIABLE:
      case OBJ_PARAMETER:
        break;
      case OBJ_FUNCTION:
        // Check if it's the current function (can assign to function name for return value)
        if (obj != symtab->currentScope->owner) {
          error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
        }
        break;
      default:
        error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
    }
  }
  return obj;
}

