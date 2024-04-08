# coding=utf-8
# Copyright 2024 https://github.com/xuguodong1999
"""PEN-SIMULATOR dataset."""

import random
import string
from typing import List, Tuple

import datasets

import pen_simulator as ps

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


class PSMulDataset(datasets.GeneratorBasedBuilder):
    """PEN-SIMULATOR dataset."""

    VERSION = datasets.Version('1.0.0')
    DEFAULT_WRITER_BATCH_SIZE = 2

    COUNT_SCALE = 88/4
    TRAIN_COUNT = round(88 * COUNT_SCALE)
    TEST_COUNT = round(12 * COUNT_SCALE)
    # TRAIN_COUNT = 8
    # TEST_COUNT = 1

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
        def get_random_float() -> str:
            max_length = 6
            length = random.randint(2, max_length)
            number = ''.join(random.choice(string.digits) for _ in range(length))
            idx = random.randint(0, length)
            return f'{number[0:idx]}.{number[idx:length]}'

        return [
            datasets.SplitGenerator(
                name=datasets.Split.TRAIN,
                gen_kwargs={'split_key': 'train', 'samples': [(
                    get_random_float(),
                    get_random_float(),
                ) for _ in range(self.TRAIN_COUNT)]},
            ),
            datasets.SplitGenerator(
                name=datasets.Split.TEST,
                gen_kwargs={'split_key': 'test', 'samples': [(
                    get_random_float(),
                    get_random_float(),
                ) for _ in range(self.TEST_COUNT)]},
            ),
        ]

    def _generate_examples(self, split_key: str, samples: List[Tuple[str, str]], ):
        image_width = 512
        image_height = 512

        batched_data: List[Tuple[str, str]] = []
        prompts = [
            'how to calculate {a} times {b}, do it with {stroke_color} pen, {bg_color} background',
            'what is the result of {a} times {b}, show me on {bg_color} paper, with {stroke_color} pen',
            'solve {a} times {b}, {stroke_color} stroke, {bg_color} background',
            'math draft, {a} times {b}, {stroke_color} stroke, {bg_color} background',
            '{a} times {b}, {stroke_color} stroke, {bg_color} background',
            '{stroke_color} stroke, {bg_color} background, {a} times {b}',
        ]
        for (idx, (sample_a, sample_b)) in enumerate(samples):
            batched_data.append((sample_a, sample_b))
            if len(batched_data) < 256 and idx != len(samples) - 1:
                continue
            tex_list = [ps.generate_multiply_draft_latex(a, b) for (a, b) in batched_data]
            batched_data_copy = batched_data
            batched_data = []
            image_png_list = ps.generate_batch_image(
                texts=tex_list,
                # source_type=ps.SourceType.SVG,
                source_type=ps.SourceType.HANDWRITING,
                text_type=ps.TextType.LATEX,
                traverse_order=ps.TraverseOrder.SORT_BY_MULTIPLY_DEMONSTRATION,
                frame_width=image_width,
                frame_height=image_height,
                # parallel_num=20,
            )
            for (batch_idx, (batch_a, batch_b)) in enumerate(batched_data_copy):
                yield idx - len(batched_data_copy) + batch_idx, {
                    'instruction': random.choice(prompts).format(
                        a=batch_a,
                        b=batch_b,
                        stroke_color='black',
                        bg_color='white',
                    ),
                    'latex': tex_list[batch_idx],
                    'image': {
                        'bytes': image_png_list[batch_idx],
                    },
                    'width': image_width,
                    'height': image_height,
                }
