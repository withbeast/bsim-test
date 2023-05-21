nsyns=(100000000 200000000 300000000 400000000 500000000 600000000 700000000 800000000 900000000) 
nparts=(2 4 8)
#输出文字信息
for npart in "${nparts[@]}"
do
    for nsyn in "${nsyns[@]}"
    do
        /home/cuimy/zfz/bsim-test/build/bin/lif_unit_test --nsyn=$nsyn --npart=$npart --net=vogel 2>&1 | tee /home/cuimy/zfz/bsim-test/benchdata/sim/bsim_vogel_${npart}_${nsyn}.txt
        echo done:$npart,$nsyn
    done
done