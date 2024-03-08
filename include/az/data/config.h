#pragma once

#include "AzData_export.h"
#include "az/core/config.h"
#include <functional>

namespace az::pen {
    class PenOp;
}
namespace az::data {
    using ShapeProvider = std::function<az::pen::PenOp *(const UCharType &)>;
}