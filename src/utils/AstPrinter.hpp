#include <format>
#include <string>

#include "ast/Expr.hpp"
#include "ast/Stmt.hpp"

class AstPrinter
{
  public:
    // Statements print
    std::string print(const Stmt* stmt)
    {
        if (auto expressionStmt = dynamic_cast<const ExpressionStmt*>(stmt))
        {
            return visit(expressionStmt);
        }

        if (auto var = dynamic_cast<const VariableDeclarationStmt*>(stmt))
        {
            return std::format("(var {} -> {} {})", var->name.lexeme, var->declared_type.lexeme,
                               print(var->expression.get()));
        }

        if (auto printStmt = dynamic_cast<const PrintStmt*>(stmt))
        {
            return visit(printStmt);
        }

        return "Unknown Statement";
    }

    // Expressions print
    std::string print(const Expr* expr)
    {
        if (auto literal = dynamic_cast<const LiteralExpr*>(expr))
        {
            return visit(literal);
        }

        if (auto binary = dynamic_cast<const BinaryExpr*>(expr))
        {
            return visit(binary);
        }

        if (auto unary = dynamic_cast<const UnaryExpr*>(expr))
        {
            return visit(unary);
        }

        if (auto grouping = dynamic_cast<const GroupingExpr*>(expr))
        {
            return visit(grouping);
        }

        if (auto variable = dynamic_cast<const VariableExpr*>(expr))
        {
            return visit(variable);
        }

        if (auto assignment = dynamic_cast<const AssignmentExpr*>(expr))
        {
            return visit(assignment);
        }

        return "Unknown";
    }

  private:
    std::string visit(const ExpressionStmt* stmt)
    {
        return print(stmt->expression.get());
    }

    std::string visit(const PrintStmt* stmt)
    {
        return std::format("(print {})", print(stmt->expression.get()));
    }

    std::string visit(const LiteralExpr* expr)
    {
        return expr->value.lexeme;
    }

    std::string visit(const BinaryExpr* expr)
    {
        return std::format("({} {} {})", expr->op.lexeme, print(expr->left.get()), print(expr->right.get()));
    }

    std::string visit(const UnaryExpr* expr)
    {
        return std::format("({} {})", expr->op.lexeme, print(expr->expr.get()));
    }

    std::string visit(const GroupingExpr* expr)
    {
        return std::format("(group {})", print(expr->expr.get()));
    }

    std::string visit(const VariableExpr* expr)
    {
        return expr->name.lexeme;
    }

    std::string visit(const AssignmentExpr* expr)
    {
        return std::format("(= {} {})", expr->name.lexeme, print(expr->value.get()));
    }
};
