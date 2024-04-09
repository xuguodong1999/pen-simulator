import random
import string
from typing import List, Tuple

import datasets

import pen_simulator as ps
from instruct_pen.ps_mul_dataset.prompts import NO_COLOR_PROMPTS


def get_random_float() -> str:
    max_length = 6
    length = random.randint(1, max_length)
    number = ''.join(random.choice(string.digits) for _ in range(length))
    while number.startswith('0'):
        number = f'{random.choice(string.digits)}{number[1:]}'
    idx = random.randint(0, length)
    float_str = f'{number[0:idx]}.{number[idx:length]}'
    if float_str.startswith('.'):
        float_str = f'0{float_str}'
    if float_str.endswith('.'):
        float_str = float_str[0:-1]
    return float_str


def get_ps_inputs(count: int):
    return [(
        get_random_float(),
        get_random_float(),
    ) for _ in range(count)]


def get_ps_features():
    return datasets.Features({
        'instruction': datasets.Value('string'),
        'latex': datasets.Value('string'),
        'image': datasets.Image(),
        'width': datasets.Value('int32'),
        'height': datasets.Value('int32'),
    })


def ps_generator_mt(samples: List[Tuple[str, str]], ):
    image_width = 512
    image_height = 512
    batched_data: List[Tuple[str, str]] = []
    prompts = NO_COLOR_PROMPTS
    for (idx, (sample_a, sample_b)) in enumerate(samples):
        batched_data.append((sample_a, sample_b))
        if len(batched_data) < 256 and idx != len(samples) - 1:
            continue
        tex_list = [ps.generate_multiply_draft_latex(a, b) for (a, b) in batched_data]
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
        # print(f'len(batched_data)={len(batched_data)}')
        # image_png_list = [b'' for _ in range(len(batched_data))]
        for (batch_idx, (batch_a, batch_b)) in enumerate(batched_data):
            yield idx - len(batched_data) + batch_idx, {
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
        batched_data = []
