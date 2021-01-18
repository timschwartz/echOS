#!/bin/bash

source envvars.sh
sudo udisksctl loop-setup -f ${DEV_ROOT}/images/disk.img
mount ${LOOP_TARGET}
