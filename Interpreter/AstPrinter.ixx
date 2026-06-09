module;
#include <string>
#include <format>

export module AstPrinter;

import Expr;
import Stmt;

export class AstPrinter {
public:
    std::string print(const Stmt* stmt)
    {
        if (auto expressionStmt = dynamic_cast<const ExpressionStmt*>(stmt)) {
            return print(expressionStmt->expression.get());
        }

        if (auto printStmt = dynamic_cast<const PrintStmt*>(stmt)) {
            return print(printStmt->expression.get());
        }

        return "Unknown Statement";
    }

    std::string print(const Expr* expr)
    {
        if (auto literal =
            dynamic_cast<const LiteralExpr*>(expr))
        {
            return literal->value.lexeme;
        }

        if (auto binary =
            dynamic_cast<const BinaryExpr*>(expr))
        {
            return std::format(
                "({} {} {})",
                binary->op.lexeme,
                print(binary->left.get()),
                print(binary->right.get())
            );
        }

        if (auto unary =
            dynamic_cast<const UnaryExpr*>(expr))
        {
            return std::format(
                "({} {})",
                unary->op.lexeme,
                print(unary->expr.get())
            );
        }

        if (auto grouping =
            dynamic_cast<const GroupingExpr*>(expr))
        {
            return std::format(
                "(group {})",
                print(grouping->expr.get())
            );
        }

        if (auto variable =
            dynamic_cast<const VariableExpr*>(expr))
        {
            return variable->name.lexeme;
        }

        if (auto assignment =
            dynamic_cast<const AssignmentExpr*>(expr))
        {
            return std::format(
                "(= {} {})",
                assignment->name.lexeme,
                print(assignment->value.get())
            );
        }

        return "Unknown";
    }

private:
    std::string visit(const LiteralExpr* expr)
    {
        return expr->value.lexeme;
    }

    std::string visit(const BinaryExpr* expr)
    {
		return std::format(
			"({} {} {})",
			expr->op.lexeme,
			print(expr->left.get()),
			print(expr->right.get())
		);
    }

    std::string visit(const UnaryExpr* expr)
    {
		return std::format(
			"({} {})",
			expr->op.lexeme,
			print(expr->expr.get())
		);
    }

    std::string visit(const GroupingExpr* expr)
    {
        return std::format(
            "(group {})",
            print(expr->expr.get())
        );
    }
};
