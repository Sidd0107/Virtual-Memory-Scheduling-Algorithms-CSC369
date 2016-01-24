ALGORITHM 1:

First In First Out

Mem Size=50

./sim -f tr-simpleloop.ref -m 50 -s 1000000 -a fifo

Hit count: 5537
Miss count: 5023
Clean evictions: 1692
Dirty evictions: 3281
Total references : 10560
Hit rate: 52.4337
Miss rate: 47.5663

Mem Size=100

./sim -f tr-simpleloop.ref -m 100 -s 1000000 -a fifo

Hit count: 7338
Miss count: 3222
Clean evictions: 139
Dirty evictions: 2983
Total references : 10560
Hit rate: 69.4886
Miss rate: 30.5114

Mem Size=150

./sim -f tr-simpleloop.ref -m 150 -s 1000000 -a fifo

Hit count: 7510
Miss count: 3050
Clean evictions: 0
Dirty evictions: 2900
Total references : 10560
Hit rate: 71.1174
Miss rate: 28.8826

Mem Size=200

./sim -f tr-simpleloop.ref -m 200 -s 1000000 -a fifo

Hit count: 7519
Miss count: 3041
Clean evictions: 0
Dirty evictions: 2841
Total references : 10560
Hit rate: 71.2027
Miss rate: 28.7973


ALGORITHM 2:

Least Recently Used

Mem Size=50

./sim -f tr-simpleloop.ref -m 50 -s 1000000 -a lru

Hit count: 7782
Miss count: 2778
Clean evictions: 91
Dirty evictions: 2637
Total references : 10560
Hit rate: 73.6932
Miss rate: 26.3068

Mem Size=100

./sim -f tr-simpleloop.ref -m 100 -s 1000000 -a lru

Hit count: 7881
Miss count: 2679
Clean evictions: 0
Dirty evictions: 2579
Total references : 10560
Hit rate: 74.6307
Miss rate: 25.3693

Mem Size=150

./sim -f tr-simpleloop.ref -m 150 -s 1000000 -a lru

Hit count: 7883
Miss count: 2677
Clean evictions: 0
Dirty evictions: 2527
Total references : 10560
Hit rate: 74.6496
Miss rate: 25.3504

Mem Size=200

./sim -f tr-simpleloop.ref -m 200 -s 1000000 -a lru

Hit count: 7883
Miss count: 2677
Clean evictions: 0
Dirty evictions: 2477
Total references : 10560
Hit rate: 74.6496
Miss rate: 25.3504


