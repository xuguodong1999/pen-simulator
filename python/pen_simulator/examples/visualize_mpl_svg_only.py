import argparse
import random

import matplotlib.pyplot as plt
import numpy as np
from matplotlib import patches
from matplotlib.path import Path
from matplotlib.transforms import Affine2D
from svgpath2mpl import parse_path

import az_init  # noqa # pylint: disable=unused-import
import pen_simulator as ps

WINDOW_WIDTH = 720
WINDOW_HEIGHT = 480
SCALE = 0.88


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
        raise NotImplemented(f'draw_text NOT implemented in matplotlib, try skia instead.')

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
]

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='visualization example of pen_simulator')
    args = parser.parse_args()
    for (sample, text_type_, maybe_order) in SAMPLES:
        ps_generate_example(
            text=sample,
            source_type=ps.SourceType.SVG,
            text_type=text_type_,
            traverse_order=ps.TraverseOrder.DEFAULT if maybe_order is None
            else ps.TraverseOrder.SORT_BY_MULTIPLY_DEMONSTRATION
        )
