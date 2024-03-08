if [ _$1 == _ ]; then
  echo usage: ./adb-run-file.sh your-binary-file
elif [ ! -f "$1" ]; then
  echo "file $1 not found!"
else
  to_run_file=$1
  echo "preparing $to_run_file ..."

  sdcard_tmp_dir=sdcard/Download/tmp
  adb shell mkdir -p $sdcard_tmp_dir
  adb push "$to_run_file" "$sdcard_tmp_dir/$to_run_file"

  exec_tmp_dir=data/local/tmp
  adb shell mkdir -p $exec_tmp_dir
  adb shell mv "$sdcard_tmp_dir/$to_run_file" "$exec_tmp_dir/$to_run_file"
  adb shell chmod 755 "$exec_tmp_dir/$to_run_file"
  adb shell "$exec_tmp_dir/$to_run_file"
fi