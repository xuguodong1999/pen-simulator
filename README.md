# pen-simulator

PEN-SIMULATOR is a handwriting / svg synthesis system built for simulation of pen-based interaction in AI research. It converts many text representations, such as latex of mathematical expression or smiles of chemical structural formula, into sketching trajectories or vectorized elements.


https://github.com/xuguodong1999/pen-simulator/assets/40680607/f4557d10-5a0f-4102-a652-4b6ae2f96842


## Feature Overview

- Adaptation of 2D typesetting library

    1. [MathJax](https://github.com/mathjax/MathJax-src)
    2. [Coordgenlibs](https://github.com/schrodinger/coordgenlibs) / [RDKit::GraphMol](https://github.com/rdkit/rdkit)

- Integration of handwriting character dataset

    1. [SCUT_IRAC Couch](http://www.hcii-lab.net/data/scutcouch)
    2. [CROHME2023](https://crohme2023.ltu-ai.dev/data-tools)

- Integration of SVG source

    1. [MathJax embedded SVG Fonts](https://github.com/mathjax/MathJax-src/tree/3.2.2/ts/output/svg/fonts/tex)
    2. [makemeahanzi Chinese Fonts](https://github.com/skishore/makemeahanzi)

## More Samples

* Mathematical expressions from im2latex.

https://github.com/xuguodong1999/pen-simulator/assets/40680607/600f2dda-f59f-455a-a984-f1d7728c92c5


* Structural formulas from drugbank.

https://github.com/xuguodong1999/pen-simulator/assets/40680607/3de121c6-1804-47d6-b1a8-563a5ad189ba


* Draft playground of decimal multiplication

https://github.com/xuguodong1999/pen-simulator/assets/40680607/498cf981-d5cc-40dc-9165-755c60dcea5a


* Draft playground of alkane isomer generation

Visualization utilities:
- Skia (C++, full support): [az-data-cli](./apps/az_data_cli/).
- matplotlib (Py, missing rotated system font support): [visualize_mpl.py](./python/pen_simulator/examples/visualize_mpl.py)
- OpenCV (Py, missing svg path and system font support): [visualize_cv2.py](./python/pen_simulator/examples/visualize_cv2.py)


## API Usage

(PEN-SIMULATOR is written in C++ and has Python bindings)

Python

```python
import pen_simulator as ps
import numpy as np

ps.global_init_couch('path/to/SCUT_IRAC/Couch', [
  'Couch_Digit_195', 'Couch_Letter_195', 'Couch_Symbol_130',
  'Couch_GB1_188', 'Couch_GB2_195',
])
ps.global_init_makemeahanzi('path/to/makemeahanzi/graphics.txt')

def example():
  def on_label(origin: str, fallback: str):
    pass  # record mathml element / atom of a molecule
  def on_hierarchy(hierarchies: list[str], label: str):
    pass  # record mathml stack / bond of a molecule
  def draw_line(x0: float, y0: float, x1: float, y1: float):
    pass  # do embedding or visualization for sketching trajectories
  def draw_path(d: str, transform: np.array):
    pass  # ... for svg path (maybe fallback)
  def draw_text(label: str, box: np.array, transform: np.array):
    pass  # ... for system font (maybe fallback)
  pen_context = ps.PenContext()
  pen_context.register_draw_line(draw_line)         # raise exception if missing
  pen_context.register_draw_path(draw_path)         # raise exception if missing
  pen_context.register_draw_text(draw_text)         # raise exception if missing
  pen_context.register_on_label(on_label)           # optional
  pen_context.register_on_hierarchy(on_hierarchy)   # optional

  pen_op = ps.generate(
    text='your-awesome-latex-code',
    source_type=ps.SourceType.HANDWRITING,
    text_type=ps.TextType.LATEX,
    traverse_order=ps.TraverseOrder.DEFAULT,
  )
  pen_op.fit_into_keep_ratio(720 * 0.8, 480 * 0.8)
  pen_op.move_center_to(np.array([720 / 2, 480 / 2]))
  pen_op.set_context(pen_context)
  pen_op.on_paint()  # will invoke all callbacks
```

---

C++ (a quick start is adding new cmake subprojects under [apps/](./apps/))

```cpp
#include "az/data/synthesis_tex_generator.h"
#include "az/data/structural_formula_item.h"

#include "az/data/couch_reader.h"
#include "az/data/makemeahanzi_reader.h"

void example() {
    using namespace az::pen;
    using namespace az::data;

    CouchDatasetReader hw_reader;
    hw_reader.sync_load_all("path/to/SCUT_IRAC/Couch", {
        "Couch_Digit_195", "Couch_Letter_195", "Couch_Symbol_130",
        "Couch_GB1_188", "Couch_GB2_195",
    });
    const auto hw_shape_provider = [&](const UCharType &label) { return hw_reader.select(label); };

    UnicodeSvgReader svg_reader;
    svg_reader.sync_load_all("path/to/makemeahanzi/graphics.txt");
    const auto svg_shape_provider = [&](const UCharType &label) { return svg_reader.select(label); };

    const auto shape_provider = hw_shape_provider; /* or svg_shape_provider */
    /* 1. generate latex */
    auto pen_tex = SynthesisTexGenerator::generate_next("your-awesome-latex-code", shape_provider);
    /* 2. generate structural formula */
    StructuralFormulaItem item;
    item.create_from_format(shape_provider, "your-awesome-smiles", "smi");
    auto pen_smi = item.data;
}
```

You can make use of derivations of PenOp like `render_with_skia` function in [apps/az_data_cli/skia_utils.cpp](./apps/az_data_cli/skia_utils.cpp):

```cpp
void example(az::pen::PenOp *data) {
    auto pen_context = std::make_shared<PenContext>();
    pen_context->register_draw_line([&](
            const ScalarType &x0, const ScalarType &y0,
            const ScalarType &x1, const ScalarType &y1
    ) { /* ... for most handwriting cases ... */ });
    pen_context->register_draw_path([&](
            const SkPath *path,
            const TransformMatrix2 &mat
    ) { /* ...for most svg cases, also work as a handwriting fallback... */ });
    pen_context->register_draw_text([&](
            const UCharType &label,
            const Box2 &box,
            const TransformMatrix2 &mat
    ) { /* ... if label is absent even in svg, use system font as a fallback */ });
    data->set_context(pen_context);
    data->on_paint();
    data->set_context(nullptr);
}
```

## Build

Recommended system configurations:

```txt
CMake 3.27.x, Ninja 1.11.x, GCC 11.x / MSVC 14.38+ / Apple Clang 15.x, Node 20.x,
Python 3.11.x (optional), Qt 6.5.3 (optional)
```

Get latest source code:

```bash
git clone git@github.com:xuguodong1999/pen-simulator.git --branch main --single-branch --recursive

# if you did not perform recursive clone on the first time, update submodules before building
git submodule init
git submodule sync
git submodule update
```

Build mathjax with [json output](./www/packages/mathjax-json) and generate js codes as C++ string buffer:

```bash
pushd www
pnpm install
pnpm --filter @xgd/mathjax-json run build
popd
```

Build root as a common CMake project:

```bash
mkdir build
cmake -G Ninja -B build -S . -DCMAKE_BUILD_TYPE=Release
# for development purpose, add -DBUILD_SHARED_LIBS=ON may be helpful
```

- (optional) If you need opencv highgui for visualization, add flags like `-DCMAKE_PREFIX_PATH=/path/to/Qt/6.5.3/gcc_64` to enable Qt integration.

Build python bindings (optional): add `-DPYTHON_EXECUTABLE=path/to/python` in cmake configurations.

We pack static version of python wheel by running `python setup.py bdist_wheel` under `python/`.

## Reference

* Jin, L. and Yan, G. and Liu, G. and Li, Y. and Ding, K. SCUT-COUCH2009: a comprehensive online unconstrained Chinese handwriting database and benchmark evaluation[J]. International Journal on Document Analysis and Recognition, 2011, 14(1): 53-64.
* MathJax team. MathJax. https://github.com/mathjax/MathJax-src. 2010.
* Schrödinger Inc. Coordgenlibs. https://github.com/schrodinger/coordgenlibs. 2017.
* Landrum, G. RDKit. https://www.rdkit.org/. 2010.
* Skishore. Makemeahanzi. https://github.com/skishore/makemeahanzi. 2016.

## License

[GPLv3 Clause](./LICENSE.md)

## Citation

```bibtex
@misc{PENSIM2024,
  title = {pen simulator: data synthesis for simulation of pen-based interaction},
  author = {xuguodong1999},
  year = {2024},
  publisher = {GitHub},
  journal = {GitHub repository},
  howpublished = {\url{https://github.com/xuguodong1999/pen-simulator}}
}
```

## Have fun training your LLM to use a pen !
