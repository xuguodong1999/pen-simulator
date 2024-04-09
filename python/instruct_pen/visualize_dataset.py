import argparse

import cv2
import numpy as np
from PIL.PngImagePlugin import PngImageFile
from datasets import Dataset, load_dataset

import az_init  # noqa # pylint: disable=unused-import
import pen_simulator as ps
from instruct_pen.ps_mul_dataset.utils import get_ps_inputs, ps_generator_mt, get_ps_features


def get_ps_dataset_mp():
    return load_dataset(
        './ps_mul_dataset/ps_mul_dataset_mpl.py',
        trust_remote_code=True,
        num_proc=20,
    )


def get_ps_dataset_mt():
    return load_dataset(
        './ps_mul_dataset/ps_mul_dataset.py',
        trust_remote_code=True,
    )


def get_ps_dataset_mt2():
    def ps_generator_wrapper():
        inputs = get_ps_inputs(8888)
        return map(lambda x: x[1], ps_generator_mt(inputs))

    return Dataset.from_generator(
        generator=ps_generator_wrapper,
        features=get_ps_features(),
    )


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='visualization example of pen_simulator')
    parser.add_argument(
        '--couch', type=str, help='path to SCUT_IRAC/Couch',
        metavar='path/to/SCUT_IRAC/Couch',
    )
    parser.add_argument(
        '--makemeahanzi', type=str, help='path to makemeahanzi/graphics.txt',
        metavar='path/to/makemeahanzi/graphics.txt',
    )
    args = parser.parse_args()
    # ps_mul_dataset = get_ps_dataset_mp()
    # ps_mul_train = ps_mul_dataset['train']

    ps.global_init_couch(args.couch, ['Couch_Digit_195', 'Couch_Letter_195', 'Couch_Symbol_130', ])
    ps.global_init_makemeahanzi(args.makemeahanzi)
    ps_mul_dataset = get_ps_dataset_mt()
    ps_mul_train = ps_mul_dataset['train']

    print(ps_mul_dataset)
    ps_mul_dataset.save_to_disk('/tmp/1/ps-multiplication')
    pass
    # ps_mul_train = get_ps_dataset_mt2()
    max_sample_count = 8
    samples = []
    for idx, sample in enumerate(ps_mul_train.shuffle()):
        if idx >= max_sample_count:
            break
        png_im: PngImageFile = sample['image']
        cv_im = np.array(png_im)
        cv_im = cv2.cvtColor(cv_im, cv2.COLOR_RGB2BGR)
        samples.append((sample['instruction'], cv_im))
    for idx, (instruction, image) in enumerate(samples):
        print(f'instruction={instruction}')
        # cv2.imshow('hello world', image)
        # cv2.waitKey(0)
        cv2.imwrite(f'/tmp/1/{idx}.png', image)
        # cv2.imwrite(f'/mnt/d/TEMP/1/{idx}.png', image)
