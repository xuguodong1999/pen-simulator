import torch
from diffusers import DiffusionPipeline

if __name__ == '__main__':
    model_path = './dist/sd-pokemon-model-lora-sdxl/'
    pipe = DiffusionPipeline.from_pretrained(model_path, torch_dtype=torch.float16)
    pipe.to('cuda')

    prompt = 'A pokemon with green eyes and red legs.'
    image = pipe(prompt, num_inference_steps=30, guidance_scale=7.5).images[0]
    image.save('pokemon.png')
