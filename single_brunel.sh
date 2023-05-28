
# nsyns=(10000000 20000000 30000000 40000000 50000000 60000000 70000000 80000000 90000000 100000000)
# nsyns=(20000000 40000000 60000000 80000000 100000000 120000000 140000000 160000000 180000000 200000000)
nsyns=(120000000 140000000 160000000 180000000)
rates=(0.002)
# rates=(0.01 0.05 0.1)
cd build/bin
chmod +x lif_unit_test
#输出文字信息
for nsyn in "${nsyns[@]}"
do
    for rate in "${rates[@]}"
    do
        nvprof ./lif_unit_test --nsyn=$nsyn --net=brunel --rate=$rate > ../../benchdata/single/bsim_brunel_${nsyn}_${rate}_nvprof.txt 2>&1
        echo done:$nsyn,$rate
    done
done
#输出nvvp文件信息
# for nsyn in "${nsyns[@]}"
# do
#     nvprof -o /home/cuimy/zfz/MGBrain-new/benchdata/single/mgbrain_brunel_${nsyn}_nvprof.nvvp /home/cuimy/zfz/MGBrain-new/build/bench/single_brunel --nsyn=$nsyn
#     echo done:$nsyn
# done