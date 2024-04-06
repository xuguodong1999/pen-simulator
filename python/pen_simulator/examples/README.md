# pen-simulator examples

## Resources

- SCUT_IRAC/Couch: www.hcii-lab.net/data/scutcouch/EN/download.html
- makemeahanzi: https://github.com/skishore/makemeahanzi

## Guide

```bash
conda create -n ps python=3.11
conda activate ps

# install prebuilt wheel
pip install pen_simulator-0.0.1-cp311-cp311-linux_x86_64.whl

# OR build wheel from sources
pip install pybind11 numpy
python setup.py bdist_wheel
pip install dist/*.whl
```

```bash
pip install matplotlib svgpath2mpl
# this example do not need external data source
python visualize_mpl_svg_only.py
```

```bash
python visualize.py --couch path/to/SCUT_IRAC/Couch --makemeahanzi path/to/makemeahanzi/graphics.txt
```

```bash
pip install opencv-python
python visualize_cv2.py --couch path/to/SCUT_IRAC/Couch --makemeahanzi path/to/makemeahanzi/graphics.txt
```

```bash
pip install matplotlib svgpath2mpl
python visualize_mpl.py --couch path/to/SCUT_IRAC/Couch --makemeahanzi path/to/makemeahanzi/graphics.txt
```

## Argument Reference

```txt
# Linux: --couch /mnt/data/datasets/SCUT_IRAC/Couch --makemeahanzi /home/xgd/git/_/web/makemeahanzi/graphics.txt
# WSL2: --couch /home/xgd/datasets/SCUT_IRAC/Couch --makemeahanzi /home/xgd/git/_/web/makemeahanzi/graphics.txt
# Windows: --couch D:/datasets/ntfs/SCUT_IRAC/Couch --makemeahanzi D:/git/_/web/makemeahanzi/graphics.txt
# Mac: --couch /Users/xgd/datasets/SCUT_IRAC/Couch --makemeahanzi /Users/xgd/git/_/web/makemeahanzi/graphics.txt
```

## play with sdxl lora

```bash
export MODEL_NAME="$HOME/weights/stabilityai/stable-diffusion-xl-base-1.0"
export VAE_NAME="$HOME/weights/madebyollin/sdxl-vae-fp16-fix"
export DATASET_NAME="$HOME/datasets/lambdalabs/pokemon-blip-captions"
accelerate launch train_text_to_image_lora_sdxl.py \
  --pretrained_model_name_or_path=$MODEL_NAME \
  --pretrained_vae_model_name_or_path=$VAE_NAME \
  --dataset_name=$DATASET_NAME --caption_column="text" \
  --resolution=1024 --random_flip \
  --train_batch_size=2 \
  --num_train_epochs=2 --checkpointing_steps=500 \
  --learning_rate=1e-04 --lr_scheduler="constant" --lr_warmup_steps=0 \
  --mixed_precision="fp16" \
  --seed=42 \
  --output_dir="./dist/sd-pokemon-model-lora-sdxl" \
  --validation_prompt="cute dragon creature"
```