# coding=utf-8
# Copyright 2024 https://github.com/xuguodong1999
"""PEN-SIMULATOR dataset."""

import random
from io import BytesIO
from typing import List, Tuple

import colour
import datasets
import matplotlib.colors as mcolors
import matplotlib.pyplot as plt
import numpy as np
from matplotlib import patches
from matplotlib.path import Path
from matplotlib.transforms import Affine2D
from svgpath2mpl import parse_path

import pen_simulator as ps
from instruct_pen.ps_mul_dataset.prompts import COLORFUL_PROMPTS
from instruct_pen.ps_mul_dataset.utils import get_ps_inputs

_CITATION = r"""\
@misc{PENSIM2024,
  title = {pen simulator: data synthesis for simulation of pen-based interaction},
  author = {xuguodong1999},
  year = {2024},
  publisher = {GitHub},
  journal = {GitHub repository},
  howpublished = {\url{https://github.com/xuguodong1999/pen-simulator}}
}
"""

_DESCRIPTION = """\
PEN-SIMULATOR is a handwriting / svg synthesis system built for simulation of pen-based interaction in AI research. \
It converts many text representations, such as latex of mathematical expression or \
smiles of chemical structural formula, into sketching trajectories or vectorized elements.
"""

_HOMEPAGE = 'https://github.com/xuguodong1999/pen-simulator'

_LICENSE = 'GPL-3.0 license'


def hex_to_lab(color: str):
    return colour.XYZ_to_Lab(colour.RGB_to_XYZ(colour.notation.HEX_to_RGB(color), colourspace='sRGB'))


TRAIN_COUNT = 8888
TEST_COUNT = 1
DELTA_E_THRESHOLD = 20


class PSMulDataset(datasets.GeneratorBasedBuilder):
    """PEN-SIMULATOR dataset."""

    VERSION = datasets.Version('1.0.0')

    def _info(self):
        features = datasets.Features({
            'instruction': datasets.Value('string'),
            'latex': datasets.Value('string'),
            'image': datasets.Image(),
            'width': datasets.Value('int32'),
            'height': datasets.Value('int32'),
        })
        return datasets.DatasetInfo(
            description=_DESCRIPTION,
            features=features,
            homepage=_HOMEPAGE,
            license=_LICENSE,
            citation=_CITATION,
        )

    def _split_generators(self, dl_manager):
        return [
            datasets.SplitGenerator(
                name=datasets.Split.TRAIN,
                gen_kwargs={
                    'split_key': 'train',
                    'samples': get_ps_inputs(TRAIN_COUNT),
                },
            ),
            datasets.SplitGenerator(
                name=datasets.Split.TEST,
                gen_kwargs={
                    'split_key': 'test',
                    'samples': get_ps_inputs(TEST_COUNT),
                },
            ),
        ]

    def _generate_examples(self, split_key: str, samples: List[Tuple[str, str]], ):
        image_width = 1024
        image_height = 1024
        scale = 1 / 1.2
        frame = np.full((image_width, image_height, 4), 0, np.uint8)

        def create_sample(pen_op: ps.PenOp, stroke_color: str, bg_color: str, ):
            fig, ax = plt.subplots()
            ax.axis('off')
            fig.set_size_inches(w=image_width, h=image_height)
            img = ax.imshow(frame)
            img.set_cmap('hot')

            def draw_line(x0: float, y0: float, x1: float, y1: float):
                ax.plot([x0, x1], [y0, y1], color=stroke_color, )

            def draw_path(d: str, transform: np.array):
                path: Path = parse_path(d).transformed(Affine2D(transform))
                patch = patches.PathPatch(path, color=stroke_color, zorder=1, )
                ax.add_patch(patch)

            def draw_text(label: str, box: np.array, transform: np.array):
                raise NotImplemented(f'draw_text NOT implemented in matplotlib, try skia instead.')

            pen_context = ps.PenContext()
            pen_context.register_draw_line(draw_line)
            pen_context.register_draw_path(draw_path)
            pen_context.register_draw_text(draw_text)

            pen_op.fit_into_keep_ratio(image_width * scale, image_height * scale)
            pen_op.move_center_to(np.array([image_width / 2, image_height / 2]))
            pen_op.set_context(pen_context)
            pen_op.on_paint()

            bio = BytesIO()
            plt.savefig(
                bio,
                format='png',
                dpi=1,
                transparent=False,
                pad_inches=0,
                facecolor=bg_color,
            )
            plt.close()
            bio.seek(0)
            image_png_ = bio.getvalue()
            bio.close()
            return image_png_

        prompts = COLORFUL_PROMPTS
        css_colors = list(mcolors.CSS4_COLORS.items())
        css_colors = random.sample(css_colors, 64)
        color_pairs = []
        for i in range(0, len(css_colors)):
            name1, value1 = css_colors[i]
            lab1 = hex_to_lab(value1)
            for j in range(i + 1, len(css_colors)):
                name2, value2 = css_colors[j]
                lab2 = hex_to_lab(value2)
                delta_e = colour.delta_E(lab1, lab2, )
                if delta_e > DELTA_E_THRESHOLD:
                    color_pairs.append((name1, name2))

        for (idx, (a, b)) in enumerate(samples):
            tex = ps.generate_multiply_draft_latex(a, b)

            # @profile
            def ps_generate():
                return ps.generate(
                    text=tex,
                    source_type=ps.SourceType.SVG,
                    text_type=ps.TextType.LATEX,
                    traverse_order=ps.TraverseOrder.SORT_BY_MULTIPLY_DEMONSTRATION,
                )

            pen_op = ps_generate()
            (stroke_color_, bg_color_) = random.choice(color_pairs)
            image_png = create_sample(pen_op, stroke_color_, bg_color_)
            yield idx, {
                'instruction': random.choice(prompts).format(
                    a=a,
                    b=b,
                    stroke_color=stroke_color_,
                    bg_color=bg_color_,
                ),
                'latex': tex,
                'image': {
                    'bytes': image_png,
                },
                'width': image_width,
                'height': image_height,
            }
