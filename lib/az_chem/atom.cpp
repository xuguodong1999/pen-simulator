#include "az/chem/atom.h"
#include <string>

using namespace az;
using namespace az::chem;

std::size_t AtomPtrHasher::operator()(const Atom *k) const {
    return std::hash<uint64_t>()(k->id);
}