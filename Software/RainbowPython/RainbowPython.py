#!/bin/env python3
# -*- coding: utf-8 -*-
import colorsys
import serial

class Rainbow:
	# Magic numbers
	_escapeSeq = b'\x1b'
	_startSeq = b'\x02'
	_stopSeq = b'\x03'
	_initSeq = _escapeSeq + _startSeq
	_endtransSeq = _escapeSeq + _stopSeq

	_serialDev = None
	def __init__(self, device="/dev/ttyUSB0", baudrate=38400):
		self._serialDev = serial.Serial(device, baudrate)

	def setColorRGB(self, red, green, blue):
		if(red > 255 or green > 255 or blue > 255 or red < 0 or green < 0 or blue < 0):
			raise Exception("ColorOutOfBounds")
			return

		redHex = bytes([red])
		greenHex = bytes([green])
		blueHex = bytes([blue])
		
		if(redHex == self._escapeSeq): redHex = self._escapeSeq*2
		if(greenHex == self._escapeSeq): greenHex = self._escapeSeq*2
		if(blueHex == self._escapeSeq): blueHex = self._escapeSeq*2

		#TODO: Calculate checksum
		checksum = b'\x00'

		self._serialDev.write(self._initSeq + redHex + greenHex + blueHex + checksum + self._endtransSeq)

	def close():
		self._serialDev.close()
