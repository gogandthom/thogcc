#pragma once

#include "ast/Node.h"
#include "types/SymbolTable.h"
#include "visitors/DefaultVisitor.h"

namespace thogcc::visitors {

class TypeCheckVisitor : public DefaultVisitor {
   public:
    TypeCheckVisitor();

    void visit(ast::Node& node) override;

   private:
    SymbolTable table;
};

};  // namespace thogcc::visitors
