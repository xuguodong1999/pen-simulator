import argparse

import numpy as np

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

    def display_frame(msg=''):
        print(msg)

    def draw_line(x0: float, y0: float, x1: float, y1: float):
        display_frame(f'* draw line from ({x0:.2f}, {y0:.2f}) to ({x1:.2f}, {y1:.2f})')

    def draw_path(d: str, transform: np.array):
        display_frame(f'* draw_path: "{d}"\n\twith 3x3 transform:\n{transform}')

    def draw_text(label: str, box: np.array, transform: np.array):
        display_frame(f'* draw_text: {label} {box}\n\twith 3x3 transform:\n{transform}')

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

# Linux: --couch /mnt/data/datasets/SCUT_IRAC/Couch --makemeahanzi /home/xgd/git/_/web/makemeahanzi/graphics.txt
# WSL2: --couch /home/xgd/datasets/SCUT_IRAC/Couch --makemeahanzi /home/xgd/git/_/web/makemeahanzi/graphics.txt
# Windows: --couch D:/datasets/ntfs/SCUT_IRAC/Couch --makemeahanzi D:/git/_/web/makemeahanzi/graphics.txt
# Mac: --couch /Users/xgd/datasets/SCUT_IRAC/Couch --makemeahanzi /Users/xgd/git/_/web/makemeahanzi/graphics.txt
if __name__ == '__main__':
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
    for source_type_ in [ps.SourceType.HANDWRITING, ps.SourceType.SVG]:
        for (sample, text_type_, maybe_order) in SAMPLES:
            ps_generate_example(
                text=sample,
                source_type=source_type_,
                text_type=text_type_,
                traverse_order=ps.TraverseOrder.DEFAULT if maybe_order is None
                else ps.TraverseOrder.SORT_BY_MULTIPLY_DEMONSTRATION
            )
