import argparse
import random
import sys
from typing import Optional, Literal

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
from matplotlib import patches
from matplotlib.path import Path
from matplotlib.text import Annotation
from matplotlib.transforms import Affine2D
from matplotlib.transforms import Transform, Bbox
from svgpath2mpl import parse_path

import pen_simulator as ps

WINDOW_WIDTH = 720
WINDOW_HEIGHT = 480
SCALE = 0.88


# https://codereview.stackexchange.com/questions/275079/auto-fitting-text-into-boxes-in-matplotlib
def text_with_autofit(
        ax: plt.Axes,
        txt: str,
        xy: tuple[float, float],
        width: float, height: float,
        *,
        transform: Optional[Transform] = None,
        ha: Literal['left', 'center', 'right'] = 'left',
        va: Literal['bottom', 'center', 'top'] = 'top',
        **kwargs,
):
    if transform is None:
        transform = ax.transData

    x, y = xy
    xa0, xa1 = {
        'center': (x - width / 2, x + width / 2),
        'left': (x, x + width),
        'right': (x - width, x),
    }[ha]
    ya0, ya1 = {
        'center': (y - height / 2, y + height / 2),
        'bottom': (y, y + height),
        'top': (y - height, y),
    }[va]
    a0 = xa0, ya0
    a1 = xa1, ya1

    x0, y0 = transform.transform(a0)
    x1, y1 = transform.transform(a1)
    rect_width = abs(x1 - x0)
    rect_height = abs(y1 - y0)

    fig: plt.Figure = ax.get_figure()
    dpi = fig.dpi
    rect_height_inch = rect_height / dpi
    fontsize = rect_height_inch * 72

    text: Annotation = ax.annotate(txt, xy, ha=ha, va=va, xycoords=transform, **kwargs)

    text.set_fontsize(fontsize)
    bbox: Bbox = text.get_window_extent(fig.canvas.get_renderer())
    adjusted_size = fontsize * rect_width / bbox.width
    text.set_fontsize(adjusted_size)

    return text


def ps_generate_example(
        *,
        text: str,
        source_type: ps.SourceType,
        text_type: ps.TextType,
        traverse_order: ps.TraverseOrder,
):
    print(f'***\n{text}\n***\n{source_type} - {text_type} - {traverse_order}')
    fig, ax = plt.subplots()
    plt.xlim((0, WINDOW_WIDTH))
    plt.ylim((WINDOW_HEIGHT, 0))

    ax.set_xticks(np.arange(0, WINDOW_WIDTH, 50))
    ax.set_xticks(np.arange(0, WINDOW_WIDTH, 10), minor=True)
    ax.set_yticks(np.arange(0, WINDOW_HEIGHT, 50))
    ax.set_yticks(np.arange(0, WINDOW_HEIGHT, 10), minor=True)
    ax.grid(True)

    def display_frame(msg=''):
        plt.title(msg, loc='left')
        if ps.SourceType.SVG == source_type or random.uniform(0, 1) < 0.1:
            plt.pause(0.001)  # TODO: figure out why mpl seems much slower than cv2
        plt.draw()

    def draw_line(x0: float, y0: float, x1: float, y1: float):
        ax.plot([x0, x1], [y0, y1])
        display_frame(f'* draw line from ({x0:.2f}, {y0:.2f}) to ({x1:.2f}, {y1:.2f})')

    def draw_path(d: str, transform: np.array):
        path: Path = parse_path(d).transformed(Affine2D(transform))
        patch = patches.PathPatch(path)
        ax.add_patch(patch)
        display_frame(f'* draw_path "{d}"')

    def draw_text(label: str, box: np.array, transform: np.array):
        size = box[:, 1] - box[:, 0]
        t_box = transform @ np.array([
            [0, size[0]],
            [-size[1], 0],
            [1, 1]]
        )
        tl = t_box[:2, 0]
        br = t_box[:2, 1]
        t_size = br - tl
        text_with_autofit(ax, label, (tl[0], tl[1]), t_size[0], t_size[1])
        display_frame(f'* draw_text "{label}"')

    def on_label(origin: str, fallback: str):
        print(f'! on_label {origin} {fallback}')

    def on_hierarchy(hierarchies: list[str], label: str):
        print(f'! on_hierarchy {hierarchies} {label}')

    pen_context = ps.PenContext()
    pen_context.register_draw_line(draw_line)
    pen_context.register_draw_path(draw_path)
    pen_context.register_draw_text(draw_text)
    pen_context.register_on_label(on_label)
    pen_context.register_on_hierarchy(on_hierarchy)

    pen_op = ps.generate(
        text=text,
        source_type=source_type,
        text_type=text_type,
        traverse_order=traverse_order,
    )
    pen_op.fit_into_keep_ratio(WINDOW_WIDTH * SCALE, WINDOW_HEIGHT * SCALE)
    pen_op.move_center_to(np.array([WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2]))
    pen_op.set_context(pen_context)
    pen_op.on_paint()

    display_frame('Press [x] in window to continue...')
    plt.show(block=True)


SAMPLES = [
    (r'''
\begin{align}
1.2\phantom{.}34&\\
\times\phantom{0}\phantom{0}\phantom{0}\phantom{0}\phantom{.}\phantom{.}5.67&\\
\hline
8\phantom{.}6\phantom{.}38&\\
74\phantom{.}0\phantom{.}4\phantom{0}&\\
6\phantom{.}17\phantom{.}0\phantom{.}\phantom{0}\phantom{0}&\\
\hline
6.99\phantom{.}6\phantom{.}78
\end{align}
''', ps.TextType.LATEX, ps.TraverseOrder.SORT_BY_MULTIPLY_DEMONSTRATION),
    (r'N[C@H](C(=O)O)C', ps.TextType.SMILES, None),
    (r'N[C@@H](C(=O)O)C', ps.TextType.SMILES, None),
    (r'CC/C=C/CC', ps.TextType.SMILES, None),
    (r'CC/C=C\CC', ps.TextType.SMILES, None),
    (r'c1ccccc1', ps.TextType.SMILES, None),
    (r'\sin(\alpha)\cos(\beta)=\frac{\sin(\alpha+\beta)+\sin(\alpha-\beta)}{2}', ps.TextType.LATEX, None),
    (r'\frac{测}{试用例😅}', ps.TextType.LATEX, None),
]

if __name__ == '__main__':
    if sys.platform.find('win32') != -1:
        mpl.rc('font', family=['Microsoft YaHei', 'Segoe UI Emoji'])
    parser = argparse.ArgumentParser(description='visualization example of pen_simulator')
    parser.add_argument(
        '--couch', type=str, help='path to SCUT_IRAC/Couch', required=True,
        metavar='path/to/SCUT_IRAC/Couch',
    )
    parser.add_argument(
        '--makemeahanzi', type=str, help='path to makemeahanzi/graphics.txt', required=True,
        metavar='path/to/makemeahanzi/graphics.txt',
    )
    args = parser.parse_args()
    ps.global_init_couch(args.couch, [
        'Couch_Digit_195', 'Couch_Letter_195', 'Couch_Symbol_130',
        # 'Couch_GB1_188', 'Couch_GB2_195',
    ])
    ps.global_init_makemeahanzi(args.makemeahanzi)
    for (sample, text_type_, maybe_order) in SAMPLES:
        for source_type_ in [
            ps.SourceType.HANDWRITING,
            ps.SourceType.SVG,
        ]:
            ps_generate_example(
                text=sample,
                source_type=source_type_,
                text_type=text_type_,
                traverse_order=ps.TraverseOrder.DEFAULT if maybe_order is None
                else ps.TraverseOrder.SORT_BY_MULTIPLY_DEMONSTRATION
            )
