N=24
#for scale in 128;
for scale in 2 8 32 128;
do
    last_scale=$[$scale/4]
    echo $last_scale $scale
    video_input_dir=/mnt/wsl/PHYSICALDRIVE6p1/datasets/cache/videos_$last_scale
    video_output_dir=/mnt/wsl/PHYSICALDRIVE6p1/datasets/cache/videos_$scale
    video_count=$(find $video_input_dir -type f -name "*.mp4" -printf '.' | wc -m)
    for i in $(seq 0 4 $[$video_count-4]);
    do
        ((n=n%N)); ((n++==0)) && wait
        ffmpeg  \
        -i $video_input_dir/$[$i+1]_video.mp4 \
        -i $video_input_dir/$[$i]_video.mp4 \
        -i $video_input_dir/$[$i+3]_video.mp4 \
        -i $video_input_dir/$[$i+2]_video.mp4 \
        -filter_complex "\
        [0:v][1:v]hstack=inputs=2[top];\
        [2:v][3:v]hstack=inputs=2[bottom];\
        [top][bottom]vstack=inputs=2[v]" -map "[v]" \
        -s 1280x1280 -c:a copy \
        "$video_output_dir/$[$i/4]"_video.mp4 -y &
#        break
    done
#    break
done
