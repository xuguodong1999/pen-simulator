import cv2
import numpy as np
from PIL.PngImagePlugin import PngImageFile
from datasets import load_dataset

import az_init  # noqa # pylint: disable=unused-import

if __name__ == '__main__':
    ps_mul_dataset = load_dataset(
        './ps_mul_dataset/ps_mul_dataset.py',
        trust_remote_code=True,
    )
    print(ps_mul_dataset)
    for idx, sample in enumerate(ps_mul_dataset['train']):
        png_im: PngImageFile = sample['image']
        cv_im = np.array(png_im)
        cv_im = cv2.cvtColor(cv_im, cv2.COLOR_RGB2BGR)
        # cv2.imshow('hello world', cv_im)
        # cv2.waitKey(0)
        cv2.imwrite(f'/mnt/d/TEMP/1/{idx}.png', cv_im)
