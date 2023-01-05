#!/bin/bash

mkfifo fifo1
./stream_pull1 < fifo1  &
./stream_push < ./hello.txt | tee fifo1 | ./stream_pull2
rm fifo1
