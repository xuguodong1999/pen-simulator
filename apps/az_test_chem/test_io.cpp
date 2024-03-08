#include "az/chem/mol_graph.h"

#include <gtest/gtest.h>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <string>

using namespace az;
using namespace az::chem;

TEST(test_chem, smi_in_inchi_out) {
    std::vector<std::string> test_cases = {
            "CC/C=C/CC",
            // known issue: openbabel adapter failed to parse or save Z/E
//            R"(CC/C=C\CC)",
            "N[C@H](C(=O)O)C",
            "N[C@@H](C(=O)O)C",
            "c1ccccc1",
            "C1CCCCC1",
            "CCCCC(CC)COC(=O)C1=CC=C(NC2=NC(NC3=CC=C(C=C3)C(=O)OCC(CC)CCCC)=NC(NC3=CC=C(C=C3)C(=O)NC(C)(C)C)=N2)C=C1",
            "OC[C@H]1O[C@H]([C@H](O)[C@@H](O)[C@@H]1O)C1C2=CC=CC(O)=C2C(=O)C2=C1C=C(CO)C=C2O",
            "O[Zr]Cl",
            "CC(=O)OC1=CC(O)=CC=C1",
            "CC1COC2(N1CC(O)=NC1=C2C=C(Cl)C=C1)C1=CC=CC=C1Cl",
            "CC1CN2CC(=O)NC3=C(C=C(Cl)C=C3)C2(O1)C1=CC=CC=C1",
            "CCN1CCC[C@H]1CNC(=O)C1=C(O)C(CC)=CC(Cl)=C1OC",
            "OC1=C(CC2=C(NC(=S)NC2=O)C2=CC=CC=C2)C2=CC=CC=C2C=C1",
    };
    std::vector<std::string> to_validate_formats = {
            "inchi",
            "inchikey",
    };
    for (auto &input: test_cases) {
        MolGraph mol;
        mol.from_format(input, "smi");
        for (auto &format: to_validate_formats) {
            EXPECT_EQ(mol.to_format<RDKitBackend>(format), mol.to_format<OpenbabelBackend>(format));
        }
    }
}

// known issue: openbabel adapter failed to parse or save R/S
TEST(test_chem, smi_in_inchi_out_print) {
    using Backend = RDKitBackend;
//    using Backend = OpenbabelBackend;
    {
        MolGraph mol;
        mol.from_format<Backend>("CC/C=C/CC", "smi");
        SPDLOG_INFO(mol.to_format<Backend>("inchi"));
        SPDLOG_INFO(mol.to_format<Backend>("can"));
    }
    {
        MolGraph mol;
        mol.from_format<Backend>(R"(CC/C=C\CC)", "smi");
        SPDLOG_INFO(mol.to_format<Backend>("inchi"));
        SPDLOG_INFO(mol.to_format<Backend>("can"));
    }
    {
        MolGraph mol;
        mol.from_format<Backend>("N[C@H](C(=O)O)C", "smi");
        SPDLOG_INFO(mol.to_format<Backend>("inchi"));
        SPDLOG_INFO(mol.to_format<Backend>("can"));
    }
    {
        MolGraph mol;
        mol.from_format<Backend>("N[C@@H](C(=O)O)C", "smi");
        SPDLOG_INFO(mol.to_format<Backend>("inchi"));
        SPDLOG_INFO(mol.to_format<Backend>("can"));
    }
}

TEST(test_chem, smi_io) {
//    using Backend = RDKitBackend;
    using Backend = OpenbabelBackend;
    // Notice: Openbabel and RDKit's canonical SMILES is different
    std::vector<std::pair<std::string, std::string>> test_cases = {
            {"c1ccccc1",                ""},
            {"[C]1=[C][C]=[C][C]=[C]1", "c1ccccc1"},
            {"[C][C][C]",               "CCC"},
            {"N[C@H](C(=O)O)C",         "C[C@@H](C(=O)O)N",},
            {"N[C@@H](C(=O)O)C",        "C[C@H](C(=O)O)N",},
    };
    for (auto &[input, ground_truth]: test_cases) {
        MolGraph mol;
        mol.from_format<RDKitBackend>(input, "smi");
        if (ground_truth.empty()) {
            EXPECT_EQ(input, mol.to_format<Backend>("can"));
        } else {
            EXPECT_EQ(ground_truth, mol.to_format<Backend>("can"));
        }
    }
}
