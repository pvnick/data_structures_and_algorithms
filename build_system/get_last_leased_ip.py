import fileinput
import re

last_ip = ""
for line in fileinput.input():
	match = re.findall(r"^lease (\d+\.\d+\.\d+\.\d+)", line)
	if match:
		last_ip = match[0]
print(last_ip)