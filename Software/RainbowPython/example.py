#!/bin/env python3
# -*- coding: utf-8 -*-
from RainbowPython import Rainbow
from time import sleep

rbp = Rainbow()
while True:
	for i in range(1, 256):
		rbp.setColorRGB(i, i, i)
		sleep(0.01)

	for i in range(1, 256):
		i = 256 - i
		rbp.setColorRGB(i, i, i)
		sleep(0.01)
