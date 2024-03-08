video_duration=5
frames_root_dir=/mnt/wsl/PHYSICALDRIVE6p1/datasets/cache/frames
video_output_dir=/mnt/wsl/PHYSICALDRIVE6p1/datasets/cache/videos_0

for i in $(seq 0 999);
do
    image_prefix="$i"_
    ffmpeg_cmd="ffmpeg -framerate $(find $frames_root_dir -type f -name "$image_prefix*" -printf '.' | wc -m)/$video_duration -start_number 0 -i $frames_root_dir/$image_prefix%08d.png -c:v libx264 -r 24 -pix_fmt yuv420p -vf tpad=stop_mode=clone:stop_duration=0.5 $video_output_dir/"$image_prefix"video.mp4 -y"
#   echo $ffmpeg_cmd
   $ffmpeg_cmd &
done