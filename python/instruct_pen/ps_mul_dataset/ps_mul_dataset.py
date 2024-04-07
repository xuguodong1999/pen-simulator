# coding=utf-8
# Copyright 2024 https://github.com/xuguodong1999
"""PEN-SIMULATOR dataset."""

import random
import string
from io import BytesIO
from typing import List, Tuple

import datasets
import matplotlib.pyplot as plt
import numpy as np
from matplotlib import patches
from matplotlib.path import Path
from matplotlib.transforms import Affine2D
from svgpath2mpl import parse_path

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

    COUNT_SCALE = 22
    # TRAIN_COUNT = round(88 * COUNT_SCALE)
    # TEST_COUNT = round(12 * COUNT_SCALE)
    TRAIN_COUNT = 8
    TEST_COUNT = 1

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
        image_width = 1024
        image_height = 1024
        scale = 0.99
        frame = np.full((image_width, image_height, 3), 255, np.uint8)

        def create_sample(pen_op: ps.PenOp):
            fig, ax = plt.subplots()
            ax.axis('off')
            fig.set_size_inches(w=image_width, h=image_height)
            img = ax.imshow(frame)
            img.set_cmap('hot')

            def draw_line(x0: float, y0: float, x1: float, y1: float):
                ax.plot([x0, x1], [y0, y1], color='black', )

            def draw_path(d: str, transform: np.array):
                path: Path = parse_path(d).transformed(Affine2D(transform))
                patch = patches.PathPatch(path, color='black', )
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
            plt.savefig(bio, format='png', dpi=1, transparent=False, )
            plt.close()
            bio.seek(0)
            image_png_ = bio.getvalue()
            bio.close()
            return image_png_

        batched_data: List[Tuple[str, str]] = []
        for (idx, (sample_a, sample_b)) in enumerate(samples):
            batched_data.append((sample_a, sample_b))
            if len(batched_data) < 256 and idx != len(samples) - 1:
                continue
            tex_list = [ps.generate_multiply_draft_latex(a, b) for (a, b) in batched_data]
            pen_op_list = ps.generate_batch(
                texts=tex_list,
                source_type=ps.SourceType.SVG,
                text_type=ps.TextType.LATEX,
                traverse_order=ps.TraverseOrder.SORT_BY_MULTIPLY_DEMONSTRATION,
            )
            batched_data_copy = batched_data
            batched_data = []
            for (batch_idx, (batch_a, batch_b)) in enumerate(batched_data_copy):
                image_png = create_sample(pen_op_list[batch_idx])
                yield idx - len(batched_data_copy) + batch_idx, {
                    'instruction': f'show me how to calculate {batch_a} times {batch_b}',
                    'latex': tex_list[batch_idx],
                    'image': {
                        'bytes': image_png,
                    },
                    'width': image_width,
                    'height': image_height,
                }


# FIXME: crash log
'''
Generating train split: 0 examples [00:00, ? examples/s]Process 171431 stopped
* thread #72, name = 'python', stop reason = signal SIGSEGV: invalid address (fault address: 0x1)
    frame #0: 0x00000000004ebd6f python`unicode_decode_utf8 at obmalloc.c:1979:13
(lldb) bt
* thread #72, name = 'python', stop reason = signal SIGSEGV: invalid address (fault address: 0x1)
  * frame #0: 0x00000000004ebd6f python`unicode_decode_utf8 at obmalloc.c:1979:13
    frame #1: 0x00007fff9bff81fd pen_simulator.cpython-311-x86_64-linux-gnu.so`pybind11_init_pen_simulator(pybind11::module_&)::'lambda2'(pybind11::args const&, pybind11::kwargs const&, std::basic_string_view<char, std::char_traits<char>>)::operator()(pybind11::args const&, pybind11::kwargs const&, std::basic_string_view<char, std::char_traits<char>>) const (.constprop.0) at cast.h:455:86
    frame #2: 0x00007fff9bff913e pen_simulator.cpython-311-x86_64-linux-gnu.so`operator() [inlined] operator(i=7, __closure=0x000000000396c638) at main.cpp:170:41
    frame #3: 0x00007fff9bff911d pen_simulator.cpython-311-x86_64-linux-gnu.so`operator() at for_each.hpp:136:16
    frame #4: 0x00007fff9bff90ec pen_simulator.cpython-311-x86_64-linux-gnu.so`operator() at partitioner.hpp:138:15
    frame #5: 0x00007fff9bff9000 pen_simulator.cpython-311-x86_64-linux-gnu.so`operator(__closure=0x00007ffee4010d40) at for_each.hpp:132:18
    frame #6: 0x00007fff9c02480e pen_simulator.cpython-311-x86_64-linux-gnu.so`tf::Executor::_invoke(tf::Worker&, tf::Node*) [inlined] std::function<void ()>::operator(this=0x00007ffee4001200)() const at std_function.h:590:9
    frame #7: 0x00007fff9c0247ef pen_simulator.cpython-311-x86_64-linux-gnu.so`tf::Executor::_invoke(tf::Worker&, tf::Node*) [inlined] tf::Executor::_invoke_async_task(node=0x00007ffee4001150, w=0x0000000003958f00, this=0x00007fffffffb800) at executor.hpp:1864:49
    frame #8: 0x00007fff9c0247b0 pen_simulator.cpython-311-x86_64-linux-gnu.so`tf::Executor::_invoke(this=<unavailable>, worker=<unavailable>, node=0x00007ffee4001150) at executor.hpp:1579:25
    frame #9: 0x00007fff9c02589b pen_simulator.cpython-311-x86_64-linux-gnu.so`tf::Executor::_spawn(unsigned long)::'lambda'(tf::Worker&, std::mutex&, std::condition_variable&, unsigned long&)::operator()(tf::Worker&, std::mutex&, std::condition_variable&, unsigned long&) const [inlined] tf::Executor::_exploit_task(t=0x00007fff13ffee80, w=0x0000000003958f00, this=<unavailable>) at executor.hpp:1322:12
    frame #10: 0x00007fff9c02587e pen_simulator.cpython-311-x86_64-linux-gnu.so`tf::Executor::_spawn(__closure=0x0000000002e03818, w=0x0000000003958f00, mutex=<unavailable>, cond=0x00007fffffffb480, n=<unavailable>)::'lambda'(tf::Worker&, std::mutex&, std::condition_variable&, unsigned long&)::operator()(tf::Worker&, std::mutex&, std::condition_variable&, unsigned long&) const at executor.hpp:1215:24
    frame #11: 0x00007fffaf786bf4 libstdc++.so.6`execute_native_thread_routine at thread.cc:82:18
    frame #12: 0x00007ffff7f79609 libpthread.so.0`start_thread(arg=<unavailable>) at pthread_create.c:477:8
    frame #13: 0x00007ffff7d42353 libc.so.6`__clone at clone.S:95
'''
