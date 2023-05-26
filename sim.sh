# nsyns=(750000000) # speedup

# nsyns=(250000000 500000000 750000000)
nsyns=(1000000000) # sim time
nparts=(8) #sim time
# nparts=(2 4) # speedup
# nparts=(1)
# rates=(0.01 0.05)
rates=(0.1 0.2)
# rates=(0.01 0.05 0.1 0.2)
cd build/bin
#输出文字信息
echo "start sim bench"
for npart in "${nparts[@]}"
do
    for nsyn in "${nsyns[@]}"
    do
        for rate in "${rates[@]}"
        do
            ./lif_unit_test --net=brunel --nsyn=$nsyn --npart=$npart --rate=$rate >> ./bsim_brunel_speedup_$npart.txt
            echo finish:$npart,$nsyn,$rate
        done 
    done
done

# for npart in "${nparts[@]}"
# do
#     for nsyn in "${nsyns[@]}"
#     do
#         for rate in "${rates[@]}"
#             /home/zhangfzh9/Lab/SNNSims/bsim-test/build/bin/lif_unit_test --net=vogel --nsyn=$nsyn --npart=$npart --rate=$rate 2>&1 | tee /home/zhangfzh9/Lab/SNNSims/bsim-test/benchdata/sim/bsim_vogel.txt
#             echo done:$npart,$nsyn
#         done 
#     done
# done

