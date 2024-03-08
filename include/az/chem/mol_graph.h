#pragma once

#include "az/core/geometry_def.h"
#include "az/chem/config.h"
#include "az/chem/atom.h"
#include "az/chem/bond.h"
#include <memory>

namespace az::chem {
    namespace impl {
        struct GraphImpl;
    }
    struct AZCHEM_EXPORT BackendTag {
    };
    struct AZCHEM_EXPORT CoordgenlibsBackend : BackendTag {
    };
    struct AZCHEM_EXPORT RDKitBackend : BackendTag {
    };
    struct AZCHEM_EXPORT OpenbabelBackend : BackendTag {
    };

    class AZCHEM_EXPORT MolGraph {
    public:
        explicit MolGraph();

        MolGraph(MolGraph &&g) noexcept;

        ~MolGraph();

        impl::GraphImpl *get_pimpl();

        void traverse(
                const std::function<void(const Atom &)> &on_discover_node,
                const std::function<void(const Bond &)> &on_edge);

        size_t bond_count() const;

        size_t atom_count() const;

        template<typename Impl>
        std::string to_format(std::string_view format) const {
            if (std::is_same_v<Impl, RDKitBackend>) {
                return to_format_rdkit_impl(format);
            } else if (std::is_same_v<Impl, OpenbabelBackend>) {
                return to_format_openbabel_impl(format);
            } else {
                throw std::runtime_error("unknown writer backend");
            }
        }

        template<typename Impl>
        void from_format(std::string_view input, std::string_view format) {
            if (std::is_same_v<Impl, RDKitBackend>) {
                from_format_rdkit_impl(input, format);
            } else if (std::is_same_v<Impl, OpenbabelBackend>) {
                from_format_openbabel_impl(input, format);
            } else {
                throw std::runtime_error("unknown parser backend");
            }
        }

        template<typename Impl>
        void generate_2d() {
            if (std::is_same_v<Impl, RDKitBackend>) {
                generate_2d_rdkit_impl();
            } else if (std::is_same_v<Impl, OpenbabelBackend>) {
                generate_2d_openbabel_impl();
            } else if (std::is_same_v<Impl, CoordgenlibsBackend>) {
                generate_2d_coordgenlibs_impl();
            } else {
                throw std::runtime_error("unknown 2d generation backend");
            }
        }

        template<typename Impl>
        void generate_3d() {
            if (std::is_same_v<Impl, RDKitBackend>) {
                generate_3d_rdkit_impl();
            } else if (std::is_same_v<Impl, OpenbabelBackend>) {
                generate_3d_openbabel_impl();
            } else {
                throw std::runtime_error("unknown 3d generation backend");
            }
        }

        std::string to_format(std::string_view format) const;

        void from_format(std::string_view input, std::string_view format);

        void generate_2d();

        void generate_3d();

    private:
        std::unique_ptr<impl::GraphImpl> p;

        std::string to_format_openbabel_impl(std::string_view format) const;

        std::string to_format_rdkit_impl(std::string_view format) const;

        void from_format_openbabel_impl(std::string_view input, std::string_view format);

        void from_format_rdkit_impl(std::string_view input, std::string_view format);

        void generate_2d_openbabel_impl();

        void generate_2d_rdkit_impl();

        void generate_2d_coordgenlibs_impl();

        void generate_3d_openbabel_impl();

        void generate_3d_rdkit_impl();
    };
}