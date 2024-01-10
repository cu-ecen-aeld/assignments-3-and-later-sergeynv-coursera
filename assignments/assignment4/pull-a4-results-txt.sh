#!/bin/bash

# Regarding "-O": 
# https://forum.openwrt.org/t/ash-usr-libexec-sftp-server-not-found-when-using-scp/125772/3
scp -O -P 10022 root@localhost:/tmp/assignment4-result.txt .
