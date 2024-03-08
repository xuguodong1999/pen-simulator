#include "az/core/config.h"

#include <unordered_map>
#include <string>

namespace az::data::impl::crohme2023 {
    std::unordered_map<std::string, LabelType> CHAR_MAP = {
            {"\\Delta",      "Δ"},
            {"\\alpha",      "α"},
            {"\\beta",       "β"},
            {"\\div",        "÷"},
            {"\\forall",     "∀"},
            {"\\gamma",      "γ"},
            {"\\geq",        "≥"},
            {"\\gt",         ">"},
            {"\\in",         "∈"},
            {"\\infty",      "∞"},
            {"\\int",        "∫"},
            {"\\lambda",     "λ"},
            {"\\ldots",      "…"},
            {"\\leq",        "≤"},
            {"\\lt",         "<"},
            {"\\neq",        "≠"},
            {"\\phi",        "φ"},
            {"\\pi",         "π"},
            {"\\pm",         "±"},
            {"\\prime",      "′"},
            {"\\rightarrow", "⟶"},
            {"\\sigma",      "σ"},
            {"\\sqrt",       "√"},
            {"\\sum",        "∑"},
            {"\\theta",      "Θ"},
            {"\\times",      "×"},
            {"\\{",          "{"},
            {"\\}",          "}"},
            // FIXME: handle these string level annotation
            {"\\sin",        " "},
            {"\\cos",        " "},
            {"\\tan",        " "},
            {"\\lim",        " "},
            {"\\log",        " "},
    };
}