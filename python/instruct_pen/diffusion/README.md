# train a diffusion model to do a sum in writing

## sdxl lora training script
https://github.com/huggingface/diffusers/blob/main/examples/text_to_image/train_text_to_image_lora_sdxl.py

```bash
export MODEL_NAME="$HOME/weights/stabilityai/stable-diffusion-xl-base-1.0"
export VAE_NAME="$HOME/weights/madebyollin/sdxl-vae-fp16-fix"
export DATASET_NAME="$HOME/datasets/lambdalabs/pokemon-blip-captions"
accelerate launch train_text_to_image_lora_sdxl.py \
  --pretrained_model_name_or_path=$MODEL_NAME \
  --pretrained_vae_model_name_or_path=$VAE_NAME \
  --dataset_name=$DATASET_NAME --caption_column="text" \
  --resolution=1024 --random_flip \
  --resume_from_checkpoint "latest" \
  --train_batch_size=2 \
  --num_train_epochs=0 --checkpointing_steps=500 \
  --learning_rate=1e-04 --lr_scheduler="constant" --lr_warmup_steps=0 \
  --mixed_precision="fp16" \
  --seed=42 \
  --output_dir="./dist/sd-pokemon-model-lora-sdxl" \
  --validation_prompt="cute dragon creature"
```
