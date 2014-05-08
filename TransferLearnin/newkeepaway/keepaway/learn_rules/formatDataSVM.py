#!/usr/bin/python

from __future__ import print_function



if __name__ == '__main__':
	print("processing data file")

	data_file = open("best_mix.data", 'r')
	output_file = open("best_mix.libsvm", 'w')
	data_file_lines = data_file.readlines()
	for data_line in data_file_lines:
		data_line = data_line.strip("\n").strip(" ")
		if (data_line != ""):
			elements = data_line.split(" ")
			if (len(elements) != 16):
				print("Unexpected number of elements per line")
			else:
				action = elements[-3]
				state = elements[:-3]
				#print("action " + str(action))
				#print("state " + str(state))
				line_string = str(action)
				for index, dim in enumerate(state):
					dim = float(dim)
					if index < 5:
						dim = dim/25
					elif index < 9:
						dim = dim/15
					elif index < 11:
						dim = dim/30
					else:
						dim = dim/180
					line_string += (" " + str(index+1) + ":" + str(dim))
				line_string += "\n"
				output_file.write(line_string)
	output_file.close()

