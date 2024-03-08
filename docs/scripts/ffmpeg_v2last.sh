last_output_dir=/mnt/wsl/PHYSICALDRIVE6p1/datasets/cache/videos

#for scale in 2;
for scale in 2 8 32 128;
do
    last_scale=0
    echo $last_scale $scale
    video_input_dir1=/mnt/wsl/PHYSICALDRIVE6p1/datasets/cache/videos_$last_scale
    video_count1=$(find $video_input_dir1 -type f -name "*.mp4" -printf '.' | wc -m)

    video_input_dir2=/mnt/wsl/PHYSICALDRIVE6p1/datasets/cache/videos_$scale
    video_count2=$(find $video_input_dir2 -type f -name "*.mp4" -printf '.' | wc -m)

    for i in $(seq 0 10);
    do
        ffmpeg  \
        -itsscale 0.5 -i $video_input_dir1/$[(2 * $RANDOM + 1) % $video_count1]_video.mp4 \
        -itsscale 0.5 -i $video_input_dir1/$[(2 * $RANDOM + 1) % $video_count1]_video.mp4 \
        -itsscale 0.5 -i $video_input_dir2/$[$RANDOM % $video_count2]_video.mp4 \
        -itsscale 0.5 -i $video_input_dir2/$[$RANDOM % $video_count2]_video.mp4 \
        -filter_complex "\
        [0:v][2:v]hstack=inputs=2[in1]; \
        [1:v][3:v]hstack=inputs=2[in2]; \
        [in1][in2]concat=n=2:v=1[v]" \
        -map "[v]" \
        -s 2560x1280 -c:a copy \
        $last_output_dir/"$scale"_"$i"_video.mp4 -y
#        break
    done
#    break
done
