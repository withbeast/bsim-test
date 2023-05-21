nsyns=(10000000 20000000 30000000 40000000 50000000 60000000 70000000 80000000 90000000 100000000)
#输出文字信息
for nsyn in "${nsyns[@]}"
do
    nvprof /home/cuimy/zfz/bsim-test/build/bin/lif_unit_test --nsyn=$nsyn --npart=1 --net=vogel 2>&1 | tee /home/cuimy/zfz/bsim-test/benchdata/single/bsim_vogel_${nsyn}_nvprof.txt
    echo done:$nsyn
done