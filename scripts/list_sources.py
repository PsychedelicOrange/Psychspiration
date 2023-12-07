import subprocess
import sys
result = subprocess.run(['ls',sys.argv[1]], stdout=subprocess.PIPE)
list = result.stdout.decode('utf-8')
files = list.split()
paths = [sys.argv[1]+x for x in files]
for path in paths:
	print(path)
